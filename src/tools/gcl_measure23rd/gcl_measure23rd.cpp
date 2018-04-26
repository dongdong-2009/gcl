
/*

  json文件的编辑可以使用 json 的在线编辑器，网址：
  http://www.jsoneditoronline.org/

{
  "dbname": "YGCT",
  "tables": [
    {
      "tbname": "T_CFG_YGCT_TOOL_MEASURE2DB",
      "rows": [
        {
          "mid": 16777216,
          "url": "",
          "interval": 30000
        },
        {
          "mid": 33554432,
          "url": "",
          "interval": 30000
        },
        {
          "mids": "33554433 33554434 33554435",
          "interval": 30000
        },
        {
          "mid2mid": "33554443~33554445",
          "interval": 30000
        }
      ]
    }
  ]
}


XML文件节点TAG：YGCT->MSG->RealData41117->RealData41117List
标识符	字段名称	类型	长度	备注	强制/可选
URL	域地址	String	20
ADDRESS	内部的点号	String	10		M
VALUE	实时值	Decimal	18,3		M
PINZHI	品质	Integer	1	0-差，1-好	M
DATATIME	时间	DateTime			M
RESOURCE	源地址	String	64
EXT	扩展字段	String	256		M
SBID	计量设备ID	Integer	10	对应管理对象2100	M

*/


#include "gcl_measure23rd.h"
#include <vdi/measure_sharememory_all.h>
#include <ccxx/cxrapidjson.h>

#ifdef RTDCLIENT_API
#include <ics_rtdclient_api.h>
#endif

using namespace std;


static size_t f_monsbChangedIndex = GM_UINT_MAX;
static size_t f_ycaddChangedIndex = GM_UINT_MAX;
static size_t f_strawChangedIndex = GM_UINT_MAX;

static int f_iPublishAll_monsb_interval = 30 * 1000 + 1000;
static int f_iPublishAll_ycadd_interval = 30 * 1000 + 2000;
static int f_iPublishAll_straw_interval = 30 * 1000 + 3000;
static msepoch_t f_dtPublishAll_monsb_nextTime = 0;
static msepoch_t f_dtPublishAll_ycadd_nextTime = 0;
static msepoch_t f_dtPublishAll_straw_nextTime = 0;

static bool f_bLogRC = false;

//*
//* regist
//*
static Measure23rd * fn_oMeasure23rd()
{
    CxInterinfoIn::addObserver(Measure23rd::interinfoIn_line);
    CxInterinfoIn::addObserver(Measure23rd::interinfoIn_cmd);

    CxApplication::registStartFunction(Measure23rd::start, 20001);
    CxApplication::registStopFunction(Measure23rd::stop);

    GeneralAccess::registerMessageCommandDeal(Measure23rd::dealMessage);
    GeneralAccess::registerFileWriteDeal(Measure23rd::dealFileWrite);
    static Measure23rd m;
    return &m;
}

static Measure23rd * m_pMeasure23rd = fn_oMeasure23rd();


template<typename T>
struct MeasureBlock {
    int address;
    T value;
    int quality;
    long long datetime;
};

template<typename T>
static int fn_publishChangeds_measure(const typename MeasureShareMemory<T>::ChangedData * const oChangedBegin, const typename MeasureShareMemory<T>::ChangedData * const oChangedEnd, int nCode)
{
    vector<MeasureBlock<T> > measureSendDatas;
    const typename MeasureShareMemory<T>::ChangedData * oChangedData = oChangedBegin;
    while (oChangedData < oChangedEnd)
    {
        MeasureBlock<T> measureSendData;
        measureSendData.address = oChangedData->measureId;
        measureSendData.value = oChangedData->value;
        measureSendData.quality = oChangedData->quality;
        measureSendData.datetime = oChangedData->changedTime;
        measureSendDatas.push_back(measureSendData);
        oChangedData++;
    }
    if (measureSendDatas.size()>0)
    {
        int iResutl = 0;
        try
        {
#ifdef RTDCLIENT_API
            iResutl = rtd_upddata(nCode, (char*)(& measureSendDatas.front()), measureSendDatas.size());
#endif
        }
        catch (...)
        {
            cxDebug() << "publish [" << sizeof(T) << "] all measure fail, rtd_upddata Error";
        }
        cxDebug() << "publish [" << sizeof(T) << "] measure change, count: " <<  measureSendDatas.size() << ", rtd_upddata_Result: " << iResutl;
    }
    return measureSendDatas.size();
}

