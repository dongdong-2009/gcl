
#ifndef BUS_TERMINAL_H
#define BUS_TERMINAL_H


#include <ccxx/ccxx.h>
#include <psm/psm_explain.h>
#include <vdi/update_template.h>
#include <vdi/fileinfo_sharememory.h>
#include "bus_strings.h"


//连接上BUS的客户端
class BusTerminal : public IPsmExplainNotify, public CxIChannelSend
{
public:
    BusTerminal(CxChannelRoad * value);

//    BusTerminal(int iTerminalId);

    ~BusTerminal();

    void setTerminalId(int iSourceId, int iOriginalId);

    void receivedData(const char *pData, int iLength);

    inline int terminalId() const { return _sourceTerminalId; }

    inline const PsmSendPacket & sendPacket() const { return _sendPacket; }

    inline int sendCacheSize() { return _sendCacheSize; }
    inline void setSendCacheSize(int value) { _sendCacheSize = value; }

    int pushPacketData(const psm_packet_body_t & body, const char * pData, int iDataLength);

    void appendSendPacket(BusTerminal *oPsm);

    PsmExplainer * psmExplainter() {return &_psmExplainer;}

    int send_data(const char* pData, int iLength, int iTargetId);

    inline bool remoteIsLocal() { return _remoteIsLocal; }

    std::string reportSelf();

protected:
    int received_packet(const PsmExplainer * oPsm, int oTag = 0);

    void received_message_command(const PsmExplainer * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL);

    void received_file_write(const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL);

    void received_realtime_data_request(const PsmExplainer * oPsm, const PsmAttach * oAttach = NULL);

    void received_realtime_data_post(const PsmExplainer * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

    void result_file_write(const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const PsmAttach * oAttach = NULL);

private:
    int sendNextPacket();

private:
    PsmExplainer _psmExplainer;
    PsmSendPacket _sendPacket;
    size_t _sendCacheSize;

    CxChannelRoad * _channelRoad;
    bool _remoteIsLocal;

    msepoch_t _lastSendDataTime;
    msepoch_t _lastReceivedDataTime;
    int _sourceTerminalId;
    int _originalTerminalId;

    friend class BusManager;

};

//*管理两容器
//map<CxChannelRoad *, BusTerminal*> f_oRoadTerminals;
//map<int, BusTerminal*> f_sourceTerminals;
//*管理两 ChannelServer，1 个 GA
//CxChannelUdp * _udpServer;
//CxChannelTcpserver * _tcpServer;
class BusManager : public CxIChannelSubject
{
public:
    static void init();

    static void uninit();

    //在 f_oRoadTerminals 中找
    static BusTerminal * findRoadTerminal(CxChannelRoad * oChannelRoad);

    //在 f_oRoadTerminals 中找
    static BusTerminal * findRoadTerminal(int iSourceId);

    //在 f_oRoadTerminals 中分配
    static BusTerminal * allocateRoadTerminal(CxChannelRoad * oChannelRoad);

    //在 f_oRoadTerminals 中删除
    static void deleteRoadTerminal(CxChannelRoad * oChannelRoad);

    //在 f_sourceTerminals 中找
    static BusTerminal * findSourceTerminal(int iSourceId);

    //在 f_sourceTerminals 中分配
    static BusTerminal * findTargetTerminal(int iTargetTermainlId, int iT2t);

    //在 f_oRoadTerminals 中删除
    static void deleteSourceTerminal(int iSourceId);

    //转发数据 - 消息
    static int transferMessage(int iSourceTerminalId, const char *pCommand, const char *pParam, int iParamLength, int iTargetTerminalId);

    //转发数据 - 文件
    static int transferFileStream(int iSourceTerminalId, const CxFileSystem::PathInfo &pathInfo, const std::vector<std::string> &fileData, int iTargetTerminalId);

    //转发数据 - 实时数据
    static int transferRealtimeDataPost(int iSourceTerminalId,  int iParamType, const char * pData, int iDataLength, int iParamCount, int iTargetTerminalId);

    //转发数据 - 实时数据
    static int transferPacketDataPost(const psm_packet_body_t & body, const char * pData, int iDataLength, int iTargetTerminalId);

    //CxInterinfo在命令行输入、IO的输入
    static int interinfoIn_line( const std::string & sInfo, const msepoch_t & dtIn, int iSource, int eInType, int iTag);

    //CxInterinfo在命令行输入、IO的输入
    static int interinfoIn_cmd( const std::string & sCommand, const std::map<std::string, std::string> & sParams, const msepoch_t & dtIn, int iSource, int eInType, int iTag);

//sFilePath is xml style :
/*
<?xml version="1.0" encoding="utf-8"?>
<xxx>
    <bus>
        <port>
            <a>
                <prop ssid="0" port=5562/>
            </a>
        </port>
    </bus>
</xxx>
*/
    static const std::string & getTableName() {
        return CS_TableName_Bus;
    }

    static void update(const std::map<std::string, std::string> & row);

    static void update( int ssid, int iPort );

    static void updateT2t( int ttid1, int ttid2 );

    static std::vector<std::string> reportSelf();

    static void reportAllToFile();

public:
    BusManager();
    ~BusManager();

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
