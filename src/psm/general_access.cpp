#include "general_access.h"

#include <ccxx/cxapplication.h>
#include <ccxx/cxchannel_udp.h>
#include <ccxx/cxchannel_tcpclient.h>

using namespace std;


static CxChannelBase * f_oChannel = NULL;
static PsmExplainer * f_oPsm = NULL;

static PsmAttach f_defaultAttach;
static int f_iHeartJumpInterval = 0;



GeneralAccess * fn_oGeneralAccess()
{
    static GeneralAccess m;
    //订阅控制台输入命令
    CxInterinfoIn::addObserver(GeneralAccess::interinfoIn_line);
    CxInterinfoIn::addObserver(GeneralAccess::interinfoIn_cmd);
    //注册 Application 的 Start 与 Stop 回调
    CxApplication::registStartFunction(GeneralAccess::start);
    CxApplication::registStopFunction(GeneralAccess::stop);
    return &m;
}

GeneralAccess * f_oGeneralAccess = fn_oGeneralAccess();

int GeneralAccess::interinfoIn_line(const std::string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

int GeneralAccess::interinfoIn_cmd(const std::string &sCommand, const std::map<std::string, std::string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

int GeneralAccess::postMessage(const char *sCommand, const char *sParam, int iParamLength, const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if(oAttach)
            return f_oPsm->postMessageCommand(sCommand, sParam, iParamLength, oAttach);
        else
            return f_oPsm->postMessageCommand(sCommand, sParam, iParamLength, &f_defaultAttach);
    }
    return -1;
}

int GeneralAccess::postFile(const string &sFilePath, const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if(oAttach)
            return f_oPsm->postFile(sFilePath, oAttach);
        else
            return f_oPsm->postFile(sFilePath, &f_defaultAttach);
    }
    return  -1;
}

int GeneralAccess::postFile(const CxFileSystem::PathInfo & pathInfo, const std::vector<string> & fileData, const PsmAttach * oAttach)
{
    if (f_oPsm)
    {
        if(oAttach)
            return f_oPsm->postFile(pathInfo, fileData, oAttach);
        else {
            return f_oPsm->postFile(pathInfo, fileData, &f_defaultAttach);
        }
    }
    return  -1;
}

int GeneralAccess::postRealtimeDataRequest(const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if(oAttach)
           return f_oPsm->postRealtimeDataRequest(oAttach);
        else
           return f_oPsm->postRealtimeDataRequest(&f_defaultAttach);
    }
    return  -1;
}

int GeneralAccess::postRealtimeDataPost(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if(oAttach)
            return f_oPsm->postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, oAttach);
        else
            return f_oPsm->postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, &f_defaultAttach);
    }
    return  -1;
}

int GeneralAccess::postRealtimeDatas(const gcd_address_int32_vqt_t *oYxs, int iYxCount, const PsmAttach *oAttach)
{
    int iResult = 0;
    int iSent = 0;
    int iCount;

    while (iSent < iYxCount)
    {
        const gcd_address_int32_vqt_t * oYx = oYxs+iSent;
        iCount = iYxCount - iSent > ci_psm_packet_default_count(gcd_address_int32_vqt_t) ? ci_psm_packet_default_count(gcd_address_int32_vqt_t) : iYxCount - iSent;
        int iSendByte = GeneralAccess::postRealtimeDataPost(ci_yx_address_int32_vqt_t, (const char *)(oYx), iCount*sizeof(gcd_address_int32_vqt_t), iCount, oAttach);
        if(iSendByte>0) iResult +=iSendByte;
        else //发送失败
        {
            iResult = -1;
            break;
        }
        iSent += iCount;
    }
    return iResult;
}

int GeneralAccess::postRealtimeDatas(const std::vector<gcd_address_int32_vqt_t> &yxs, const PsmAttach *oAttach)
{
    return postRealtimeDatas(& yxs.front(), yxs.size(), oAttach);
}

int GeneralAccess::postRealtimeData(const gcd_address_int32_vqt_t &yx, const PsmAttach *oAttach)
{
    return GeneralAccess::postRealtimeDataPost(ci_yx_address_int32_vqt_t, (const char *)(&yx), sizeof(gcd_address_int32_vqt_t), 1, oAttach);
}

