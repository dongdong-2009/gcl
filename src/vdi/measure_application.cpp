#include "measure_application.h"

#include <common/gcl_resource.h>
#include <common/gcl_macro.h>

#include <psm/general_access.h>
#include <ccxx/cxapplication.h>

//#define GM_MEASURE_MANAGER
#ifdef GM_MEASURE_MANAGER
#include <vdi/monsb_measure.h>
#include <vdi/ycadd_measure.h>
#include <vdi/straw_measure.h>
#endif

//#define GM_GCL_SHAREMEMORY
#ifdef GM_GCL_SHAREMEMORY
#include <shm/measure_sharememory_all.h>
#include <shm/fileinfo_sharememory.h>
#endif

using namespace std;

//*var
static int f_iMonsbIdBegin = 0;
static int f_iMonsbCount = 0;
static int f_iMonsbBeginFix = 0;
static int f_iYcaddIdBegin = 0;
static int f_iYcaddCount = 0;
static int f_iYcaddBeginFix = 0;
static int f_iStrawIdBegin = 0;
static int f_iStrawCount = 0;
static int f_iStrawBeginFix = 0;


static size_t f_monsbPublishedIndex = GM_UINT_MAX;
static size_t f_ycaddPublishedIndex = GM_UINT_MAX;
static size_t f_strawPublishedIndex = GM_UINT_MAX;
static size_t f_fileInfoPublishedIndex = GM_UINT_MAX;

static int f_iTimes = 0;

//静态初始化
MeasureApplication * fn_oMeasureApplication()
{
    CxApplication::registStartFunction(MeasureApplication::start);
    CxApplication::registStopFunction(MeasureApplication::stop);

    static MeasureApplication m;
    return &m;
}
static MeasureApplication * f_oMeasureApplication = fn_oMeasureApplication();


#ifdef GM_GCL_SHAREMEMORY
#ifdef GM_MEASURE_MANAGER
void fn_publishChangeds_monsb(const MonsbShareMemory::ChangedData * const oChangedBegin, const MonsbShareMemory::ChangedData * const oChangedEnd)
{
    const MonsbShareMemory::ChangedData * oChangedData = oChangedBegin;

    //遍历 oChangedData

    map<int, vector<const MonsbShareMemory::ChangedData *> > changedMeasures;

    while (oChangedData < oChangedEnd)
    {
        map<int, vector<const MonsbShareMemory::ChangedData *> >::iterator it = changedMeasures.find(oChangedData->changedSourceId);
        if (it != changedMeasures.end())
        {
            vector<const MonsbShareMemory::ChangedData *> & measures = it->second;
            measures.push_back(oChangedData);
        }
        else
        {
            vector<const MonsbShareMemory::ChangedData *> measures;
            measures.push_back(oChangedData);
            changedMeasures[oChangedData->changedSourceId] = measures;
        }
        oChangedData++;
    }

    if(changedMeasures.size()<1) return;

    for (map<int, vector<const MonsbShareMemory::ChangedData *> >::const_iterator it = changedMeasures.begin(); it != changedMeasures.end(); ++it)
    {
        int iSourceId = it->first;
        const vector<const MonsbShareMemory::ChangedData *> & measures = it->second;
        MonsbMeasure::beginSetValues(0, iSourceId, ci_tag_ShareMemory);
        MonsbMeasure * o = NULL;
        for (size_t i = 0; i < measures.size(); ++i)
        {
            oChangedData = measures.at(i);
            o = MonsbManager::measure(oChangedData->measureId);
            if(o!=NULL)
            {
                o->setValue(oChangedData->value);
                o->setQuality(oChangedData->quality);
            }
        }
        MonsbMeasure::endSetValues();
    }
}

