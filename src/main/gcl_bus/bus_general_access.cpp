#include "bus_general_access.h"


#include <ccxx/cxapplication.h>


#include "bus_data_deal.h"
#include <common/gcl_macro.h>

using namespace std;


CxChannelBase * f_oChannel = NULL;
CxChannelRoad * f_oRoad = NULL;
PsmExplainer * f_oPsm = NULL;

int f_iGaSourceId = 0;
PsmAttach f_defaultAttach;
int f_iHeartJumpInterval = 0;



BusGeneralAccess * fn_oGeneralAccess()
{
    static BusGeneralAccess m;
    //订阅控制台输入命令
    CxInterinfoIn::addObserver(BusGeneralAccess::interinfoIn_line);
    CxInterinfoIn::addObserver(BusGeneralAccess::interinfoIn_cmd);
    //注册 Application 的 Start 与 Stop 回调
    CxApplication::registStartFunction(BusGeneralAccess::start, 11);
    CxApplication::registStopFunction(BusGeneralAccess::stop);
    return &m;
}

BusGeneralAccess * f_oGeneralAccess = fn_oGeneralAccess();

int BusGeneralAccess::interinfoIn_line(const std::string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

int BusGeneralAccess::interinfoIn_cmd(const std::string &sCommand, const std::map<std::string, std::string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

int BusGeneralAccess::postMessage(const char *sCommand, const char *sParam, int iParamLength, const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if (oAttach)
            return f_oPsm->postMessageCommand(sCommand, sParam, iParamLength, oAttach);
        else
            return f_oPsm->postMessageCommand(sCommand, sParam, iParamLength, & f_defaultAttach);
    }
    return -1;
}

int BusGeneralAccess::postFile(const string &sFilePath, const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if (oAttach)
            return f_oPsm->postFile(sFilePath, oAttach);
        else
            return f_oPsm->postFile(sFilePath, & f_defaultAttach);
    }
    return  -1;
}

int BusGeneralAccess::postFile(const CxFileSystem::PathInfo & pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if (oAttach)
            return f_oPsm->postFile(pathInfo, fileData, oAttach);
        else
            return f_oPsm->postFile(pathInfo, fileData, & f_defaultAttach);
    }
    return  -1;
}

int BusGeneralAccess::postRealtimeDataRequest(const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if (oAttach)
            return f_oPsm->postRealtimeDataRequest(oAttach);
        else
            return f_oPsm->postRealtimeDataRequest(& f_defaultAttach);
    }
    return  -1;
}

int BusGeneralAccess::postRealtimeDataPost(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    if (f_oPsm)
    {
        if (oAttach)
            return f_oPsm->postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, oAttach);
        else
            return f_oPsm->postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, & f_defaultAttach);
    }
    return  -1;
}

int BusGeneralAccess::postPacketData(const psm_packet_body_t &body, const char *pData, int iDataLength)
{
    if (f_oPsm)
    {
        return f_oPsm->postPacketData(body, pData, iDataLength);
    }
    return  -1;
}

void BusGeneralAccess::timerTimeoutHeartJump(int iInterval)
{
    if (f_oPsm)
    {
        f_oPsm->postHeartJump(& f_defaultAttach);
    }
}

void BusGeneralAccess::start()
{
    init("");
}

void BusGeneralAccess::stop()
{
    //删除 _channel _psm
    CxTimerManager::stopTimer(BusGeneralAccess::timerTimeoutHeartJump);
    if (f_oPsm)
    {
        f_oPsm->stop();
        f_oPsm->registerPsmExplainNotify(NULL);
        f_oPsm->registerPsmExplainWrite(NULL);
    }

    GM_DELETEANDNULL_OBJECT(f_oChannel);
    GM_DELETEANDNULL_OBJECT(f_oPsm);
}

bool BusGeneralAccess::isConnected()
{
    return f_oPsm && f_oChannel && f_oChannel->connected();
}

int BusGeneralAccess::getSourceId()
{
    return f_iGaSourceId;
}

const PsmAttach *BusGeneralAccess::defaultAttach()
{
    return & f_defaultAttach;
}