int GeneralAccess::postRealtimeDatas(const gcd_address_double_vqt_t *oYcs, int iYcCount, const PsmAttach *oAttach)
{
    int iResult = 0;
    int iSent = 0;
    int iCount;
    while (iSent < iYcCount)
    {
        const gcd_address_double_vqt_t * oYc = oYcs+iSent;
        iCount = iYcCount - iSent > ci_psm_packet_default_count(gcd_address_double_vqt_t) ? ci_psm_packet_default_count(gcd_address_double_vqt_t) : iYcCount - iSent;
        int iSendByte = GeneralAccess::postRealtimeDataPost(ci_yc_address_double_vqt_t, (const char *)(oYc), iCount*sizeof(gcd_address_double_vqt_t), iCount, oAttach);
        if(iSendByte>0) iResult +=iSendByte;
        else//发送失败
        {
            iResult = -1;
            break;
        }
        iSent += iCount;
    }
    return iResult;
}

int GeneralAccess::postRealtimeDatas(const std::vector<gcd_address_double_vqt_t> &ycs, const PsmAttach *oAttach)
{
    return postRealtimeDatas(& ycs.front(), ycs.size(), oAttach);
}

int GeneralAccess::postRealtimeData(const gcd_address_double_vqt_t &yc, const PsmAttach *oAttach)
{
    return GeneralAccess::postRealtimeDataPost(ci_yc_address_double_vqt_t, (const char *)(&yc), sizeof(gcd_address_double_vqt_t), 1, oAttach);
}

int GeneralAccess::postRealtimeDatas(const gcd_address_vqt_string128_t * oYws, int iYwCount, const PsmAttach *oAttach)
{
    int iResult = 0;
    int iSent = 0;
    int iCount;
    while (iSent < iYwCount)
    {
        const gcd_address_vqt_string128_t * oYw = oYws+iSent;
        iCount = iYwCount - iSent > ci_psm_packet_default_count(gcd_address_vqt_string128_t) ? ci_psm_packet_default_count(gcd_address_vqt_string128_t) : iYwCount - iSent;
        int iSendByte =  GeneralAccess::postRealtimeDataPost(ci_yw_address_vqt_string128_t, (const char *)(oYw), iCount*sizeof(gcd_address_vqt_string128_t), iCount, oAttach);
        if(iSendByte>0) iResult +=iSendByte;
        else //发送失败
        {
            iResult = -1;
            break;
        }

        iSent += iCount;
    }
    return iResult;
}

int GeneralAccess::postRealtimeDatas(const std::vector<gcd_address_vqt_string128_t> &yws, const PsmAttach *oAttach)
{
    return postRealtimeDatas(& yws.front(), yws.size(), oAttach);
}

int GeneralAccess::postRealtimeData(const gcd_address_vqt_string128_t &yw, const PsmAttach *oAttach)
{
    return GeneralAccess::postRealtimeDataPost(ci_yw_address_vqt_string128_t, (const char *)(&yw), sizeof(gcd_address_vqt_string128_t), 1, oAttach);
}

void GeneralAccess::timerTimeoutHeartJump(int iInterval)
{
    if (f_oPsm)
    {
        f_oPsm->postHeartJump(& f_defaultAttach);
    }
}

int GeneralAccess::heartJumpInterval()
{
    return f_iHeartJumpInterval;
}

bool GeneralAccess::isHeartJumpOnline()
{
    if (f_oPsm && f_iHeartJumpInterval>0)
    {
        return CxTime::milliSecondDifferToNow(f_oPsm->lastReceivedDataTime()) < f_iHeartJumpInterval;
    }
    else
    {
        return false;
    }
}

void GeneralAccess::registerMessageCommandDeal(fn_psm_explain_received_message_command_t fn, int iIndex)
{
    vector<fn_psm_explain_received_message_command_t> * oFnDeals = getFnMessageCommandDeals();
    if (std::find(oFnDeals->begin(), oFnDeals->end(), fn) == oFnDeals->end())
    {
        if (iIndex > -1 )
        {
            oFnDeals->insert(oFnDeals->begin(), 1, fn);
        }
        else
        {
            oFnDeals->push_back(fn);
        }
    }
}

void GeneralAccess::unregisterMessageCommandDeal(fn_psm_explain_received_message_command_t fn)
{
    vector<fn_psm_explain_received_message_command_t> * oFnDeals = getFnMessageCommandDeals();
    CxContainer::remove(*oFnDeals, fn);
}

void GeneralAccess::registerFileWriteDeal(fn_psm_explain_received_file_write_t fn, int iIndex)
{
    vector<fn_psm_explain_received_file_write_t> * oFnDeals = getFnFileWriteDeals();
    if (std::find(oFnDeals->begin(), oFnDeals->end(), fn) == oFnDeals->end())
    {
        if (iIndex > -1 )
        {
            oFnDeals->insert(oFnDeals->begin(), 1, fn);
        }
        else
        {
            oFnDeals->push_back(fn);
        }
    }
}