static void fn_publishChangeds_monsb(const MonsbShareMemory::ChangedData * const oChangedBegin, const typename MonsbShareMemory::ChangedData * const oChangedEnd)
{
    fn_publishChangeds_measure<int>(oChangedBegin, oChangedEnd, 1);
}

static void fn_publishChangeds_ycadd(const YcaddShareMemory::ChangedData * const oChangedBegin, const typename YcaddShareMemory::ChangedData * const oChangedEnd)
{
    fn_publishChangeds_measure<double>(oChangedBegin, oChangedEnd, 2);
}

static void fn_publishChangeds_straw(const StrawShareMemory::ChangedData * const oChangedBegin, const typename StrawShareMemory::ChangedData * const oChangedEnd)
{
    fn_publishChangeds_measure<StrawValue>(oChangedBegin, oChangedEnd, 3);
}

template<typename T>
static int fn_publishAll_measure(typename MeasureShareMemory<T>::MemoryManagerBase * oMemoryManager, int nCode)
{
    //MemoryManager
    size_t * oMeasuresIndex;
    size_t * oMeasuresIndexEnd;
    oMeasuresIndex = oMemoryManager->measuresIndexBegin();
    oMeasuresIndexEnd = oMemoryManager->measuresIndexEnd();
    typename MeasureShareMemory<T>::MeasureData * oMeasureDataBegin = oMemoryManager->measuresDataBegin();
    typename MeasureShareMemory<T>::MeasureData measureData;
    //SendData
    int iCount = oMeasuresIndexEnd - oMeasuresIndex;
    if (iCount != oMemoryManager->measureCount())
    {
        cxDebug() << "fn_publishAll_measure error !!! internal error : iCount != oMemoryManager->measureCount()";
        return 0;
    }
    if (iCount <= 0 || iCount > 1024 * 100)
    {
        cxDebug() << "fn_publishAll_measure error !!! iCount error : iCount: " << iCount;
        return 0;
    }
    vector<MeasureBlock<T> > measureSendDatas;measureSendDatas.resize(iCount);
    int iIndex = 0;
    while (oMeasuresIndex < oMeasuresIndexEnd)
    {
        measureData = *(oMeasureDataBegin + (*oMeasuresIndex));
        MeasureBlock<T> & measureSendData = measureSendDatas.at(iIndex);
        //MeasureId
        measureSendData.address = measureData.measureId;
        //值
        measureSendData.value = measureData.value;
        //品质
        measureSendData.quality = measureData.quality;
        //修改时间
        measureSendData.datetime = measureData.changedTime;
        oMeasuresIndex++;
        iIndex++;
    }
    if (measureSendDatas.size()>0)
    {
        int iResutl = 0;
        try
        {
#ifdef RTDCLIENT_API
            iResutl = rtd_upddata(nCode, (char*)(& measureSendDatas.front()), measureSendDatas.size());
#endif
        }
        catch (...)
        {
            cxDebug() << "publish [" << sizeof(T) << "] all measure fail, rtd_upddata Error";
        }
        cxDebug() << "publish [" << sizeof(T) << "] all measure, count: " <<  measureSendDatas.size() << ", rtd_upddata_Result: " << iResutl;
    }
    return measureSendDatas.size();
}

