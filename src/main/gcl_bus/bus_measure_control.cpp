#include "bus_measure_control.h"


#include <vdi/measure_sharememory_all.h>
#include "bus_terminal.h"


using namespace std;


//定义实时改变发送区的缓存大小
#define f_iMeasureControlCacheCount (1024 * 5)

//定义一数据包的大小
#define f_iControlPacketDataSize    (1024 * 2 - sizeof(psm_packet_body_t) - 16)


size_t f_monsbControlIndex = 0;
size_t f_ycaddControlIndex = 0;
size_t f_strawControlIndex = 0;


BusMeasureControl * fn_oBusMeasureControl()
{
    CxApplication::registStartFunction(BusMeasureControl::start);
    CxApplication::registStopFunction(BusMeasureControl::stop);

    static BusMeasureControl m;
    return &m;
}

static BusMeasureControl * f_oBusRealtimedataControl = fn_oBusMeasureControl();




template<typename T>
class MeasureControlBond
{
public:
    struct ControlPacket
    {
        int terminalId;
        vector<char> command;
        vector<char> param;
    };

protected:
    int _measureMinId;
    int _measureCount;
    int _measureType;
    int _structTypeId;
    vector<vector<int> > _measureTerminalIds;

public:
    MeasureControlBond() :
        _measureMinId(0), _measureCount(0), _measureType(0), _structTypeId(0)
    { }

    void init(int iMinId, int iCount, int iMeasureType, int iStructTypeId) {
        _measureMinId = iMinId;
        _measureCount = iCount;
        _measureType = iMeasureType;
        _structTypeId = iStructTypeId;
        _measureTerminalIds.resize(iCount);
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
        std::string sControls;

        iTerminalId = CxContainer::valueTo(row, string("terminal"), iTerminalId);
        GM_INVALID_RETURE_(iTerminalId>0, iResult);

        sControls = CxContainer::value(row, string("controls"));
        GM_VALID_RETURE_(sControls.empty(), iResult);

        vector<string> v = CxString::split(sControls,',');

        for (size_t i = 0; i < v.size(); ++i)
        {
            string s = v.at(i);
            //更新带 ‘~' 段式
            if (CxString::contain(s, "~")) {
                string sMeasureBegin = CxString::token(s, '~');
                string sMeasureEnd = s;
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
                int mid =  CxString::toInt32(s);
                if (g_fn_getMeasureType(mid) == _measureType)
                {
                    update(mid, iTerminalId );
                    iResult = TRUE;
                }
            }

        }

        return iResult;
    }