void GeneralAccess::unregisterFileWriteDeal(fn_psm_explain_received_file_write_t fn)
{
    vector<fn_psm_explain_received_file_write_t> * oFnDeals = getFnFileWriteDeals();
    CxContainer::remove(*oFnDeals, fn);
}

void GeneralAccess::registerRealtimeDataRequestDeal(fn_psm_explain_received_realtime_data_request_t fn, int iIndex)
{
    vector<fn_psm_explain_received_realtime_data_request_t> * oFnDeals = getFnRealtimeDataRequestDeals();
    if (std::find(oFnDeals->begin(), oFnDeals->end(), fn) == oFnDeals->end())
    {
        if (iIndex > -1 )
        {
            oFnDeals->insert(oFnDeals->begin(), 1, fn);
        }
        else
        {
            oFnDeals->push_back(fn);
        }
    }
}

void GeneralAccess::unregisterRealtimeDataRequestDeal(fn_psm_explain_received_realtime_data_request_t fn)
{
    vector<fn_psm_explain_received_realtime_data_request_t> * oFnDeals = getFnRealtimeDataRequestDeals();
    CxContainer::remove(*oFnDeals, fn);
}

void GeneralAccess::registerRealtimeDataPostDeal(fn_psm_explain_received_realtime_data_post_t fn, int iIndex)
{
    vector<fn_psm_explain_received_realtime_data_post_t> * oFnDeals = getFnRealtimeDataPostDeals();
    if (std::find(oFnDeals->begin(), oFnDeals->end(), fn) == oFnDeals->end())
    {
        if (iIndex > -1 )
        {
            oFnDeals->insert(oFnDeals->begin(), 1, fn);
        }
        else
        {
            oFnDeals->push_back(fn);
        }
    }
}

void GeneralAccess::unregisterRealtimeDataPostDeal(fn_psm_explain_received_realtime_data_post_t fn)
{
    vector<fn_psm_explain_received_realtime_data_post_t> * oFnDeals = getFnRealtimeDataPostDeals();
    CxContainer::remove(*oFnDeals, fn);
}

void GeneralAccess::registerResultFileWriteDeal(fn_psm_explain_result_file_write_t fn, int iIndex)
{
    vector<fn_psm_explain_result_file_write_t> * oFnDeals = getFnResultFileWriteDeals();
    if (std::find(oFnDeals->begin(), oFnDeals->end(), fn) == oFnDeals->end())
    {
        if (iIndex > -1 )
        {
            oFnDeals->insert(oFnDeals->begin(), 1, fn);
        }
        else
        {
            oFnDeals->push_back(fn);
        }
    }
}

void GeneralAccess::unregisterResultFileWriteDeal(fn_psm_explain_result_file_write_t fn)
{
    vector<fn_psm_explain_result_file_write_t> * oFnDeals = getFnResultFileWriteDeals();
    CxContainer::remove(*oFnDeals, fn);
}

void GeneralAccess::raiseReceivedFile(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    vector<fn_psm_explain_received_file_write_t> * oFnDeals = getFnFileWriteDeals();
    for (size_t i = 0; i < oFnDeals->size(); ++i)
    {
        fn_psm_explain_received_file_write_t fnDeal = oFnDeals->at(i);
        if (fnDeal(oPsm, pathInfo, fileData, oAttach) > 0)
            break;
    }
}

std::vector<fn_psm_explain_received_message_command_t> *GeneralAccess::getFnMessageCommandDeals()
{
    static vector<fn_psm_explain_received_message_command_t> m;
    return & m;
}

std::vector<fn_psm_explain_received_file_write_t> *GeneralAccess::getFnFileWriteDeals()
{
    static vector<fn_psm_explain_received_file_write_t> m;
    return & m;
}

std::vector<fn_psm_explain_received_realtime_data_request_t> *GeneralAccess::getFnRealtimeDataRequestDeals()
{
    static vector<fn_psm_explain_received_realtime_data_request_t> m;
    return & m;
}

std::vector<fn_psm_explain_received_realtime_data_post_t> *GeneralAccess::getFnRealtimeDataPostDeals()
{
    static vector<fn_psm_explain_received_realtime_data_post_t> m;
    return & m;
}

std::vector<fn_psm_explain_result_file_write_t> *GeneralAccess::getFnResultFileWriteDeals()
{
    static vector<fn_psm_explain_result_file_write_t> m;
    return & m;
}