void fn_publishChangeds_ycadd(const YcaddShareMemory::ChangedData * const oChangedBegin, const YcaddShareMemory::ChangedData * const oChangedEnd)
{
    const YcaddShareMemory::ChangedData * oChangedData = oChangedBegin;
    map<int, vector<const YcaddShareMemory::ChangedData *> > changedMeasures;
    //遍历 oChangedData
    while (oChangedData < oChangedEnd)
    {
        map<int, vector<const YcaddShareMemory::ChangedData *> >::iterator it = changedMeasures.find(oChangedData->changedSourceId);
        if (it != changedMeasures.end())
        {
            vector<const YcaddShareMemory::ChangedData *> & measures = it->second;
            measures.push_back(oChangedData);
        }
        else
        {
            vector<const YcaddShareMemory::ChangedData *> measures;
            measures.push_back(oChangedData);
            changedMeasures[oChangedData->changedSourceId] = measures;
        }

        oChangedData++;
    }

    if(changedMeasures.size()<1) return;

    for (map<int, vector<const YcaddShareMemory::ChangedData *> >::const_iterator it = changedMeasures.begin(); it != changedMeasures.end(); ++it)
    {
        int iSourceId = it->first;
        const vector<const YcaddShareMemory::ChangedData *> & measures = it->second;
        YcaddMeasure::beginSetValues(0, iSourceId, ci_tag_ShareMemory);
        YcaddMeasure * o = NULL;
        for (size_t i = 0; i < measures.size(); ++i)
        {
            oChangedData = measures.at(i);
            o = YcaddManager::measure(oChangedData->measureId);
            if(o!=NULL)
            {
                o->setValue(oChangedData->value);
                o->setQuality(oChangedData->quality);
            }
        }
        YcaddMeasure::endSetValues();
    }

}

void fn_publishChangeds_straw(const StrawShareMemory::ChangedData * const oChangedBegin, const StrawShareMemory::ChangedData * const oChangedEnd)
{
    const StrawShareMemory::ChangedData * oChangedData = oChangedBegin;
    map<int, vector<const StrawShareMemory::ChangedData *> > changedMeasures;
    //遍历 oChangedData
    while (oChangedData < oChangedEnd)
    {
        map<int, vector<const StrawShareMemory::ChangedData *> >::iterator it = changedMeasures.find(oChangedData->changedSourceId);
        if (it != changedMeasures.end())
        {
            vector<const StrawShareMemory::ChangedData *> & measures = it->second;
            measures.push_back(oChangedData);
        }
        else
        {
            vector<const StrawShareMemory::ChangedData *> measures;
            measures.push_back(oChangedData);
            changedMeasures[oChangedData->changedSourceId] = measures;
        }
        oChangedData++;
    }
    if(changedMeasures.size()<1) return;

    for (map<int, vector<const StrawShareMemory::ChangedData *> >::const_iterator it = changedMeasures.begin(); it != changedMeasures.end(); ++it)
    {
        int iSourceId = it->first;
        const vector<const StrawShareMemory::ChangedData *> & measures = it->second;
        StrawMeasure::beginSetValues(0, iSourceId, ci_tag_ShareMemory);
        StrawMeasure * o = NULL;
        for (size_t i = 0; i < measures.size(); ++i)
        {
            oChangedData = measures.at(i);
            o = StrawManager::measure(oChangedData->measureId);
            if(o!=NULL)
            {
                string s(oChangedData->value.value);
                o->setValue(s);
                o->setQuality(oChangedData->quality);
            }
        }
        StrawMeasure::endSetValues();
    }
}

#endif

void fn_publishChangeds_fileInfo(const FileInfoChangedData * const oChangedBegin, const FileInfoChangedData * const oChangedEnd)
{
    const FileInfoChangedData * oChangedData = oChangedBegin;
    const PsmAttach * oDefaultAttach = GeneralAccess::defaultAttach();
    int iGaSourceId = oDefaultAttach ? oDefaultAttach->sourceId : 0;

    //遍历 oChangedData
    while(oChangedData < oChangedEnd)
    {
        if(iGaSourceId == 0 || oChangedData->targetId == iGaSourceId)
        {
//          void GeneralAccess::received_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
            CxFileSystem::PathInfo  pathInfo;

            pathInfo.pathType= (CxFileSystem::PathTypeEnum)oChangedData->fileType;
            string sFilePath = CxString::newString(oChangedData->filepath, sizeof(oChangedData->filepath));

            pathInfo.fileName = CxFileSystem::extractFileName(sFilePath);
            pathInfo.path = CxFileSystem::extractPath(sFilePath);

            pathInfo.lastWriteTime = oChangedData->lastWriteTime;
            pathInfo.creationTime  = oChangedData->creationTime;
            pathInfo.fileSize      = oChangedData->fileSize;

            PsmAttach oAttach;

            oAttach.sourceId = oChangedData->sourceId;
            oAttach.tag      = oChangedData->saveStatus;
            oAttach.targetId = oChangedData->targetId;

            cxPrompt() << "fn_publishChangeds_fileInfo recv file : " << sFilePath;
            GeneralAccess::raiseReceivedFile(NULL, pathInfo, std::vector<string>(), &oAttach);
        }
        oChangedData++;
    }
}


