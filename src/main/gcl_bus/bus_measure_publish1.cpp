#include "bus_measure_publish.h"


#include <vdi/measure_sharememory_all.h>
#include "bus_terminal.h"


using namespace std;


//定义实时改变发送区的缓存大小
#define f_iPublishPacketDataCount (1024 * 5)

//定义一数据包的大小
#define f_iPublishPacketDataSize    (1024 * 2 - sizeof(psm_packet_body_t) - 16)


size_t f_monsbPublishedIndex = 0;
size_t f_ycaddPublishedIndex = 0;
size_t f_strawPublishedIndex = 0;


BusMeasurePublisher * fn_oBusMeasurePublisher()
{
    CxApplication::registStartFunction(BusMeasurePublisher::start);
    CxApplication::registStopFunction(BusMeasurePublisher::stop);

    static BusMeasurePublisher m;
    return &m;
}

static BusMeasurePublisher * f_oBusRealtimedataPublisher = fn_oBusMeasurePublisher();




template<typename T>
class MeasurePublishBond
{
public:
#pragma pack(push,4)
    struct MeasureBlock {
        int address;
        T value;
        int quality;
        long long datetime;
    };
#pragma pack(pop)

    struct PublishPacket
    {
        int terminalId;
        int structTypeId;
        int dataLength;
        char * data;
        int dataCount;
        vector<MeasureBlock> measureBlocks;
        
        void resetData() {
            data = (char*)measureBlocks.data();
            dataLength = measureBlocks.size() * sizeof(MeasureBlock);
            dataCount = measureBlocks.size();
        }

    };

    struct TimerAllMeasure
    {
        int terminalId;
        int interval;
        vector<int> measureIds;
        msepoch_t nextTime;

        TimerAllMeasure(int iTid) :
            terminalId(iTid), interval(0), nextTime(0)
        { }
    };

protected:
    int _measureMinId;
    int _measureCount;
    int _measureType;
    int _structTypeId;
    vector<vector<int> > _measureTerminalIds;
    vector<TimerAllMeasure> _timerAllMeasures;

public:
    MeasurePublishBond() :
        _measureMinId(0), _measureCount(0), _measureType(0), _structTypeId(0)
    { }
    
    void init(int iMinId, int iCount, int iMeasureType, int iStructTypeId) {
        _measureMinId = iMinId;
        _measureCount = iCount;
        _measureType = iMeasureType;
        _structTypeId = iStructTypeId;
        _measureTerminalIds.resize(iCount);
    }

    TimerAllMeasure * allocate(int iTerminalId) {
        TimerAllMeasure * oTimerAllMeasure = NULL;
        for (size_t i = 0; i < _timerAllMeasures.size(); ++i)
        {
            TimerAllMeasure & timerAllMeasure = _timerAllMeasures.at(i);
            if (timerAllMeasure.terminalId == iTerminalId)
            {
                oTimerAllMeasure = & timerAllMeasure;
                break;
            }
        }
        if (! oTimerAllMeasure)
        {
            _timerAllMeasures.push_back(TimerAllMeasure(iTerminalId));
            oTimerAllMeasure = & ( _timerAllMeasures.back() );
        }
        return oTimerAllMeasure;
    }

    void update( int iMeasureId, int iTerminalId ) {
        size_t iIndex = iMeasureId - _measureMinId;
        if (iIndex < _measureTerminalIds.size())
        {
            vector<int> & terminalIds = _measureTerminalIds.at(iIndex);
            if (! CxContainer::contain(terminalIds, iTerminalId))
            {
                terminalIds.push_back(iTerminalId);
            }
        }
        TimerAllMeasure * oTimerAllMeasure = allocate(iTerminalId);
        CxContainer::update(oTimerAllMeasure->measureIds, iMeasureId);
    }

    void updateTimerAllMeasure( int iTerminalId, int iInterval ) {
        TimerAllMeasure * oTimerAllMeasure = allocate(iTerminalId);
        oTimerAllMeasure->interval = iInterval;
        oTimerAllMeasure->nextTime = 0;
    }

