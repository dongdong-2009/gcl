#ifndef BUS_GENERAL_ACCESS_H
#define BUS_GENERAL_ACCESS_H


#include <psm/psm_explain.h>


const std::string CS_SectionGeneralAccess   = "GeneralAccess";

class BusGeneralAccess : public CxIChannelSubject, public CxIChannelSend, public IPsmExplainNotify
{
public:
    //1、加载配置，如果配有 Channel 就加载，并且加载 PsmExplainer
    static void start( );

    static void stop( );

    static bool isConnected( );

    static int getSourceId();

    static const PsmAttach * defaultAttach();

    //1、加载配置，如果配有 Channel 就加载，并且加载 PsmExplainer
    static void init( const std::string & sDeaultChannelTypeSimpleName );

    static int interinfoIn_line( const std::string & sInfo, const msepoch_t & dtIn, int iSource, int eInType, int iTag );

    static int interinfoIn_cmd( const std::string & sCommand, const std::map<std::string, std::string> & sParams, const msepoch_t & dtIn, int iSource, int eInType, int iTag );

    //对外发送消息
    inline static int postMessage( const std::string &sCommand, const std::string &sParam, const PsmAttach * oAttach = NULL ) { return postMessage(sCommand.c_str(), sParam.data(), sParam.length(), oAttach); }

    //对外发送消息
    static int postMessage( const char * sCommand, const char * sParam = 0, int iParamLength = 0, const PsmAttach * oAttach = NULL );

    //对外发送文件
    static int postFile( const std::string & sFilePath, const PsmAttach * oAttach = NULL );

    //对外发送文件
    static int postFile(const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL );

    //对外发送实时数据请求
    static int postRealtimeDataRequest( const PsmAttach * oAttach = NULL );

    //对外发送实时数据
    static int postRealtimeDataPost( int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL );

    //对外发送包数据
    static int postPacketData(const psm_packet_body_t & body, const char * pData, int iDataLength);

    static void timerTimeoutHeartJump(int iInterval);

public:
    int send_data( const char* pData, int iLength, int iTargetId );

protected:
    inline int received_packet( const PsmExplainer * oPsm, int oTag = 0) { return 0; }

    void received_message_command( const PsmExplainer * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL );

    void received_file_write( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL );

    void received_realtime_data_request( const PsmExplainer * oPsm, const PsmAttach * oAttach = NULL );

    void received_realtime_data_post( const PsmExplainer * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL );

    void result_file_write(const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const PsmAttach * oAttach = NULL);

protected:
    bool channel_canChangeConnect( const CxChannelBase* oChannel, bool bOldConnect, bool bNewConnect) { return true; }

    void channel_connectChanged( const CxChannelBase* oChannel) {}

    void channel_beforeDelete( const CxChannelBase* oChannel) {}

    void channel_receivedData( const uchar* pData, int iLength, void * oSource );

};


#endif // BUS_GENERAL_ACCESS_H