void fn_measureShareMemoryAfterLoad(int iMeasureType)
{
    switch (iMeasureType)
    {
    case MeasureType_Monsb:
        f_monsbPublishedIndex = MonsbShareMemory::memoryManager()->analysePublishedChangedIndex();
        break;
    case MeasureType_Ycadd:
        f_ycaddPublishedIndex = YcaddShareMemory::memoryManager()->analysePublishedChangedIndex();
        break;
    case MeasureType_Straw:
        f_strawPublishedIndex = StrawShareMemory::memoryManager()->analysePublishedChangedIndex();
        break;
    default:
        break;
    }
}

void fn_measureShareMemoryBeforeClose(int iMeasureType)
{

}

void fn_fileInfoShareMemoryAfterLoad(int iType)
{
    f_fileInfoPublishedIndex = FileInfoShareMemory::memoryManager()->analysePublishedChangedIndex();
}

void fn_fileInfoShareMemoryBeforeClose(int iType)
{

}

#endif

















void MeasureApplication::start()
{
    f_iMonsbIdBegin = CxApplication::findConfig( CS_SectionMeasure , CS_EntryMonsbBegin , ci_int_zero );
    f_iMonsbCount = CxApplication::findConfig( CS_SectionMeasure , CS_EntryMonsbCount , ci_int_zero );
    f_iYcaddIdBegin = CxApplication::findConfig( CS_SectionMeasure , CS_EntryYcaddBegin , ci_int_zero );
    f_iYcaddCount = CxApplication::findConfig( CS_SectionMeasure , CS_EntryYcaddCount , ci_int_zero );
    f_iStrawIdBegin = CxApplication::findConfig( CS_SectionMeasure , CS_EntryStrawBegin , ci_int_zero );
    f_iStrawCount = CxApplication::findConfig( CS_SectionMeasure , CS_EntryStrawCount , ci_int_zero );
//获取固定点
    f_iMonsbBeginFix = CxApplication::findConfig( CS_SectionMeasure , CS_EntryMonsbBeginFix , ci_int_zero );
    f_iYcaddBeginFix = CxApplication::findConfig( CS_SectionMeasure , CS_EntryYcaddBeginFix , ci_int_zero );
    f_iStrawBeginFix = CxApplication::findConfig( CS_SectionMeasure , CS_EntryStrawBeginFix , ci_int_zero );


#ifdef GM_GCL_SHAREMEMORY
    //共享内存扫描
    MeasureShareMemoryAll::setAfterLoadCallback(fn_measureShareMemoryAfterLoad);
    MeasureShareMemoryAll::setBeforeCloseCallback(fn_measureShareMemoryBeforeClose);
    MeasureShareMemoryAll::open();

    FileInfoShareMemory::setAfterLoadCallback(fn_fileInfoShareMemoryAfterLoad);
    FileInfoShareMemory::setBeforeCloseCallback(fn_fileInfoShareMemoryBeforeClose);
    FileInfoShareMemory::open();
#endif

    GeneralAccess::start();

    //定时扫描
    CxTimerManager::startTimer(MeasureApplication::timerTimeout, 200);
}

void MeasureApplication::stop()
{
    CxTimerManager::stopTimer(MeasureApplication::timerTimeout);
}

void MeasureApplication::timerTimeout(int iInterval)
{
    scanShareMemory();

    if (f_iTimes == 0 || f_iTimes % 100 == 0)
    {
        setFixStraws();
    }
    //1S执行一次
    if (f_iTimes % 5 == 0)
    {
        setFixMonsbs();
        setFixYcadds();
    }

    ++f_iTimes;
}

int MeasureApplication::monsbIdBegin()
{
    return f_iMonsbIdBegin;
}

int MeasureApplication::monsbCount()
{
    return f_iMonsbCount;
}

int MeasureApplication::ycaddIdBegin()
{
    return f_iYcaddIdBegin;
}

int MeasureApplication::ycaddCount()
{
    return f_iYcaddCount;
}

int MeasureApplication::strawIdBegin()
{
    return f_iStrawIdBegin;
}

int MeasureApplication::strawCount()
{
    return f_iStrawCount;
}

void MeasureApplication::scanShareMemory()
{
#ifdef GM_GCL_SHAREMEMORY
#ifdef GM_MEASURE_MANAGER
    if (MonsbShareMemory::memoryManager()->isValid())
    {
        f_monsbPublishedIndex = MonsbShareMemory::memoryManager()->checkChangeds(f_monsbPublishedIndex, fn_publishChangeds_monsb);
    }
    if (YcaddShareMemory::memoryManager()->isValid())
    {
        f_ycaddPublishedIndex = YcaddShareMemory::memoryManager()->checkChangeds(f_ycaddPublishedIndex, fn_publishChangeds_ycadd);
    }
    if (StrawShareMemory::memoryManager()->isValid())
    {
        f_strawPublishedIndex = StrawShareMemory::memoryManager()->checkChangeds(f_strawPublishedIndex, fn_publishChangeds_straw);
    }
#endif
    if (FileInfoShareMemory::memoryManager()->isValid())
    {
        f_fileInfoPublishedIndex = FileInfoShareMemory::memoryManager()->checkChangeds(f_fileInfoPublishedIndex, fn_publishChangeds_fileInfo);
    }
#endif
}