    void clear( int iTerminalId ) {
        size_t iSize = _measureTerminalIds.size();
        for (int i = 0; i < iSize; ++i)
        {
            vector<int> & terminalIds = _measureTerminalIds.at(i);
            CxContainer::remove(terminalIds, iTerminalId);
        }
    }

    int update(const std::map<string, string> &row) {
        int iResult = FALSE;
        int iTerminalId = 0;

        iTerminalId = CxContainer::valueTo(row, string("terminal"), iTerminalId);
        GM_INVALID_RETURE_(iTerminalId>0, iResult);

        std::string sSubscibes = CxContainer::value(row, string("bookms"));
        if (sSubscibes.size() > 0)
        {
            //更新带 ‘~' 段式
            if (CxString::contain(sSubscibes, "~")) {
                string sMeasureBegin = CxString::token(sSubscibes, '~');
                string sMeasureEnd = sSubscibes;
                int iMeasureBegin = CxString::toInt32(sMeasureBegin);
                int iMeasureEnd = CxString::toInt32(sMeasureEnd);
                if (g_fn_getMeasureType(iMeasureBegin) == _measureType)
                {
                    for (int iMeasureId = iMeasureBegin; iMeasureId <= iMeasureEnd; ++iMeasureId)
                    {
                        update( iMeasureId, iTerminalId );
                    }
                    iResult = TRUE;
                }
            }
            //更新单个
            else
            {
            }
        }
        else
        {
            int iInterval = CxContainer::valueTo(row, string("interval"), ci_int_zero);
            std::string sType = CxContainer::value(row, string("type"));
            if (sType == g_fn_getMeasureTypeName(_measureType))
            {
                updateTimerAllMeasure(iTerminalId, iInterval);
            }
        }
        return iResult;
    }

    std::vector<std::string> reportSelf() {
        vector<string> sReports;
        sReports.push_back( CxString::format("*** MeasurePublishBond.%s Report ***" , g_fn_getMeasureTypeName(_measureType).c_str() ) );
        sReports.push_back(CxString::format("MeasureMinId=", _measureMinId));
        sReports.push_back(CxString::format("MeasureCount=", _measureCount));
        sReports.push_back(CxString::format("StructTypeId=", _structTypeId));
        for (size_t i = 0; i < _timerAllMeasures.size(); ++i)
        {
            TimerAllMeasure & timerAllMeasure = _timerAllMeasures.at(i);
            sReports.push_back(CxString::format("TimerAllMeasure=%s terminalId=%d interval=%d", g_fn_getMeasureTypeName(_measureType).c_str(), timerAllMeasure.terminalId, timerAllMeasure.interval));
        }
        std::string sMeasureTerminalIds("MeasureTerminalIds=");
        int iMeasureId = _measureMinId;
        for (size_t i = 0; i < _measureTerminalIds.size(); ++i)
        {
            const vector<int> & terminalIds = _measureTerminalIds.at(i);
            sMeasureTerminalIds += CxString::toHexstring(iMeasureId++) + "+" + CxContainer::joinToHexstring(terminalIds,',') + " ";
        }
        sReports.push_back(sMeasureTerminalIds);
        return sReports;
    }

    const vector<int> * getMeasureTerminals( int iMeasureId ) {
        size_t iIndex = iMeasureId - _measureMinId;
        if (iIndex < _measureTerminalIds.size())
            return &(_measureTerminalIds.at(iIndex));
        else
            return NULL;
    }