void BusGeneralAccess::init(const string &sDeaultChannelTypeSimpleName)
{
    GM_VALID_RETURE(f_oChannel);
    GM_VALID_RETURE(f_oPsm);

    BusGeneralAccess * oGeneralAccess = fn_oGeneralAccess();

    std::string sChannelTypeSimpleName = CxApplication::findConfig( CS_SectionGeneralAccess , "Channel", sDeaultChannelTypeSimpleName, ! sDeaultChannelTypeSimpleName.empty());
    if (sChannelTypeSimpleName.size() > 0)
    {
        //1、加载配置，如果配有 Channel 就加载，并且加载 PsmExplainer
        f_iGaSourceId = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentSourceId, f_iGaSourceId, true);

        f_defaultAttach.reason = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentReason, f_defaultAttach.reason, true);
        f_defaultAttach.containerId = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentContainerId, f_defaultAttach.containerId, true);
        f_defaultAttach.sourceId = f_iGaSourceId;
        f_defaultAttach.targetId = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentTargetId, f_defaultAttach.targetId, true);
        f_defaultAttach.tag = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmSentTag, f_defaultAttach.tag, true);

        f_oChannel  = CxChannelManager::createChannel(sChannelTypeSimpleName);
        if (f_oChannel)
        {
            f_oChannel->addObserver(oGeneralAccess);
            CxGetSkvMap skv(CxApplication::findConfigs(CS_SectionGeneralAccess));
            f_oChannel->fromContext(skv);

            f_oPsm = new PsmExplainer();

            f_oPsm->registerPsmExplainNotify(oGeneralAccess);
            f_oPsm->registerPsmExplainWrite(oGeneralAccess);
            f_oPsm->setSentSourceId(f_defaultAttach.sourceId);
            f_oPsm->setSentTargetId(f_defaultAttach.targetId);

            string sRemoteIp;
            int iRemotePort = 0;
            sRemoteIp = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryRemoteIpAddress, sRemoteIp, true);
            iRemotePort = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryRemotePort, iRemotePort, true);

            f_oRoad = CxChannelRoadManager::allocate(sRemoteIp, iRemotePort, f_oChannel, f_defaultAttach.sourceId);

            f_oChannel->open();
            f_oPsm->start();
        }

        f_iHeartJumpInterval = CxApplication::findConfig( CS_SectionGeneralAccess , CS_EntryPsmHeartJumpInterval, f_iHeartJumpInterval, true);
        if (f_iHeartJumpInterval>=3000 && f_oChannel && f_oPsm)
        {
            CxTimerManager::startTimer(BusGeneralAccess::timerTimeoutHeartJump, f_iHeartJumpInterval);
        }
    }
}

void BusGeneralAccess::channel_receivedData(const uchar *pData, int iLength, void *oSource)
{
    if (f_oPsm)
    {
        f_oPsm->receivedData(pData, iLength);
    }
}

int BusGeneralAccess::send_data(const char *pData, int iLength, int iTargetId)
{
    if (f_oChannel)
    {
        return f_oChannel->sendData(pData, iLength);
    }
    return -1;
}

void BusGeneralAccess::received_message_command(const PsmExplainer *oPsm, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach *oAttach)
{   
    if (oAttach && oAttach->targetId>0)
    {
        if (oAttach->sourceId == oAttach->targetId)
        {
            PsmAttach attach(oAttach->reason, oAttach->containerId, oAttach->sourceId, 0, oAttach->tag);
            BusDataDeal::dealMessageCommand(f_iGaSourceId, pCommand, pParam, iParamLength, &attach);
        }
        else
        {
            BusManager::transferMessage(oAttach->sourceId, pCommand, pParam, iParamLength, oAttach->targetId);
        }
    }
    else
    {
        BusDataDeal::dealMessageCommand(f_iGaSourceId, pCommand, pParam, iParamLength, oAttach);
    }
}

void BusGeneralAccess::received_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    if (oAttach && oAttach->targetId > 0)
    {
        if (oAttach->sourceId == oAttach->targetId)
        {
            PsmAttach attach(oAttach->reason, oAttach->containerId, oAttach->sourceId, 0, oAttach->tag);
            BusDataDeal::dealFile(f_iGaSourceId, pathInfo, fileData, &attach);
        }
        else
        {
            BusManager::transferFileStream(oAttach->sourceId, pathInfo, fileData, oAttach->targetId);
        }
    }
    else
    {
        BusDataDeal::dealFile(f_iGaSourceId, pathInfo, fileData, oAttach);
    }
}

void BusGeneralAccess::received_realtime_data_request(const PsmExplainer *oPsm, const PsmAttach *oAttach)
{
    BusDataDeal::dealRealtimeDataRequest(f_iGaSourceId, oAttach);
}

void BusGeneralAccess::received_realtime_data_post(const PsmExplainer *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    BusDataDeal::dealRealtimeDataPost(f_iGaSourceId, iParamType, pData, iDataLength, iParamCount, oAttach);
}

void BusGeneralAccess::result_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const PsmAttach *oAttach)
{
}
