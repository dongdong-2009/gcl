#include "ntp_server.h"


#include <common/gcl_resource.h>


using namespace std;


static const std::string CS_SectionNtpServer = "NtpServer";
static const std::string CS_EntryNtpUdpPort = "UdpServerPort";
static const std::string CS_EntryNtpTcpPort = "TcpServerPort";


NtpServer * fn_oBusManager()
{
    CxApplication::registStartFunction(NtpServer::init);
    CxApplication::registStopFunction(NtpServer::uninit);

    CxInterinfoIn::addObserver(NtpServer::interinfoIn_line);
    CxInterinfoIn::addObserver(NtpServer::interinfoIn_cmd);

    static NtpServer m;
    return &m;
}

static NtpServer * f_oBusManager = fn_oBusManager();

static map<CxChannelRoad *, NtpTerminal*> f_oRoadTerminals;







//* BusTerminal
NtpTerminal::NtpTerminal(CxChannelRoad *value) :
    _psmExplainer(),
    _channelRoad(value),
    _lastSendDataTime(0),
    _lastReceivedDataTime(0)
{
    _psmExplainer.setSentSourceId(0);
    _psmExplainer.registerPsmExplainNotify(this);
    _psmExplainer.registerPsmExplainWrite(this);
    _psmExplainer.start();

    _remoteIsLocal = _channelRoad ? _channelRoad->remoteIpAddress().isLocalIpAddress() : false;
}

NtpTerminal::~NtpTerminal()
{
    _psmExplainer.registerPsmExplainNotify(NULL);
    _psmExplainer.registerPsmExplainWrite(NULL);
    _psmExplainer.stop();
}

void NtpTerminal::receivedData(const char *pData, int iLength)
{
    if (_psmExplainer.receivedData((const uchar *)pData, iLength) > 0)
    {
        _lastReceivedDataTime = CxTime::currentSystemTime();
    }
}

int NtpTerminal::send_data(const char *pData, int iLength, int iTargetId)
{
    if (_channelRoad)
    {
        return _channelRoad->sendData(pData, iLength);
    }
    else
    {
        cxWarning() << "NtpTerminal::send_data _channelRoad=NULL";
        return -1;
    }
}

string NtpTerminal::reportSelf()
{
    std::string sResult = CxString::format("PsmExplainer: {%s}, \n", _psmExplainer.reportSelf().c_str());
    if (_channelRoad)
        sResult += CxString::format("ChannelRoad: {%s}, \n", _channelRoad->reportSelf().c_str());
    else
        sResult += CxString::format("ChannelRoad: {%s}, \n", "nil");

    string sLastSendTime = CxTime::toString(_lastSendDataTime);
    string sLastReceivedTime = CxTime::toString(_lastReceivedDataTime);
    sResult += CxString::format("lastReceivedDataTime: %s, _lastSendDataTime: %s, remoteIsLocal: %d, ",
                                           sLastSendTime.c_str(), sLastReceivedTime.c_str(), _remoteIsLocal);

    return sResult;
}

void NtpTerminal::received_message_command(const PsmExplainer *oPsm, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach *oAttach)
{
    string sCommand(pCommand);
    string sParam(pParam, iParamLength);

    if (sCommand.find(".ntp.") != std::string::npos)
    {
        sParam += CxString::format("t2=%ld;t3=%ld;", CxTime::currentMsepoch(), CxTime::currentMsepoch());
        _psmExplainer.postMessageCommand("resp.ntp..", sParam.c_str(), sParam.size());
    }
}

void NtpTerminal::received_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
}

void NtpTerminal::received_realtime_data_request(const PsmExplainer *oPsm, const PsmAttach *oAttach)
{
}

void NtpTerminal::received_realtime_data_post(const PsmExplainer *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
}

void NtpTerminal::result_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const PsmAttach *oAttach)
{

}




























void NtpServer::init()
{
    f_oBusManager->start();
}

void NtpServer::uninit()
{
    f_oBusManager->stop();
}

NtpTerminal * NtpServer::findRoadTerminal(CxChannelRoad * oChannelRoad)
{
    map<CxChannelRoad *, NtpTerminal*>::iterator it = f_oRoadTerminals.find(oChannelRoad);
    if (it != f_oRoadTerminals.end())
    {
        return it->second;
    }
    return NULL;
}

NtpTerminal * NtpServer::allocateRoadTerminal(CxChannelRoad * oChannelRoad)
{
    NtpTerminal * oTerminal = findRoadTerminal(oChannelRoad);
    if (!oTerminal)
    {
        oTerminal = new NtpTerminal(oChannelRoad);
        f_oRoadTerminals[oChannelRoad] = oTerminal;
    }
    return oTerminal;
}