    int publishChanged(const typename MeasureShareMemory<T>::ChangedData * const oChangedBegin, const typename MeasureShareMemory<T>::ChangedData * const oChangedEnd,
                              PublishPacket * * const oChangedPacketsBegin, PublishPacket * * const oChangedPacketsEnd, int iChangedPacketIndex)
    {
        static size_t ci_maxMeasureBlockCount = f_iPublishPacketDataSize / sizeof(MeasureBlock);
        GM_INVALID_RETURE_(_measureTerminalIds.size() > 0, iChangedPacketIndex);

        vector<PublishPacket *> fullChangedPackets;  //已经装满了(==iMaxCount) ChangedPacket
        map<int, PublishPacket *> ingChangedPackets; //正处理的 ChangedPackets
        const typename MeasureShareMemory<T>::ChangedData * oChangedData = oChangedBegin;
        PublishPacket * oChangedPacketNULL = NULL;
        PublishPacket * oChangedPacket;
        MeasureBlock mChangedBlock;
        int iTerminalId;
        const vector<int> * oMeasureTerminals;


        cxTrack(ci_track_bus_publish) << " publishChanged MaxCount << " << ci_maxMeasureBlockCount;
        cxTrack(ci_track_bus_publish) << " publishChanged count << " << oChangedEnd - oChangedData;

        //遍历 oChangedData --> 组织成 ingChangedPackets --> fullChangedPackets
        while (oChangedData < oChangedEnd)
        {
            int iMeasureId = oChangedData->measureId;
            //小心这里，_measureTerminalIds 初始化时是共享内存的 measureCount，如果有错也是初始化的错
            oMeasureTerminals = & ( _measureTerminalIds.at(iMeasureId - _measureMinId) );
            if (oMeasureTerminals->size()>0)
            {
                mChangedBlock.address = iMeasureId;
                mChangedBlock.value = oChangedData->value;
                mChangedBlock.quality = oChangedData->quality;
                mChangedBlock.datetime = oChangedData->changedTime;

                for (size_t i = 0; i < oMeasureTerminals->size(); ++i)
                {
                    iTerminalId = oMeasureTerminals->at(i);
                    oChangedPacket = CxContainer::value(ingChangedPackets, iTerminalId, oChangedPacketNULL);
                    //
                    if (oChangedPacket)
                    {
                        if (oChangedPacket->measureBlocks.size() == ci_maxMeasureBlockCount)
                        {
                            oChangedPacket->resetData();
                            fullChangedPackets.push_back(oChangedPacket);
                            oChangedPacket = new PublishPacket();
                            ingChangedPackets[iTerminalId] = oChangedPacket;
                        }
                    }
                    else
                    {
                         oChangedPacket = new PublishPacket();
                         ingChangedPackets[iTerminalId] = oChangedPacket;
                    }
                    if (oChangedPacket->measureBlocks.empty())
                    {
                        oChangedPacket->terminalId = iTerminalId;
                        oChangedPacket->structTypeId = _structTypeId;
                    }

                    oChangedPacket->measureBlocks.push_back(mChangedBlock);
                }
            }
            oChangedData++;
        }

        //发布到缓存中
        PublishPacket * * pOChangedPacket = oChangedPacketsBegin + iChangedPacketIndex;
        for (size_t i = 0; i < fullChangedPackets.size(); ++i)
        {
            if (* pOChangedPacket != NULL)
            {
                string sError = CxString::format("error error error !!! Bus %s Publish - PublishPacket Cache Full (* pOChangedPacket != NULL) , Can not publish !!!", g_fn_getMeasureTypeName(_measureType).c_str());
                cxWarning() << sError;
                cout        << sError << endl;
                break;
            }
            * pOChangedPacket = fullChangedPackets.at(i);
            pOChangedPacket ++;
            if (pOChangedPacket >= oChangedPacketsEnd)
            {
                pOChangedPacket = oChangedPacketsBegin;
            }
        }

        for(typename map<int, PublishPacket *>::const_iterator it = ingChangedPackets.begin(); it != ingChangedPackets.end(); ++it)
        {
            if (* pOChangedPacket != NULL)
            {
                string sError = CxString::format("error error error !!! Bus %s Publish - PublishPacket Cache Full (* pOChangedPacket != NULL) , Can not publish !!!", g_fn_getMeasureTypeName(_measureType).c_str());
                cxWarning() << sError;
                cout        << sError << endl;
                break;
            }
            PublishPacket * oChangedPacket = it->second;
            oChangedPacket->resetData();
            * pOChangedPacket = oChangedPacket;
            pOChangedPacket ++;
            if (pOChangedPacket >= oChangedPacketsEnd)
            {
                pOChangedPacket = oChangedPacketsBegin;
            }
        }
        return pOChangedPacket - oChangedPacketsBegin;
    }

