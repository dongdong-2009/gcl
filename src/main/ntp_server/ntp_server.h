
#ifndef BUS_TERMINAL_H
#define BUS_TERMINAL_H


#include <ccxx/ccxx.h>
#include <psm/psm_explain.h>


//连接上BUS的客户端
class NtpTerminal : public IPsmExplainNotify, public CxIChannelSend
{
public:
    NtpTerminal(CxChannelRoad * value);

//    BusTerminal(int iTerminalId);

    ~NtpTerminal();

    void receivedData(const char *pData, int iLength);

    PsmExplainer * psmExplainter() {return &_psmExplainer;}

    int send_data(const char* pData, int iLength, int iTargetId);

    inline bool remoteIsLocal() { return _remoteIsLocal; }

    std::string reportSelf();

protected:
    int received_packet(const PsmExplainer * oPsm, int oTag = 0) { return 0; }

    void received_message_command(const PsmExplainer * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL);

    void received_file_write(const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL);

    void received_realtime_data_request(const PsmExplainer * oPsm, const PsmAttach * oAttach = NULL);

    void received_realtime_data_post(const PsmExplainer * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

    void result_file_write(const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const PsmAttach * oAttach = NULL);

private:
    PsmExplainer _psmExplainer;
    CxChannelRoad * _channelRoad;
    bool _remoteIsLocal;
    msepoch_t _lastSendDataTime;
    msepoch_t _lastReceivedDataTime;

    friend class NtpServer;

};

//*管理两容器
//map<CxChannelRoad *, BusTerminal*> f_oRoadTerminals;
//map<int, BusTerminal*> f_sourceTerminals;
//*管理两 ChannelServer，1 个 GA
//CxChannelUdp * _udpServer;
//CxChannelTcpserver * _tcpServer;
class NtpServer : public CxIChannelSubject
{
public:
    static void init();

    static void uninit();

    //在 f_oRoadTerminals 中找
    static NtpTerminal * findRoadTerminal(CxChannelRoad * oChannelRoad);

    //在 f_oRoadTerminals 中分配
    static NtpTerminal * allocateRoadTerminal(CxChannelRoad * oChannelRoad);

    //CxInterinfo在命令行输入、IO的输入
    static int interinfoIn_line( const std::string & sInfo, const msepoch_t & dtIn, int iSource, int eInType, int iTag);

    //CxInterinfo在命令行输入、IO的输入
    static int interinfoIn_cmd( const std::string & sCommand, const std::map<std::string, std::string> & sParams, const msepoch_t & dtIn, int iSource, int eInType, int iTag);

    static std::vector<std::string> reportSelf();

    static void reportAllToFile();

public:
    NtpServer();
    ~NtpServer();

    void start();

    bool isStarted();

    void stop();

private:
    void channel_receivedData(const uchar* pData, int iLength, void * oChannelRoad);

private:
    CxChannelUdp * _udpServer;
    CxChannelTcpserver * _tcpServer;
    CxChannelBase * _gaChannel;

};

#endif // BUS_TERMINAL_H