static int fn_publishAll_monsb()
{
    return fn_publishAll_measure<int>(MonsbShareMemory::memoryManager(), 1);
}

static int fn_publishAll_ycadd()
{
    return fn_publishAll_measure<double>(YcaddShareMemory::memoryManager(), 2);
}

static int fn_publishAll_straw()
{
    return fn_publishAll_measure<StrawValue>(StrawShareMemory::memoryManager(), 3);
}









void Measure23rd::start()
{
    CxTimerManager::startTimer(Measure23rd::timerDo, 100);
    f_bLogRC = CxApplication::containArgument("logrc");
}

void Measure23rd::stop()
{
    CxTimerManager::stopTimer(Measure23rd::timerDo);
}

void Measure23rd::measureShareMemoryAfterLoad(int iMeasureType)
{
    switch (iMeasureType)
    {
    case MeasureType_Monsb:
        f_monsbChangedIndex = MonsbShareMemory::memoryManager()->changedCount()-1;
        break;
    case MeasureType_Ycadd:
        f_ycaddChangedIndex = YcaddShareMemory::memoryManager()->changedCount()-1;
        break;
    case MeasureType_Straw:
        f_strawChangedIndex = StrawShareMemory::memoryManager()->changedCount()-1;
        break;
    default:
        break;
    }
}

void Measure23rd::measureShareMemoryBeforeClose(int iMeasureType)
{
}

int Measure23rd::dealMessage(const PsmExplainer *oPsm, const string &sMessageCommand, const string &sMessageParam, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    return FALSE;
}

int Measure23rd::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    if (fileData.empty())
        return FALSE;
    return FALSE;
}

void Measure23rd::timerDo(int iInterval)
{
    static int iTimes = 0;

    msepoch_t dtNow = CxTime::currentMsepoch();
    iTimes++;
    int iCase;
    iCase = iTimes % 5;
    if (iCase == 1)
    {
        if (MonsbShareMemory::memoryManager()->isValid())
        {
            if (dtNow > f_dtPublishAll_monsb_nextTime)
            {
                fn_publishAll_monsb();
                f_dtPublishAll_monsb_nextTime += dtNow + f_iPublishAll_monsb_interval;
            }
        }
        if (YcaddShareMemory::memoryManager()->isValid())
        {
            if (dtNow > f_dtPublishAll_ycadd_nextTime)
            {
                fn_publishAll_ycadd();
                f_dtPublishAll_ycadd_nextTime += dtNow + f_iPublishAll_ycadd_interval;
            }
        }
        if (StrawShareMemory::memoryManager()->isValid())
        {
            if (dtNow > f_dtPublishAll_straw_nextTime)
            {
                fn_publishAll_straw();
                f_dtPublishAll_straw_nextTime += dtNow + f_iPublishAll_straw_interval;
            }
        }
    }
    else if (iCase == 2)
    {
        if (MonsbShareMemory::memoryManager()->isValid())
        {
            f_monsbChangedIndex = MonsbShareMemory::memoryManager()->checkChangeds(f_monsbChangedIndex, fn_publishChangeds_monsb);
        }
    }
    else if (iCase == 3)
    {
        if (YcaddShareMemory::memoryManager()->isValid())
        {
            f_ycaddChangedIndex = YcaddShareMemory::memoryManager()->checkChangeds(f_ycaddChangedIndex, fn_publishChangeds_ycadd);
        }
    }
    else if (iCase == 4)
    {
        if (StrawShareMemory::memoryManager()->isValid())
        {
            f_strawChangedIndex = StrawShareMemory::memoryManager()->checkChangeds(f_strawChangedIndex, fn_publishChangeds_straw);
        }
    }
}

/***
 m2db -update ssid=1;name=xxx;ttid=2

*/
int Measure23rd::interinfoIn_line(const string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

int Measure23rd::interinfoIn_cmd(const string &sCommand, const std::map<string, string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}