    int publishAllMeasure(msepoch_t dtNow, typename MeasureShareMemory<T>::MemoryManagerBase * oMeasureManager,
                          PublishPacket * * const oPacketsBegin, PublishPacket * * const oPacketsEnd, int iPacketIndex) {
        static size_t ci_maxMeasureBlockCount = f_iPublishPacketDataSize / sizeof(MeasureBlock);

        //发布到缓存中
        PublishPacket * * pPacket = oPacketsBegin + iPacketIndex;
        PublishPacket * oPacket = NULL;
        MeasureBlock mBlock;
        mBlock.datetime = dtNow;
        int iTerminalId;

        for (size_t i = 0; i < _timerAllMeasures.size(); ++i)
        {
            TimerAllMeasure & timerAllMeasure = _timerAllMeasures.at(i);
            if (timerAllMeasure.interval > 0 && dtNow > timerAllMeasure.nextTime)
            {
                iTerminalId = timerAllMeasure.terminalId;
                vector<int> & measureIds = timerAllMeasure.measureIds;
                for (size_t i = 0; i < measureIds.size(); ++i)
                {
                    int iMeasureId = measureIds.at(i);
                    mBlock.address = iMeasureId;

                    //从共享内存中读数据
                    typename MeasureShareMemory<T>::MeasureData * oMeasure = oMeasureManager->measureData(iMeasureId);
                    mBlock.value = oMeasure->value;
                    mBlock.quality = oMeasure->quality;

                    if (oPacket == NULL)
                    {
                        oPacket = new PublishPacket();
                        oPacket->terminalId = iTerminalId;
                        oPacket->structTypeId = _structTypeId;
                    }
                    oPacket->measureBlocks.push_back(mBlock);
                    if (oPacket->measureBlocks.size() == ci_maxMeasureBlockCount)
                    {
                        oPacket->resetData();

                        //放发送缓存中
                        if (* pPacket != NULL)
                        {
                            string sError = CxString::format("error error error !!! Bus %s Publish - PublishPacket Cache Full (* pOPublishPacket != NULL) , Can not publish !!!", g_fn_getMeasureTypeName(_measureType).c_str());
                            cxWarning() << sError;
                            cout        << sError << endl;
                            break;
                        }
                        * pPacket = oPacket;
                        pPacket ++;
                        if (pPacket >= oPacketsEnd)
                        {
                            pPacket = oPacketsBegin;
                        }

                        oPacket = NULL;
                    }
                }

                //还有数据没放发送缓存中的
                if (oPacket)
                {
                    oPacket->resetData();

                    //放发送缓存中
                    if (* pPacket != NULL)
                    {
                        string sError = CxString::format("error error error !!! Bus %s Publish - PublishPacket Cache Full (* pOChangedPacket != NULL) , Can not publish !!!", g_fn_getMeasureTypeName(_measureType).c_str());
                        cxWarning() << sError;
                        cout        << sError << endl;
                        break;
                    }
                    * pPacket = oPacket;
                    pPacket ++;
                    if (pPacket >= oPacketsEnd)
                    {
                        pPacket = oPacketsBegin;
                    }

                    oPacket = NULL;
                }

                timerAllMeasure.nextTime += timerAllMeasure.interval;

                cxTrack(ci_track_bus_publish) << g_fn_getMeasureTypeName(_measureType) << " publishAllMeasure count=" << measureIds.size();
            }
        }

        return pPacket - oPacketsBegin;
    }

};


//*YX
class MonsbPublishBond : public MeasurePublishBond<int>
{
public:
};
typedef MonsbPublishBond::PublishPacket MonsbPublishPacket;