    std::vector<std::string> reportSelf() {
        vector<string> sReports;
        sReports.push_back( CxString::format("*** MeasureControlBond.%s Report ***" , g_fn_getMeasureTypeName(_measureType).c_str() ) );
        sReports.push_back(CxString::format("MeasureMinId=", _measureMinId));
        sReports.push_back(CxString::format("MeasureCount=", _measureCount));
        sReports.push_back(CxString::format("StructTypeId=", _structTypeId));
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

    //按照订阅表来发布
    int publishControl(int iMeasureId, const std::string & sCommand, const std::string & sParam, const PsmAttach *oAttach,
                       ControlPacket * * const oControlPacketsBegin, ControlPacket * * const oControlPacketsEnd, int iControlPacketIndex, int * pTransferCount = NULL)
    {
        GM_INVALID_RETURE_(_measureTerminalIds.size() > 0, iControlPacketIndex);

        const vector<int> * oMeasureTerminals = getMeasureTerminals(iMeasureId);
        if (oMeasureTerminals && oMeasureTerminals->size()>0)
        {
            //发布到缓存中
            int iTransferCount = 0;
            ControlPacket * * pOControlPacket = oControlPacketsBegin + iControlPacketIndex;
            ControlPacket * oControlPacket;
            for (size_t i = 0; i < oMeasureTerminals->size(); ++i)
            {
                if (* pOControlPacket != NULL)
                {
                    string sError = CxString::format("Error ! Error ! Error ! Bus Control %s Publish - ControlPacket Cache Full (* pOControlPacket != NULL) , Can not publish !!!", g_fn_getMeasureTypeName(_measureType).c_str());
                    cxWarning() << sError;
                    cout        << sError << endl;
                    break;
                }
                oControlPacket = new ControlPacket();
                oControlPacket->command.insert(oControlPacket->command.begin(), sCommand.begin(), sCommand.end());
                oControlPacket->param.insert(oControlPacket->param.begin(), sParam.begin(), sParam.end());
                oControlPacket->terminalId = oMeasureTerminals->at(i);
                * pOControlPacket = oControlPacket;
                pOControlPacket ++;
                if (pOControlPacket >= oControlPacketsEnd)
                {
                    pOControlPacket = oControlPacketsBegin;
                }
                iTransferCount++;
            }
            if (pTransferCount) * pTransferCount = iTransferCount;
            return pOControlPacket - oControlPacketsBegin;
        }
        return iControlPacketIndex;
    }

};


//*YX
class MonsbControlBond : public MeasureControlBond<int>
{
public:
};
typedef MonsbControlBond::ControlPacket MonsbControlPacket;

MonsbControlPacket * * fn_oMonsbControlPackets()
{
    static vector<MonsbControlPacket*> m(f_iMeasureControlCacheCount, (MonsbControlPacket*)NULL);
    return (MonsbControlPacket * *)(m.data());
}

static MonsbControlPacket * * const f_oMonsbControlPackets = fn_oMonsbControlPackets();
static MonsbControlPacket * * const f_oMonsbControlPacketEnd = fn_oMonsbControlPackets() + f_iMeasureControlCacheCount;
static int f_iMonsbControlPacketIndexPush = 0;
static int f_iMonsbControlPacketIndexPop = 0;
static MonsbControlBond f_mMonsbControlBond;



//*YC
class YcaddControlBond : public MeasureControlBond<double>
{
public:
};
typedef YcaddControlBond::ControlPacket YcaddControlPacket;

YcaddControlPacket * * fn_oYcaddControlPackets()
{
    static vector<YcaddControlPacket*> m(f_iMeasureControlCacheCount, (YcaddControlPacket*)NULL);
    return (YcaddControlPacket * *)(m.data());
}

static YcaddControlPacket * * const f_oYcaddControlPackets = fn_oYcaddControlPackets();
static YcaddControlPacket * * const f_oYcaddControlPacketEnd = fn_oYcaddControlPackets() + f_iMeasureControlCacheCount;
static int f_iYcaddControlPacketIndexPush = 0;
static int f_iYcaddControlPacketIndexPop = 0;
static YcaddControlBond f_mYcaddControlBond;



//*YW
class StrawControlBond : public MeasureControlBond<StrawValue>
{
public:
};
typedef StrawControlBond::ControlPacket StrawControlPacket;

StrawControlPacket * * fn_oStrawControlPackets()
{
    static vector<StrawControlPacket*> m(f_iMeasureControlCacheCount, (StrawControlPacket*)NULL);
    return (StrawControlPacket * *)(m.data());
}

static StrawControlPacket * * const f_oStrawControlPackets = fn_oStrawControlPackets();
static StrawControlPacket * * const f_oStrawControlPacketEnd = fn_oStrawControlPackets() + f_iMeasureControlCacheCount;
static int f_iStrawControlPacketIndexPush = 0;
static int f_iStrawControlPacketIndexPop = 0;
static StrawControlBond f_mStrawControlBond;


template<typename T>
int fn_postControlPacket(typename MeasureControlBond<T>::ControlPacket * * const oControlPacketsBegin, typename MeasureControlBond<T>::ControlPacket * * const oControlPacketsEnd,
                         int iControlPacketIndex)
{
    typename MeasureControlBond<T>::ControlPacket * * pOControlPacketEnd = oControlPacketsBegin + iControlPacketIndex;
    typename MeasureControlBond<T>::ControlPacket * * pOControlPacket = oControlPacketsBegin + iControlPacketIndex;
    typename MeasureControlBond<T>::ControlPacket * oControlPacket;
    do
    {
        //判断是否收到 push 的 ProcessCallBack
        if (* pOControlPacket != NULL)
        {
            //复制指针
            oControlPacket = * pOControlPacket;
            * pOControlPacket = NULL;
            pOControlPacket ++;
            if (pOControlPacket == oControlPacketsEnd)
                pOControlPacket = oControlPacketsBegin;

            //发送
            BusManager::transferMessage(0, oControlPacket->command.data(), oControlPacket->param.data(), oControlPacket->param.size(), oControlPacket->terminalId);
            delete oControlPacket;
        }
        else
        {
            break;
        }
    } while (pOControlPacket != pOControlPacketEnd);
    return pOControlPacket - oControlPacketsBegin;
}


void BusMeasureControl::start()
{
    f_mMonsbControlBond.init(MonsbShareMemory::memoryManager()->getMinId(), MonsbShareMemory::memoryManager()->measureCount(),
                             MonsbShareMemory::memoryManager()->getMeasureType(), ci_yx_address_int32_vqt_t);
    f_mYcaddControlBond.init(YcaddShareMemory::memoryManager()->getMinId(), YcaddShareMemory::memoryManager()->measureCount(),
                             YcaddShareMemory::memoryManager()->getMeasureType(), ci_yc_address_double_vqt_t);
    f_mStrawControlBond.init(StrawShareMemory::memoryManager()->getMinId(), StrawShareMemory::memoryManager()->measureCount(),
                             StrawShareMemory::memoryManager()->getMeasureType(), ci_yw_address_vqt_string128_t);
}

void BusMeasureControl::stop()
{
}

void BusMeasureControl::timerCheckPublish(int iInterval)
{
    static int iTimes = 0;
    int iCase = iTimes % 4;
    if (iCase == 0)
    {
    }
    else if (iCase == 1)
    {
        f_iMonsbControlPacketIndexPop = fn_postControlPacket<int>(f_oMonsbControlPackets, f_oMonsbControlPacketEnd, f_iMonsbControlPacketIndexPop);
    }
    else if (iCase == 2)
    {
        f_iYcaddControlPacketIndexPop = fn_postControlPacket<double>(f_oYcaddControlPackets, f_oYcaddControlPacketEnd, f_iYcaddControlPacketIndexPop);
    }
    else if (iCase == 3)
    {
        f_iStrawControlPacketIndexPop = fn_postControlPacket<StrawValue>(f_oStrawControlPackets, f_oStrawControlPacketEnd, f_iStrawControlPacketIndexPop);
    }

    iTimes ++;
}

void BusMeasureControl::dealPsmMeasureControl(const std::string &sCommand, const char *pParam, int iParamLength, const PsmAttach *oAttach)
{
    //    struct ControlData
    //    {
    //        int controlId; // 变化ID : 1,2,3...0x7fffffff
    //        int measureId; // 量ID
    //        T value; // 量值
    //        msepoch_t controlTime; // 值的修改时间
    //        int controlSourceId; // 控制的源ID
    //        int controlReasonId; // 控制的原因
    //        int controlStrategyId; // 控制的策略
    //        int controlMethodId; // 控制的方法
    //        char params[512];
    //        int res; // 保留
    //    };

    //send.yk measure=0x01000011;value=1;reason=2;strategy=3;method=4\nk11=v11;k12=v12\nk21=v21;k22=v22\n

    string sParam(pParam, iParamLength);

    //    std::string sMainParam = CxString::token(sParam, '\n');
    std::string sMainParam;
    std::string sSubParam;
    size_t iIndex = sParam.find('\n');
    if (iIndex != string::npos)
    {
        sMainParam = sParam.substr(0, iIndex);
        sSubParam = sParam.substr(iIndex+1);
    }
    else
    {
        sMainParam = sParam;
        sSubParam = "";
    }

    std::map<string, string> sMainParams = CxString::splitToMap( sMainParam , '=', ';' );

    int controlMeasureId = CxString::toInt32(sMainParams[CS_EntryControlMeasure]); // 量ID
    int iMeasureType = g_fn_getMeasureType(controlMeasureId);

    if (iMeasureType == MeasureType_None) {
        cxDebug() << "dealPsmMeasureControl error : measure is invalid !!!"<<controlMeasureId<<cxEndLine;
        return;
    }

    msepoch_t controlTime = CxTime::currentSystemTime(); // 值的控制时间
    int controlSourceId = oAttach ? oAttach->sourceId : 0; // 控制的源ID
    int controlReasonId = 0; // 控制的原因
    int controlStrategyId = 0; // 控制的策略
    int controlMethodId = 0; // 控制的方法
    int transferCount = 0;

    controlReasonId = CxContainer::valueTo(sMainParams, CS_EntryControlReasonId, controlReasonId);
    controlStrategyId = CxContainer::valueTo(sMainParams, CS_EntryControlStrategyId, controlStrategyId);
    controlMethodId = CxContainer::valueTo(sMainParams, CS_EntryControlMethodId, controlMethodId);

    if (iMeasureType == MeasureType_Monsb)
    {
        //按照订阅表来发布，放队列中
        f_iMonsbControlPacketIndexPush = f_mMonsbControlBond.publishControl(controlMeasureId, sCommand, sParam, oAttach, f_oMonsbControlPackets, f_oMonsbControlPacketEnd, f_iMonsbControlPacketIndexPush, &transferCount);

        //放共享内存中
        int controlValue = CxContainer::valueTo(sMainParams, CS_EntryControlValue, int(-1));
        if (controlValue == -1) {
            cxDebug() << "dealPsmMeasureControl error : valud is invalid, shareMemory do not setControl. but push transfer list!!!";
            return;
        }
        MonsbShareMemory::memoryManager()->pushControl(controlMeasureId, controlValue, controlTime, controlSourceId, controlReasonId, controlStrategyId, controlMethodId, sSubParam, transferCount);
    }
    else if (iMeasureType == MeasureType_Ycadd)
    {
        //按照订阅表来发布，放队列中
        f_iYcaddControlPacketIndexPush = f_mYcaddControlBond.publishControl(controlMeasureId, sCommand, sParam, oAttach, f_oYcaddControlPackets, f_oYcaddControlPacketEnd, f_iYcaddControlPacketIndexPush, &transferCount);

        //放共享内存中
        double controlValue = CxContainer::valueTo(sMainParams, CS_EntryControlValue, double(-1));
        if (controlValue == -1) {
            cxDebug() << "dealPsmMeasureControl error : valud is invalid, shareMemory do not setControl. but push transfer list!!!";
            return;
        }
        YcaddShareMemory::memoryManager()->pushControl(controlMeasureId, controlValue, controlTime, controlSourceId, controlReasonId, controlStrategyId, controlMethodId, sSubParam, transferCount);
    }
    else if (iMeasureType == MeasureType_Straw)
    {
        //按照订阅表来发布，放队列中
        f_iStrawControlPacketIndexPush = f_mStrawControlBond.publishControl(controlMeasureId, sCommand, sParam, oAttach, f_oStrawControlPackets, f_oStrawControlPacketEnd, f_iStrawControlPacketIndexPush, &transferCount);

        //放共享内存中
        string controlValue = CxContainer::valueTo(sMainParams, CS_EntryControlValue, std::string());
        if (controlValue.empty()) {
            cxDebug() << "dealPsmMeasureControl error : valud is invalid, shareMemory do not setControl. but push transfer list!!!";
            return;
        }
        StrawShareMemory::memoryManager()->pushControl(controlMeasureId, controlValue, controlTime, controlSourceId, controlReasonId, controlStrategyId, controlMethodId, sSubParam, transferCount);
    }
}

void BusMeasureControl::update(const std::map<string, string> &row)
{
    if (! f_mMonsbControlBond.update(row))
        if (! f_mYcaddControlBond.update(row))
            f_mStrawControlBond.update(row);
}

std::vector<string> BusMeasureControl::reportSelf()
{
    vector<string> sReports = f_mMonsbControlBond.reportSelf();
    CxContainer::append(sReports, f_mYcaddControlBond.reportSelf());
    CxContainer::append(sReports, f_mStrawControlBond.reportSelf());
    return sReports;
}

