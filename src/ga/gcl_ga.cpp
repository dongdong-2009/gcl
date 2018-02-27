#include "gcl_ga.h"

#include <psm/general_access.h>

#include <vdi/measure_sharememory_all.h>

using namespace std;

static volatile int f_sdkClientStatus = 0;


static GclGa f_sdkClient;
static gci_fn_error_t _fn_error = (gci_fn_error_t)0;
static gci_fn_message_command_t _fn_message_command = (gci_fn_message_command_t)0;
static gci_fn_file_transfer_t _fn_file_transfer = (gci_fn_file_transfer_t)0;
static gci_fn_realtime_data_request_t _fn_realtime_data_request = (gci_fn_realtime_data_request_t)0;
static gci_fn_realtime_data_post_t _fn_realtime_data_post = (gci_fn_realtime_data_post_t)0;
static gci_fn_file_transfer_t _fn_file_transfer_result = (gci_fn_file_transfer_t)0;


CxMutex * fn_oGaPostLock()
{
    static CxMutex m;
    return & m;
}
static CxMutex * f_oGaPostLock = fn_oGaPostLock();


class ThreadMainPrivate : public CxJoinableThread
{
public:
    inline void waitExit() { join(); }

protected:
    void run()
    {
        CxApplication::exec();
    }


};

ThreadMainPrivate f_threadMain;

int GclGa::init(int argc, const char *argv[])
{
    if (f_sdkClientStatus > 0)
        return 0;

    f_sdkClientStatus = 1;

    if (argc > 0)
    {
        CxApplication::setApplicationDefaultDeployPath(CxFileSystem::mergeFilePath( CxFileSystem::extractPath(argv[0]) , "gcl_sdk" ));
    }

    CxApplication::init(argc, argv, GM_PROJECT_TYPE_LIB_DLL);

    //* deal input GeneralAccess
    if (CxAppEnv::findArgument(CS_SectionGeneralAccess).size() > 0)
    {
        if (CxAppEnv::findConfig(CS_SectionGeneralAccess, string("RemotePort"), string()).size()>0)
        {
            if (CxAppEnv::findConfig(CS_SectionGeneralAccess, string("Channel"), string()).empty())
            {
                CxAppEnv::saveConfig(CS_SectionGeneralAccess, string("Channel"), string("Udp"));
            }
            if (CxAppEnv::findConfig(CS_SectionGeneralAccess, string("PsmHeartJumpInterval"), string()).empty())
            {
                CxAppEnv::saveConfig(CS_SectionGeneralAccess, string("PsmHeartJumpInterval"), string("3000"));
            }
        }
        if (CxAppEnv::findConfig(CS_SectionProgramConfig, string("LogDisables"), string()).empty())
        {
            CxAppEnv::saveConfig(CS_SectionProgramConfig, string("LogDisables"), string("all"));
        }
    }

    CxFileSystem::createDirMultiLevel(CxFileSystem::mergeFilePath( CxApplication::applicationDeployPath(), "temp" ));

    GeneralAccess::registerMessageCommandDeal(GclGa::received_message_command);
    GeneralAccess::registerFileWriteDeal(GclGa::received_file_write);
    GeneralAccess::registerRealtimeDataRequestDeal(GclGa::received_realtime_data_request);
    GeneralAccess::registerRealtimeDataPostDeal(GclGa::received_realtime_data_post);

    GeneralAccess::registerResultFileWriteDeal(GclGa::result_file_write);

//    string sLogPath = CxFileSystem::mergeFilePath( CxFileSystem::getCurrentDir() , "gcl_sdk.log");
//    string sLog = CxAppEnv::logPath();
//    CxFile::save(sLogPath, sLog);

    f_threadMain.start();

    return TRUE;
}

void GclGa::cleanup()
{
    CxApplication::exit();
    if (f_sdkClientStatus == 0)
        return;
    f_sdkClientStatus = 0;
    f_threadMain.waitExit();
}

int GclGa::status()
{
    return f_sdkClientStatus;
}

bool GclGa::connected()
{
    return GeneralAccess::isConnected();
}

int GclGa::onlineMillisecond()
{
    return CxTime::milliSecondDifferToNow(GeneralAccess::lastReceivedProtocolDataTime());
}

int GclGa::lastReceivedProtocolDataTime()
{
    return GeneralAccess::lastReceivedProtocolDataTime() / 1000;
}

char f_statusInfos[4096] = {0};

char *GclGa::statusInfos(int iUserId)
{
    vector<string> sReports = GeneralAccess::reportSelf();
    string sReport = CxString::join(sReports);
    if (sReport.size() < sizeof(f_statusInfos))
    {
        memcpy(f_statusInfos, sReport.data(), sReport.size());
        f_statusInfos[sReport.size()] = 0;
    }
    return f_statusInfos;
}