MonsbPublishPacket * * fn_oMonsbPublishPackets()
{
    static vector<MonsbPublishPacket*> m(f_iPublishPacketDataCount, (MonsbPublishPacket*)NULL);
    return (MonsbPublishPacket * *)(m.data());
}

static MonsbPublishPacket * * const f_oMonsbPublishPackets = fn_oMonsbPublishPackets();
static MonsbPublishPacket * * const f_oMonsbPublishPacketEnd = fn_oMonsbPublishPackets() + f_iPublishPacketDataCount;
static int f_iMonsbPublishPacketIndexPush = 0;
static int f_iMonsbPublishPacketIndexPop = 0;
static MonsbPublishBond f_mMonsbPublishBond;



//*YC
class YcaddPublishBond : public MeasurePublishBond<double>
{
public:
};
typedef YcaddPublishBond::PublishPacket YcaddPublishPacket;

YcaddPublishPacket * * fn_oYcaddPublishPackets()
{
    static vector<YcaddPublishPacket*> m(f_iPublishPacketDataCount, (YcaddPublishPacket*)NULL);
    return (YcaddPublishPacket * *)(m.data());
}

static YcaddPublishPacket * * const f_oYcaddPublishPackets = fn_oYcaddPublishPackets();
static YcaddPublishPacket * * const f_oYcaddPublishPacketEnd = fn_oYcaddPublishPackets() + f_iPublishPacketDataCount;
static int f_iYcaddPublishPacketIndexPush = 0;
static int f_iYcaddPublishPacketIndexPop = 0;
static YcaddPublishBond f_mYcaddPublishBond;



//*YW
class StrawPublishBond : public MeasurePublishBond<StrawValue>
{
public:
};
typedef StrawPublishBond::PublishPacket StrawPublishPacket;

StrawPublishPacket * * fn_oStrawPublishPackets()
{
    static vector<StrawPublishPacket*> m(f_iPublishPacketDataCount, (StrawPublishPacket*)NULL);
    return (StrawPublishPacket * *)(m.data());
}

static StrawPublishPacket * * const f_oStrawPublishPackets = fn_oStrawPublishPackets();
static StrawPublishPacket * * const f_oStrawPublishPacketEnd = fn_oStrawPublishPackets() + f_iPublishPacketDataCount;
static int f_iStrawPublishPacketIndexPush = 0;
static int f_iStrawPublishPacketIndexPop = 0;
static StrawPublishBond f_mStrawPublishBond;



void fn_publishChangeds_monsb(const MonsbShareMemory::ChangedData * const oChangedBegin, const typename MonsbShareMemory::ChangedData * const oChangedEnd)
{
    f_iMonsbPublishPacketIndexPush = f_mMonsbPublishBond.publishChanged(oChangedBegin, oChangedEnd, f_oMonsbPublishPackets, f_oMonsbPublishPacketEnd, f_iMonsbPublishPacketIndexPush);
}

void fn_publishChangeds_ycadd(const YcaddShareMemory::ChangedData * const oChangedBegin, const typename YcaddShareMemory::ChangedData * const oChangedEnd)
{
    f_iYcaddPublishPacketIndexPush = f_mYcaddPublishBond.publishChanged(oChangedBegin, oChangedEnd, f_oYcaddPublishPackets, f_oYcaddPublishPacketEnd, f_iYcaddPublishPacketIndexPush);
}

void fn_publishChangeds_straw(const StrawShareMemory::ChangedData * const oChangedBegin, const typename StrawShareMemory::ChangedData * const oChangedEnd)
{
    f_iStrawPublishPacketIndexPush = f_mStrawPublishBond.publishChanged(oChangedBegin, oChangedEnd, f_oStrawPublishPackets, f_oStrawPublishPacketEnd, f_iStrawPublishPacketIndexPush);
}

class ScanMeasurePublishThread : public CxJoinableThread
{
public:
    void setStatus(int value) {
        _status = value;
    }