void GeneralAccess::start()
{
    GM_VALID_RETURE(f_oChannel);
    GM_VALID_RETURE(f_oPsm);

    GeneralAccess * oGeneralAccess = fn_oGeneralAccess();

    std::string sChannelTypeSimpleName = CxApplication::findConfig( CS_SectionGeneralAccess , "Channel", std::string());
    if (sChannelTypeSimpleName.size() > 0)
    {
        f_oChannel  = CxChannelManager::createChannel(sChannelTypeSimpleName);
        if (f_oChannel)
        {
            f_oChannel->addObserver(oGeneralAccess);
            CxGetSkvMap skv(CxApplication::findConfigs(CS_SectionGeneralAccess));
            f_oChannel->fromContext(skv);
            f_oPsm = new PsmExplainer();
            f_oPsm->registerPsmExplainNotify(oGeneralAccess);
            f_oPsm->registerPsmExplainWrite(oGeneralAccess);

            f_oChannel->open();
            f_oPsm->setIsProcessQueue(f_oChannel->isProcessQueue());
            f_oPsm->start();

            cxPrompt() << "GeneralAccess Start :";
            cxPrompt() << "Channel Open : \n" << f_oChannel->reportSelf();
            cxPrompt() << "Psm Start : \n" << f_oPsm->reportSelf();
        }
        else
        {
            cxPrompt() << "GeneralAccess Start Fail :" << "Config Channel Is Empty!";
        }

        //1、加载配置，如果配有 Channel 就加载，并且加载 PsmExplainer
        f_defaultAttach.reason = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentReason, f_defaultAttach.reason, true);
        f_defaultAttach.containerId = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentContainerId, f_defaultAttach.containerId, true);
        f_defaultAttach.sourceId = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentSourceId, f_defaultAttach.sourceId, true);
        f_defaultAttach.targetId = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentTargetId, f_defaultAttach.targetId, true);
        f_defaultAttach.tag = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentTag, f_defaultAttach.tag, true);

        f_iHeartJumpInterval = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmHeartJumpInterval, f_iHeartJumpInterval, true);
        if (f_iHeartJumpInterval>=3000 && f_oChannel && f_oPsm)
        {
            CxTimerManager::startTimer(GeneralAccess::timerTimeoutHeartJump, f_iHeartJumpInterval);
        }
    }
}

void GeneralAccess::stop()
{
    if (f_oChannel)
    {
        f_oChannel->close();
        delete f_oChannel;
        f_oChannel = NULL;
    }

    if (f_oPsm)
    {
        f_oPsm->stop();
        f_oPsm->registerPsmExplainNotify(NULL);
        f_oPsm->registerPsmExplainWrite(NULL);
        f_oPsm = NULL;
    }
}

bool GeneralAccess::isConnected()
{
    return f_oPsm && f_oChannel && f_oChannel->connected();
}

const PsmAttach *GeneralAccess::defaultAttach()
{
    return & f_defaultAttach;
}

void GeneralAccess::setRemoteIpAddress(const std::string& sIp, ushort iPort)
{
    if (f_oChannel)
    {
        if (f_oChannel->typeSimpleId() == CxInterinfo::Type_IO_Udp)
        {
            CxChannelUdp * oUdp = (CxChannelUdp *)f_oChannel;
            if (oUdp->remotePort() != iPort || oUdp->remoteIp() != sIp)
            {
                oUdp->setRemoteIpAddress(sIp, iPort);
                cxPrompt() << "GeneralAccess setRemoteIpAddress sIp=" << sIp << "; iPort=" << iPort << "; isSendQueue=" << oUdp->isSendQueue();
            }
        }
    }
}

const CxChannelBase *GeneralAccess::getChannel()
{
    return f_oChannel;
}

msepoch_t GeneralAccess::lastReceivedProtocolDataTime()
{
    return f_oPsm ? f_oPsm->lastReceivedDataTime() : 0;
}