int GclGa::postMessageCommand(  const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach )
{
    CxMutexScope lock(f_oGaPostLock);
    return GeneralAccess::postMessage(sCommand, sParam, iParamLength, oAttach);
}

int GclGa::postFile(const std::string &sFilePath, const PsmAttach * oAttach)
{
    CxMutexScope lock(f_oGaPostLock);
    return GeneralAccess::postFile(sFilePath, oAttach);
}

int GclGa::postRealtimeDataRequest(const PsmAttach * oAttach)
{
    CxMutexScope lock(f_oGaPostLock);
    return GeneralAccess::postRealtimeDataRequest(oAttach);
}

int GclGa::postRealtimeDataPost(int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach)
{
    CxMutexScope lock(f_oGaPostLock);
    return GeneralAccess::postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, oAttach);
}

int GclGa::setReceiveErrorNotify(gci_fn_error_t fn)
{
    _fn_error = fn;
    return TRUE;
}

int GclGa::setReceiveMessageNotify(gci_fn_message_command_t fn)
{
    _fn_message_command = fn;
    return TRUE;
}

int GclGa::setReceiveFileNotify(gci_fn_file_transfer_t fn)
{
    _fn_file_transfer = fn;
    return TRUE;
}

int GclGa::setReceiveRealtimeDataRequestNotify(gci_fn_realtime_data_request_t fn)
{
    _fn_realtime_data_request = fn;
    return TRUE;
}

int GclGa::setReceiveRealtimeDataPostNotify(gci_fn_realtime_data_post_t fn)
{
    _fn_realtime_data_post = fn;
    return TRUE;
}

int GclGa::setResultFileNotify(gci_fn_file_transfer_t fn)
{
    _fn_file_transfer_result = fn;
    return TRUE;
}

int GclGa::received_message_command(const PsmExplainer *oPsm, const string &sCommand, const string &sParam, const PsmAttach *oAttach)
{
    if (sCommand == "error" && _fn_error)
    {
        int iError = 1;
        _fn_error(iError);
    }
    else if (_fn_message_command)
    {
        _fn_message_command(sCommand.c_str(), sParam.c_str(), sParam.size(), oAttach);
    }
    cxDebug() << "SdkClient - received message -command : " << sCommand << " ParamLength : " << sParam.size() << cxEndLine;
    return TRUE;
}

int GclGa::received_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    if (_fn_file_transfer)
    {
        std::string sFilePath;
        if (pathInfo.path.size()>0 && fileData.empty())
        {
            sFilePath = pathInfo.filePath();
        }
        else
        {
            sFilePath = CxFileSystem::mergeFilePath( CxFileSystem::mergeFilePath( CxApplication::applicationDeployPath(), "temp" ) , pathInfo.fileName );
            CxFile::save(sFilePath, fileData, std::string());
        }
        _fn_file_transfer(sFilePath.c_str(),oAttach);
    }
    cxDebug() << "SdkClient - received_file : " << pathInfo.fileName << " --> size " << pathInfo.fileSize << cxEndLine;
    return TRUE;
}

int GclGa::received_realtime_data_request(const PsmExplainer *oPsm, const PsmAttach *oAttach)
{
    if (_fn_realtime_data_request)
    {
        _fn_realtime_data_request(oAttach);
    }
//    stringstream sInfo;
//    sInfo << "SdkClient - received -realtime_data -request";
//    if (oAttach)
//        sInfo << " -containerId=" << oAttach->containerId;
//    cxPrompt() << sInfo.str() << cxEndLine;
    return TRUE;
}

int GclGa::received_realtime_data_post(const PsmExplainer *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    if (_fn_realtime_data_post)
    {
        _fn_realtime_data_post(iParamType, pData, iDataLength, iParamCount, oAttach);
    }
//    cxPrompt() << "SdkClient - received -realtime_data -post -DataLength=" << iDataLength << " -ParamType=" << iParamType << " -ParamCount=" << iParamCount << cxEndLine;
    return TRUE;
}

int GclGa::result_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const PsmAttach *oAttach)
{
    if (_fn_file_transfer_result)
    {
        std::string sFilePath = pathInfo.filePath();
        _fn_file_transfer_result(sFilePath.c_str(),oAttach);
    }
    cxDebug() << "SdkClient - result_file : " << pathInfo.filePath();
    return TRUE;
}