void MeasureApplication::setFixMonsbs()
{
    if (f_iMonsbIdBegin<0x01000000 || f_iMonsbCount<=0) return;


#ifdef GM_MEASURE_MANAGER
    int iTid = GeneralAccess::defaultAttach()->sourceId;

    MonsbMeasure::beginSetValues(0, iTid);
    const std::vector<MonsbMeasure*> * oMonsbs = MonsbManager::measures();
    MonsbMeasure * oMonsb;
    if ((f_iMonsbBeginFix+GCL_Measure_Index_HeartJump) < oMonsbs->size())
    {
        oMonsb = oMonsbs->at(f_iMonsbBeginFix+GCL_Measure_Index_HeartJump);
        oMonsb->setValue((CxTime::currentSepoch()%2)+1,iTid); //1,2
        oMonsb->setQuality(1);
    }
    MonsbMeasure::endSetValues();
#else
    int iResult = 0;

    vector<gcd_address_int32_vqt_t> yxs;

    msepoch_t dtNow = CxTime::currentMsepoch();

    gcd_address_int32_vqt_t v;
    v.address = f_iMonsbIdBegin +f_iMonsbBeginFix+ GCL_Measure_Index_HeartJump;
    v.value = CxTime::currentSepoch()%2+1;
    v.quality = 1;
    v.datetime = dtNow;
    yxs.push_back(v);

    iResult += GeneralAccess::postRealtimeDataPost(ci_yx_address_int32_vqt_t, (char*)(&yxs.front()), yxs.size()*sizeof(gcd_address_int32_vqt_t), yxs.size(), GeneralAccess::defaultAttach());
//    cxDebug() << CxString::format("send yx by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, yxs.size(), yxs.size()*sizeof(gcd_address_int32_vqt_t));
#endif
}

void MeasureApplication::setFixStraws()
{
    if (f_iStrawIdBegin < 0x03000000 || f_iStrawCount <= 0)
        return;
    int iTid = GeneralAccess::defaultAttach()->sourceId;

#ifdef GM_MEASURE_MANAGER
    const std::vector<StrawMeasure*> * oStraws = StrawManager::measures();
    StrawMeasure * oStrawInfo = NULL;
    if ((f_iStrawBeginFix+GCL_Measure_Index_Info) < oStraws->size())
    {
        oStrawInfo = oStraws->at(f_iStrawBeginFix+GCL_Measure_Index_Info);
    }

    if (oStrawInfo && oStrawInfo->value().empty())
    {
        string      sName = CxApplication::applicationTargetName();

        string sInfo = CxString::format("name=%s;tid=%d;",sName.c_str(),iTid);

        StrawMeasure::beginSetValues(0, iTid);
        oStrawInfo->setValue(sInfo,iTid);
        oStrawInfo->setQuality(1);
        StrawMeasure::endSetValues();
    }
#else
    int iResult = 0;

    vector<gcd_address_vqt_string128_t> yws;

    msepoch_t dtNow = CxTime::currentMsepoch();

    string sName = CxApplication::applicationTargetName();
    string sInfo = CxString::format("name=%s;tid=%d;",sName.c_str(),iTid);

    gcd_address_vqt_string128_t yw;
    yw.address = f_iStrawIdBegin + GCL_Measure_Index_Info;
    if (sInfo.size()<128)
    {
        memcpy(yw.value, sInfo.data(), sInfo.size());
        yw.value[sInfo.size()] = 0;
    }
    yw.quality = 1;
    yw.datetime = dtNow;
    yws.push_back(yw);

    iResult += GeneralAccess::postRealtimeDataPost(ci_yw_address_vqt_string128_t, (char*)(&yws.front()), yws.size()*sizeof(gcd_address_vqt_string128_t), yws.size(), GeneralAccess::defaultAttach());
//    cxDebug() << CxString::format("send yc by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, ycs.size(), ycs.size()*sizeof(gcd_address_double_vqt_t));
#endif
}