    void run() {
        uint iTimes = 0;
        int iCase;
        while (_status)
        {
            iCase = iTimes % 4;
            if (iCase == 0)
            {

            }
            else if (iCase == 1)
            {
                if (MonsbShareMemory::memoryManager()->isValid())
                {
                    f_monsbPublishedIndex = MonsbShareMemory::memoryManager()->checkChangeds(f_monsbPublishedIndex, fn_publishChangeds_monsb);
                    f_iMonsbPublishPacketIndexPush = f_mMonsbPublishBond.publishAllMeasure(CxTime::currentSystemTime(), MonsbShareMemory::memoryManager(), f_oMonsbPublishPackets, f_oMonsbPublishPacketEnd, f_iMonsbPublishPacketIndexPush);
                }
            }
            else if (iCase == 2)
            {
                if (YcaddShareMemory::memoryManager()->isValid())
                {
                    f_ycaddPublishedIndex = YcaddShareMemory::memoryManager()->checkChangeds(f_ycaddPublishedIndex, fn_publishChangeds_ycadd);
                    f_iYcaddPublishPacketIndexPush = f_mYcaddPublishBond.publishAllMeasure(CxTime::currentSystemTime(), YcaddShareMemory::memoryManager(), f_oYcaddPublishPackets, f_oYcaddPublishPacketEnd, f_iYcaddPublishPacketIndexPush);
                }
            }
            else if (iCase == 3)
            {
                if (StrawShareMemory::memoryManager()->isValid())
                {
                    f_strawPublishedIndex = StrawShareMemory::memoryManager()->checkChangeds(f_strawPublishedIndex, fn_publishChangeds_straw);
                    f_iStrawPublishPacketIndexPush = f_mStrawPublishBond.publishAllMeasure(CxTime::currentSystemTime(), StrawShareMemory::memoryManager(), f_oStrawPublishPackets, f_oStrawPublishPacketEnd, f_iStrawPublishPacketIndexPush);
                }
            }
            iTimes++;
            CxThread::sleep(30);
        }
    }

    void waitExit() {
        _status = false; join();
    }

private:
    volatile int _status;

};

ScanMeasurePublishThread * f_oScanMeasurePublishThread = NULL;

template<typename T>
int fn_postPublishPacket(typename MeasurePublishBond<T>::PublishPacket * * const oPublishPacketsBegin, typename MeasurePublishBond<T>::PublishPacket * * const oPublishPacketsEnd,
                          int iPublishPacketIndex)
{
    typename MeasurePublishBond<T>::PublishPacket * * pOPublishPacketEnd = oPublishPacketsBegin + iPublishPacketIndex;
    typename MeasurePublishBond<T>::PublishPacket * * pOPublishPacket = oPublishPacketsBegin + iPublishPacketIndex;
    typename MeasurePublishBond<T>::PublishPacket * oPublishPacket;
    do
    {
        //判断是否收到 push 的 ProcessCallBack
        if (* pOPublishPacket != NULL)
        {
            //复制指针
            oPublishPacket = * pOPublishPacket;
            * pOPublishPacket = NULL;
            pOPublishPacket ++;
            if (pOPublishPacket == oPublishPacketsEnd)
                pOPublishPacket = oPublishPacketsBegin;

            //发送
            BusManager::transferRealtimeDataPost(0, oPublishPacket->structTypeId, oPublishPacket->data, oPublishPacket->dataLength, oPublishPacket->dataCount, oPublishPacket->terminalId);
            delete oPublishPacket;
        }
        else
        {
            break;
        }
    } while (pOPublishPacket != pOPublishPacketEnd);
    return pOPublishPacket - oPublishPacketsBegin;
}