int GclGa::realtimeDataGetArray(int iBeginMid, int iMidCount, int iStructType, char *pOutResult, int iOutResultLength, const PsmAttach *oAttach)
{
    GM_INVALID_RETURE_(pOutResult, 0);

    int iResult = 0;

    switch (iStructType) {

        //*yx
        //遥信处理 vqt整形方式
    case ci_yx_address_int32_vqt_t:
    {
        MonsbMemoryManager * oMemoryManager = MonsbShareMemory::memoryManager();

        GM_INVALID_RETURE_(iMidCount>0 && iMidCount<oMemoryManager->measureCount(), 0);
        GM_INVALID_RETURE_(iOutResultLength >= sizeof(gcd_address_int32_vqt_t)*iMidCount, 0);
        GM_INVALID_RETURE_(iBeginMid >= oMemoryManager->getMinId(), 0);
        int iEndMid = iBeginMid + iMidCount;
        GM_INVALID_RETURE_(iEndMid <= oMemoryManager->getMaxId(), 0);

        gcd_address_int32_vqt_t * oYx = (gcd_address_int32_vqt_t *)pOutResult;
        for (int i = iBeginMid; i < iEndMid; ++i)
        {
            MonsbShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(i);
            if (! oMeasureData) continue;
            oYx->address = oMeasureData->measureId;
            oYx->value = oMeasureData->value;
            oYx->quality = oMeasureData->quality;
            oYx->datetime = oMeasureData->changedTime;
            oYx++;
            iResult++;
        }
    }
        break;

        //*yc
        //遥测处理 vqt浮点方式
    case ci_yc_address_double_vqt_t:
    {
        YcaddMemoryManager * oMemoryManager = YcaddShareMemory::memoryManager();

        GM_INVALID_RETURE_(iMidCount>0 && iMidCount<oMemoryManager->measureCount(), 0);
        GM_INVALID_RETURE_(iOutResultLength >= sizeof(gcd_address_double_vqt_t)*iMidCount, 0);
        GM_INVALID_RETURE_(iBeginMid >= oMemoryManager->getMinId(), 0);
        int iEndMid = iBeginMid + iMidCount;
        GM_INVALID_RETURE_(iEndMid <= oMemoryManager->getMaxId(), 0);

        gcd_address_double_vqt_t * oYc = (gcd_address_double_vqt_t *)pOutResult;
        for (int i = iBeginMid; i < iEndMid; ++i)
        {
            YcaddShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(i);
            if (! oMeasureData) continue;
            oYc->address = oMeasureData->measureId;
            oYc->value = oMeasureData->value;
            oYc->quality = oMeasureData->quality;
            oYc->datetime = oMeasureData->changedTime;
            oYc++;
            iResult++;
        }
    }
        break;

        //*yw
        //遥测处理 vqt文本方式
    case ci_yw_address_vqt_string128_t:
    {
        StrawMemoryManager * oMemoryManager = StrawShareMemory::memoryManager();

        GM_INVALID_RETURE_(iMidCount>0 && iMidCount<oMemoryManager->measureCount(), 0);
        GM_INVALID_RETURE_(iOutResultLength >= sizeof(gcd_address_vqt_string128_t)*iMidCount, 0);
        GM_INVALID_RETURE_(iBeginMid >= oMemoryManager->getMinId(), 0);
        int iEndMid = iBeginMid + iMidCount;
        GM_INVALID_RETURE_(iEndMid <= oMemoryManager->getMaxId(), 0);

        gcd_address_vqt_string128_t * oYw = (gcd_address_vqt_string128_t *)pOutResult;
        for (int i = iBeginMid; i < iEndMid; ++i)
        {
            StrawShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(i);
            if (! oMeasureData) continue;
            oYw->address = oMeasureData->measureId;
            memcpy(oYw->value, oMeasureData->value.value, 128);
            oYw->quality = oMeasureData->quality;
            oYw->datetime = oMeasureData->changedTime;
            oYw++;
            iResult++;
        }
    }
        break;

    default:
        break;
    }

    return iResult;
}