/***
 bus -update ssid=1;name=xxx;ttid=2

*/
int NtpServer::interinfoIn_line(const string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

int NtpServer::interinfoIn_cmd(const string &sCommand, const std::map<string, string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    if (sCommand == "ntp")
    {
        if (CxContainer::contain(sParams, std::string("report")))
        {
            reportAllToFile();
            return CxInterinfo::InDeal_Ed;
        }
        if (CxContainer::contain(sParams, std::string("exit")) && CxContainer::contain(sParams, std::string("test135246")))
        {
            CxApplication::exit();
            return CxInterinfo::InDeal_Ed;
        }
        return CxInterinfo::InDeal_Ed;
    }
    return 0;
}

std::vector<string> NtpServer::reportSelf()
{
    vector<string> sReports;
    sReports.push_back("*** NtpServer Report ***");
    sReports.push_back(CxString::format("NtpTerminal Count[%d] :", f_oRoadTerminals.size()));
    int iIndex = 0;
    for (map<CxChannelRoad *, NtpTerminal*>::iterator it = f_oRoadTerminals.begin(); it != f_oRoadTerminals.end(); ++it)
    {
        NtpTerminal * oTerminal = it->second;
        sReports.push_back(CxString::format("NtpTerminal %03d : ",iIndex) + oTerminal->reportSelf());
        iIndex++;
    }
    return sReports;
}

void NtpServer::reportAllToFile()
{
    string sReportFilePath = CxFileSystem::mergeFilePath( CxApplication::applicationDeployPath(), "report" );
    sReportFilePath = CxFileSystem::mergeFilePath( sReportFilePath, CxApplication::applicationTargetName() + ".report.txt" );
    vector<string> sReports;

    CxContainer::append(sReports, f_oBusManager->_udpServer->reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, f_oBusManager->_tcpServer->reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, CxChannelRoadManager::reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, NtpServer::reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");


    if (CxFile::save(sReportFilePath, sReports)>0)
    {
        cxPrompt() << "NtpServer report success , and save to file " << sReportFilePath;
    }
    else
    {
        cxPrompt() << "NtpServer report fail!!!" << sReportFilePath;
    }
}
















NtpServer::NtpServer()
{
    _udpServer = NULL;
    _tcpServer = NULL;
}

NtpServer::~NtpServer()
{
    stop();
}

void NtpServer::start()
{
    cxDebug() << "NtpServer load config ...";

    if (! _udpServer)
    {
        _udpServer = new CxChannelUdp();
        _udpServer->setIsServer(true);
        _udpServer->addObserver(f_oBusManager);
        _udpServer->setAutoOpenInterval(3000);
        _udpServer->setIsMultiRoad(true);
        _udpServer->setIsSendQueue(true);
    }
    if (! _tcpServer)
    {
        _tcpServer = new CxChannelTcpserver();
        _tcpServer->addObserver(f_oBusManager);
        _tcpServer->setAutoOpenInterval(3000);
        _tcpServer->setIsMultiRoad(true);
        _tcpServer->setIsSendQueue(true);
    }

    int iUdpPort = 6641;
    int iTcpPort = 6651;
    iUdpPort = CxApplication::findConfig(CS_SectionNtpServer, CS_EntryNtpUdpPort, iUdpPort);
    iTcpPort = CxApplication::findConfig(CS_SectionNtpServer, CS_EntryNtpTcpPort, iTcpPort);

    _udpServer->setLocalIp("127.0.0.1");
    _udpServer->setLocalPort(iUdpPort);
    _udpServer->open();

    _tcpServer->setLocalIp("127.0.0.1");
    _tcpServer->setLocalPort(iTcpPort);
    _tcpServer->open();

    int iCount = 0;
    while (! _udpServer->connected())
    {
        if (iCount > 3000)
            break;
        CxApplication::waiting(1);
        ++iCount;
    }

    iCount = 0;
    while (! _tcpServer->connected())
    {
        if (iCount > 3000)
            break;
        CxApplication::waiting(1);
        ++iCount;
    }

    reportAllToFile();
}

bool NtpServer::isStarted()
{
    return _udpServer && _tcpServer && _udpServer->connected() && _tcpServer->connected();
}

void NtpServer::stop()
{
    if (_udpServer)
    {
        _udpServer->setAutoOpenInterval(0);
        _udpServer->removeObserver(this);
        _udpServer->close();
        GM_DELETEANDNULL_OBJECT(_udpServer);
    }

    if (_tcpServer)
    {
        _tcpServer->setAutoOpenInterval(0);
        _tcpServer->removeObserver(this);
        _tcpServer->close();
        GM_DELETEANDNULL_OBJECT(_tcpServer);
    }

}

//*001 从通道接收数据，找相关对象来处理
void NtpServer::channel_receivedData(const uchar *pData, int iLength, void *oChannelRoad)
{
    assert(oChannelRoad);
    NtpTerminal * oTerminal = allocateRoadTerminal((CxChannelRoad*)oChannelRoad);
    assert(oTerminal!=NULL);
    oTerminal->receivedData((const char *)pData, iLength);
}