void BusMeasurePublisher::start()
{
    f_mMonsbPublishBond.init(MonsbShareMemory::memoryManager()->getMinId(), MonsbShareMemory::memoryManager()->measureCount(),
                             MonsbShareMemory::memoryManager()->getMeasureType(), ci_yx_address_int32_vqt_t);
    f_mYcaddPublishBond.init(YcaddShareMemory::memoryManager()->getMinId(), YcaddShareMemory::memoryManager()->measureCount(),
                             YcaddShareMemory::memoryManager()->getMeasureType(), ci_yc_address_double_vqt_t);
    f_mStrawPublishBond.init(StrawShareMemory::memoryManager()->getMinId(), StrawShareMemory::memoryManager()->measureCount(),
                             StrawShareMemory::memoryManager()->getMeasureType(), ci_yw_address_vqt_string128_t);

    f_monsbPublishedIndex = MonsbShareMemory::memoryManager()->changedCount() - 1;
    f_ycaddPublishedIndex = YcaddShareMemory::memoryManager()->changedCount() - 1;
    f_strawPublishedIndex = StrawShareMemory::memoryManager()->changedCount() - 1;

    if (!f_oScanMeasurePublishThread)
    {
        f_oScanMeasurePublishThread = new ScanMeasurePublishThread();
    }
    f_oScanMeasurePublishThread->setStatus(true);
    f_oScanMeasurePublishThread->start();
}

void BusMeasurePublisher::stop()
{
    if (f_oScanMeasurePublishThread)
    {
        f_oScanMeasurePublishThread->waitExit();
        delete f_oScanMeasurePublishThread;
        f_oScanMeasurePublishThread = NULL;
    }
}

void BusMeasurePublisher::timerCheckPublish(int iInterval)
{
    static int iTimes = 0;
    int iCase = iTimes % 4;
    if (iCase == 0)
    {
    }
    else if (iCase == 1)
    {
        f_iMonsbPublishPacketIndexPop = fn_postPublishPacket<int>(f_oMonsbPublishPackets, f_oMonsbPublishPacketEnd, f_iMonsbPublishPacketIndexPop);
    }
    else if (iCase == 2)
    {
        f_iYcaddPublishPacketIndexPop = fn_postPublishPacket<double>(f_oYcaddPublishPackets, f_oYcaddPublishPacketEnd, f_iYcaddPublishPacketIndexPop);
    }
    else if (iCase == 3)
    {
        f_iStrawPublishPacketIndexPop = fn_postPublishPacket<StrawValue>(f_oStrawPublishPackets, f_oStrawPublishPacketEnd, f_iStrawPublishPacketIndexPop);
    }

    iTimes ++;
}

void BusMeasurePublisher::timerSendAll(int iInterval)
{
    static int iTimes = 0;
    int iCase = iTimes % 4;
    if (iCase == 0)
    {
    }
    else if (iCase == 1)
    {
        f_iMonsbChangedPacketIndexPop = fn_postChangedPacket<int>(f_oMonsbChangedPackets, f_oMonsbChangedPacketEnd, f_iMonsbChangedPacketIndexPop);
    }
    else if (iCase == 2)
    {
        f_iYcaddChangedPacketIndexPop = fn_postChangedPacket<double>(f_oYcaddChangedPackets, f_oYcaddChangedPacketEnd, f_iYcaddChangedPacketIndexPop);
    }
    else if (iCase == 3)
    {
        f_iStrawChangedPacketIndexPop = fn_postChangedPacket<StrawValue>(f_oStrawChangedPackets, f_oStrawChangedPacketEnd, f_iStrawChangedPacketIndexPop);
    }

    iTimes ++;
}

void BusMeasurePublisher::update(const std::map<string, string> &row)
{
    if (! f_mMonsbPublishBond.update(row))
        if (! f_mYcaddPublishBond.update(row))
            f_mStrawPublishBond.update(row);
}

std::vector<string> BusMeasurePublisher::reportSelf()
{
    vector<string> sReports = f_mMonsbPublishBond.reportSelf();
    CxContainer::append(sReports, f_mYcaddPublishBond.reportSelf());
    CxContainer::append(sReports, f_mStrawPublishBond.reportSelf());
    return sReports;
}