std::vector<string> GeneralAccess::reportSelf()
{
    std::vector<string> sReports;

    string sRemoteIpAddress   ;
    string sRemotePort        ;
    string sLocalIpAddress    ;
    string sLocalPort         ;
    string sReceivedByteCount ;
    string sSentByteCount     ;
    string sLastReceivedTime  ;
    string sLastSentTime      ;

    if (f_oChannel)
    {
        if (f_oChannel->typeSimpleId() == CxInterinfo::Type_IO_Udp)
        {
            CxChannelUdp * oUdp = (CxChannelUdp *)f_oChannel;
            sRemoteIpAddress = oUdp->remoteIp();
            sRemotePort      = CxString::toString(oUdp->remotePort());
            sLocalIpAddress  = oUdp->localIp();
            sLocalPort       = CxString::toString(oUdp->localPort());
        }
        if (f_oChannel->typeSimpleId() == CxInterinfo::Type_IO_Tcpclient)
        {
            CxChannelTcpclient * oTcpclient = (CxChannelTcpclient *)f_oChannel;
            sRemoteIpAddress = oTcpclient->remoteIp();
            sRemotePort      = CxString::toString(oTcpclient->remotePort());
        }
        sReceivedByteCount  = CxString::toString(f_oChannel->receivedByteCount());
        sSentByteCount      = CxString::toString(f_oChannel->sentByteCount    ());
        sLastReceivedTime   = CxTime::toString(f_oChannel->lastReceivedTime ());
        sLastSentTime       = CxTime::toString(f_oChannel->lastSentTime     ());
    }

    sReports.push_back( "IsConnected=" + CxString::toString(isConnected()));
    sReports.push_back( CxString::format("%s=%d", CS_EntryPsmSentSourceId.c_str(), f_defaultAttach.sourceId));
    sReports.push_back( CxString::format("%s=%d", CS_EntryPsmSentTargetId.c_str(), f_defaultAttach.targetId));
    sReports.push_back( CxString::format("%s=%d", CS_EntryPsmSentReason.c_str(), f_defaultAttach.reason));
    sReports.push_back(CS_EntryLocalIpAddress   + "=" + sLocalIpAddress);
    sReports.push_back(CS_EntryLocalPort        + "=" + sLocalPort     );
    sReports.push_back(CS_EntryRemoteIpAddress  + "=" + sRemoteIpAddress);
    sReports.push_back(CS_EntryRemotePort       + "=" + sRemotePort    );
    sReports.push_back( "ReceivedByteCount=" + sReceivedByteCount);
    sReports.push_back( "SentByteCount=" + sSentByteCount    );
    sReports.push_back( "LastReceivedTime=" + sLastReceivedTime );
    sReports.push_back( "LastSentTime=" + sLastSentTime     );
    return sReports;
}

void GeneralAccess::channel_receivedData(const uchar *pData, int iLength, void *oSource)
{
    if (f_oPsm)
    {
        f_oPsm->receivedData(pData, iLength);
    }
}

int GeneralAccess::send_data(const char *pData, int iLength, int iTargetId)
{
    if (f_oChannel)
    {
        return f_oChannel->sendData(pData, iLength);
    }
    return 0;
}

void GeneralAccess::received_message_command(const PsmExplainer *oPsm, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach *oAttach)
{
    string sCommand(pCommand);
    string sParam(pParam, iParamLength);
    vector<fn_psm_explain_received_message_command_t> * oFnDeals = getFnMessageCommandDeals();
    for (size_t i = 0; i < oFnDeals->size(); ++i)
    {
        fn_psm_explain_received_message_command_t fnDeal = oFnDeals->at(i);
        if (fnDeal(oPsm, sCommand, sParam, oAttach) > 0)
            break;
    }
}

void GeneralAccess::received_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    vector<fn_psm_explain_received_file_write_t> * oFnDeals = getFnFileWriteDeals();
    for (size_t i = 0; i < oFnDeals->size(); ++i)
    {
        fn_psm_explain_received_file_write_t fnDeal = oFnDeals->at(i);
        if (fnDeal(oPsm, pathInfo, fileData, oAttach) > 0)
            break;
    }
}

void GeneralAccess::received_realtime_data_request(const PsmExplainer *oPsm, const PsmAttach *oAttach)
{
    vector<fn_psm_explain_received_realtime_data_request_t> * oFnDeals = getFnRealtimeDataRequestDeals();
    for (size_t i = 0; i < oFnDeals->size(); ++i)
    {
        fn_psm_explain_received_realtime_data_request_t fnDeal = oFnDeals->at(i);
        if (fnDeal(oPsm, oAttach) > 0)
            break;
    }
}

void GeneralAccess::received_realtime_data_post(const PsmExplainer *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    vector<fn_psm_explain_received_realtime_data_post_t> * oFnDeals = getFnRealtimeDataPostDeals();
    for (size_t i = 0; i < oFnDeals->size(); ++i)
    {
        fn_psm_explain_received_realtime_data_post_t fnDeal = oFnDeals->at(i);
        if (fnDeal(oPsm, iParamType, pData, iDataLength, iParamCount, oAttach) > 0)
            break;
    }
}

void GeneralAccess::result_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const PsmAttach *oAttach)
{
    vector<fn_psm_explain_result_file_write_t> * oFnDeals = getFnResultFileWriteDeals();
    for (size_t i = 0; i < oFnDeals->size(); ++i)
    {
        fn_psm_explain_result_file_write_t fnDeal = oFnDeals->at(i);
        if (fnDeal(oPsm, pathInfo, oAttach) > 0)
            break;
    }
}