int GclGa::realtimeDataGetList(int iStructType, char *pOutResult, int iOutResultLength, int iStructCount, const PsmAttach *oAttach)
{
    GM_INVALID_RETURE_(pOutResult, 0);

    int iResult = 0;

    switch (iStructType) {

        //*yx
        //遥信处理 vqt整形方式
    case ci_yx_address_int32_vqt_t:
    {
        MonsbMemoryManager * oMemoryManager = MonsbShareMemory::memoryManager();

        GM_INVALID_RETURE_(iStructCount>0 && iStructCount<oMemoryManager->measureCount(), 0);
        GM_INVALID_RETURE_(iOutResultLength >= sizeof(gcd_address_int32_vqt_t)*iStructCount, 0);

        gcd_address_int32_vqt_t * oYx = (gcd_address_int32_vqt_t *)pOutResult;
        for (int i = 0; i < iStructCount; ++i)
        {
            MonsbShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(oYx->address);
            if (! oMeasureData) continue;
            oYx->value = oMeasureData->value;
            oYx->quality = oMeasureData->quality;
            oYx->datetime = oMeasureData->changedTime;
            oYx++;
            iResult++;
        }
    }
        break;

        //*yc
        //遥测处理 vqt浮点方式
    case ci_yc_address_double_vqt_t:
    {
        YcaddMemoryManager * oMemoryManager = YcaddShareMemory::memoryManager();

        GM_INVALID_RETURE_(iStructCount>0 && iStructCount<oMemoryManager->measureCount(), 0);
        GM_INVALID_RETURE_(iOutResultLength >= sizeof(gcd_address_double_vqt_t)*iStructCount, 0);

        gcd_address_double_vqt_t * oYc = (gcd_address_double_vqt_t *)pOutResult;
        for (int i = 0; i < iStructCount; ++i)
        {
            YcaddShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(oYc->address);
            if (! oMeasureData) continue;
            oYc->value = oMeasureData->value;
            oYc->quality = oMeasureData->quality;
            oYc->datetime = oMeasureData->changedTime;
            oYc++;
            iResult++;
        }
    }
        break;

        //*yw
        //遥测处理 vqt文本方式
    case ci_yw_address_vqt_string128_t:
    {
        StrawMemoryManager * oMemoryManager = StrawShareMemory::memoryManager();

        GM_INVALID_RETURE_(iStructCount>0 && iStructCount<oMemoryManager->measureCount(), 0);
        GM_INVALID_RETURE_(iOutResultLength >= sizeof(gcd_address_vqt_string128_t)*iStructCount, 0);

        gcd_address_vqt_string128_t * oYw = (gcd_address_vqt_string128_t *)pOutResult;
        for (int i = 0; i < iStructCount; ++i)
        {
            StrawShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(oYw->address);
            if (! oMeasureData) continue;
            memcpy(oYw->value, oMeasureData->value.value, 128);
            oYw->quality = oMeasureData->quality;
            oYw->datetime = oMeasureData->changedTime;
            oYw++;
            iResult++;
        }
    }
        break;

    default:
        break;
    }

    return iResult;
}

int GclGa::realtimeDataGet(int iMid, int iStructType, char *pOutResult, int iOutResultLength, const PsmAttach *oAttach)
{
    GM_INVALID_RETURE_(pOutResult, 0);
    GM_INVALID_RETURE_(iOutResultLength >= sizeof(gcd_address_int32_vqt_t)*1, 0);

    gcd_address_int32_vqt_t * oYx = (gcd_address_int32_vqt_t *)pOutResult;
    oYx->address = iMid;
    return realtimeDataGetList(iStructType, pOutResult, iOutResultLength, 1, oAttach);
}

int GclGa::realtimeDataSetList(int iStructType, char *pStructData, int iStructDataLength, int iStructCount, const PsmAttach *oAttach)
{
    GM_INVALID_RETURE_(pStructData, 0);
    GM_INVALID_RETURE_(iStructCount>0, 0);

    switch (iStructType) {

        //*yx
        //遥信处理 vqt整形方式
    case ci_yx_address_int32_vqt_t:
    {
        GM_INVALID_RETURE_(iStructDataLength >= sizeof(gcd_address_int32_vqt_t)*iStructCount, 0);
        return GeneralAccess::postRealtimeDatas((const gcd_address_int32_vqt_t *)pStructData, iStructCount, oAttach) / sizeof(gcd_address_int32_vqt_t);
    }
        break;

        //*yc
        //遥测处理 vqt浮点方式
    case ci_yc_address_double_vqt_t:
    {
        GM_INVALID_RETURE_(iStructDataLength >= sizeof(gcd_address_double_vqt_t)*iStructCount, 0);
        return GeneralAccess::postRealtimeDatas((const gcd_address_double_vqt_t *)pStructData, iStructCount, oAttach) / sizeof(gcd_address_double_vqt_t);
    }
        break;

        //*yw
        //遥测处理 vqt文本方式
    case ci_yw_address_vqt_string128_t:
    {
        GM_INVALID_RETURE_(iStructDataLength >= sizeof(gcd_address_vqt_string128_t)*iStructCount, 0);
        return GeneralAccess::postRealtimeDatas((const gcd_address_vqt_string128_t *)pStructData, iStructCount, oAttach) / sizeof(gcd_address_vqt_string128_t);
    }
        break;

    default:
        break;
    }

    return 0;
}

int GclGa::realtimeDataSet(int iStructType, char *pStructData, int iStructDataLength, const PsmAttach *oAttach)
{
    return realtimeDataSetList(iStructType, pStructData, iStructDataLength, 1, oAttach);
}