void MeasureApplication::setFixYcadds()
{
    if (f_iYcaddIdBegin<0x02000000 || f_iYcaddCount<=0)
        return;

    int64       iReceivedByteCount = 0;
    int64       iSentByteCount = 0;
    sepoch_t   iLastReceivedTime = 0;
    sepoch_t   iLastSentTime = 0;
    const CxChannelBase * oChannel = GeneralAccess::getChannel();
    if (oChannel)
    {
        iReceivedByteCount  = oChannel->receivedByteCount();
        iSentByteCount      = oChannel->sentByteCount    ();
        iLastReceivedTime   = oChannel->lastReceivedTime () / 1000;
        iLastSentTime       = oChannel->lastSentTime     () / 1000;
    }

#ifdef GM_MEASURE_MANAGER
    int iTid = GeneralAccess::defaultAttach()->sourceId;

    YcaddMeasure::beginSetValues(0, iTid);
    const std::vector<YcaddMeasure*> * oMeasures = YcaddManager::measures();

    YcaddMeasure * oMeasure;

    if ((f_iYcaddBeginFix+GCL_Measure_Index_NowTime) < oMeasures->size())
    {
        oMeasure = oMeasures->at(f_iYcaddBeginFix+GCL_Measure_Index_NowTime);
        oMeasure->setValue(CxTime::currentSepoch(),iTid);
        oMeasure->setQuality(1);
    }

    if ((f_iYcaddBeginFix+GCL_Measure_Index_LastReceivedTime) < oMeasures->size())
    {
        oMeasure = oMeasures->at(f_iYcaddBeginFix+GCL_Measure_Index_LastReceivedTime);
        oMeasure->setValue(iLastReceivedTime,iTid);
        oMeasure->setQuality(1);
    }
    if ((f_iYcaddBeginFix+GCL_Measure_Index_LastSentTime) < oMeasures->size())
    {
        oMeasure = oMeasures->at(f_iYcaddBeginFix+GCL_Measure_Index_LastSentTime);
        oMeasure->setValue(iLastSentTime,iTid);
        oMeasure->setQuality(1);
    }

    if ((f_iYcaddBeginFix+GCL_Measure_Index_ReceivedByteCount) < oMeasures->size())
    {
        oMeasure = oMeasures->at(f_iYcaddBeginFix+GCL_Measure_Index_ReceivedByteCount);
        oMeasure->setValue(iReceivedByteCount,iTid);
        oMeasure->setQuality(1);
    }

    if ((f_iYcaddBeginFix+GCL_Measure_Index_SentByteCount) < oMeasures->size())
    {
        oMeasure = oMeasures->at(f_iYcaddBeginFix+GCL_Measure_Index_SentByteCount);
        oMeasure->setValue(iSentByteCount,iTid);
        oMeasure->setQuality(1);
    }



    YcaddMeasure::endSetValues();
#else
    int iResult = 0;

    vector<gcd_address_double_vqt_t> vv;

    msepoch_t dtNow = CxTime::currentMsepoch();

    gcd_address_double_vqt_t v;

    v.address = f_iYcaddIdBegin +f_iYcaddBeginFix+ GCL_Measure_Index_NowTime;
    v.value = CxTime::currentSepoch();
    v.quality = 1;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = f_iYcaddIdBegin +f_iYcaddBeginFix+ GCL_Measure_Index_HeartJump;
    v.value = CxTime::currentSepoch()%2+1;
    v.quality = 1;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = f_iYcaddIdBegin +f_iYcaddBeginFix+ GCL_Measure_Index_LastReceivedTime;
    v.value = iLastReceivedTime;
    v.quality = 1;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = f_iYcaddIdBegin +f_iYcaddBeginFix+ GCL_Measure_Index_LastSentTime;
    v.value = iLastSentTime;
    v.quality = 1;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = f_iYcaddIdBegin +f_iYcaddBeginFix+ GCL_Measure_Index_ReceivedByteCount;
    v.value = iReceivedByteCount;
    v.quality = 1;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = f_iYcaddIdBegin +f_iYcaddBeginFix+ GCL_Measure_Index_SentByteCount;
    v.value = iSentByteCount;
    v.quality = 1;
    v.datetime = dtNow;
    vv.push_back(v);

    iResult += GeneralAccess::postRealtimeDataPost(ci_yc_address_double_vqt_t, (char*)(&vv.front()), vv.size()*sizeof(gcd_address_double_vqt_t), vv.size(), GeneralAccess::defaultAttach());
//    cxDebug() << CxString::format("send yx by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, yxs.size(), yxs.size()*sizeof(gcd_address_int32_vqt_t));
#endif
}


