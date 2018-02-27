#ifndef GENERALACCESS_H
#define GENERALACCESS_H

#include "psm_explain.h"

const std::string CS_SectionGeneralAccess   = "GeneralAccess";

class GeneralAccess : public CxIChannelSubject, public CxIChannelSend, public IPsmExplainNotify
{
public:
    //1、加载配置，如果配有 Channel 就加载，并且加载 PsmExplainer
    static void start();

    static void stop();

    static bool isConnected();

    static const PsmAttach * defaultAttach();

    static const CxChannelBase * getChannel();

    static msepoch_t lastReceivedProtocolDataTime();

    static std::vector<std::string> reportSelf();

    static void setRemoteIpAddress(const std::string& sIp, ushort iPort);

    static int interinfoIn_line( const std::string & sInfo, const msepoch_t & dtIn, int iSource, int eInType, int iTag );

    static int interinfoIn_cmd( const std::string & sCommand, const std::map<std::string, std::string> & sParams, const msepoch_t & dtIn, int iSource, int eInType, int iTag );

    //对外发送消息
    inline static int postMessage( const std::string &sCommand, const std::string &sParam, const PsmAttach * oAttach = NULL ) { return postMessage(sCommand.c_str(), sParam.data(), sParam.length(), oAttach); }

    //对外发送消息
    static int postMessage( const char * sCommand, const char * sParam = 0, int iParamLength = 0, const PsmAttach * oAttach = NULL );

    //对外发送文件
    static int postFile( const std::string & sFilePath, const PsmAttach * oAttach = NULL );

    //对外发送文件
    static int postFile(const CxFileSystem::PathInfo &pathInfo, const std::vector<std::string> &fileData, const PsmAttach * oAttach = NULL );

    //对外发送实时数据请求
    static int postRealtimeDataRequest( const PsmAttach * oAttach = NULL );

    //对外发送实时数据
    static int postRealtimeDataPost( int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL );

    //对外发送实时数据
    //yx
    static int postRealtimeDatas( const gcd_address_int32_vqt_t * oYxs , int iYxCount , const PsmAttach * oAttach = NULL );
    static int postRealtimeDatas( const std::vector<gcd_address_int32_vqt_t> & yxs , const PsmAttach * oAttach = NULL );
    static int postRealtimeData( const gcd_address_int32_vqt_t & yx , const PsmAttach * oAttach = NULL );
    //yc
    static int postRealtimeDatas( const gcd_address_double_vqt_t * oYcs , int iYcCount , const PsmAttach * oAttach = NULL );
    static int postRealtimeDatas( const std::vector<gcd_address_double_vqt_t> & ycs , const PsmAttach * oAttach = NULL );
    static int postRealtimeData( const gcd_address_double_vqt_t & yc , const PsmAttach * oAttach = NULL );
    //yw
    static int postRealtimeDatas(const gcd_address_vqt_string128_t * oYws , int iYwCount , const PsmAttach * oAttach = NULL );
    static int postRealtimeDatas( const std::vector<gcd_address_vqt_string128_t> & yws , const PsmAttach * oAttach = NULL );
    static int postRealtimeData( const gcd_address_vqt_string128_t & yw , const PsmAttach * oAttach = NULL );

    static void timerTimeoutHeartJump(int iInterval);

    static int heartJumpInterval();

    static bool isHeartJumpOnline();

    //注册消息处理的回调
    static void registerMessageCommandDeal(fn_psm_explain_received_message_command_t fn, int iIndex=-1);

    static void unregisterMessageCommandDeal(fn_psm_explain_received_message_command_t fn);

    static void registerFileWriteDeal(fn_psm_explain_received_file_write_t fn, int iIndex=-1);

    static void unregisterFileWriteDeal(fn_psm_explain_received_file_write_t fn);

    static void registerRealtimeDataRequestDeal(fn_psm_explain_received_realtime_data_request_t fn, int iIndex=-1);

    static void unregisterRealtimeDataRequestDeal(fn_psm_explain_received_realtime_data_request_t fn);

    static void registerRealtimeDataPostDeal(fn_psm_explain_received_realtime_data_post_t fn, int iIndex=-1);

    static void unregisterRealtimeDataPostDeal(fn_psm_explain_received_realtime_data_post_t fn);

    static void registerResultFileWriteDeal(fn_psm_explain_result_file_write_t fn, int iIndex=-1);

    static void unregisterResultFileWriteDeal(fn_psm_explain_result_file_write_t fn);

    static void raiseReceivedFile( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL );

private:
    static std::vector<fn_psm_explain_received_message_command_t> * getFnMessageCommandDeals();

    static std::vector<fn_psm_explain_received_file_write_t> * getFnFileWriteDeals();

    static std::vector<fn_psm_explain_received_realtime_data_request_t> * getFnRealtimeDataRequestDeals();

    static std::vector<fn_psm_explain_received_realtime_data_post_t> * getFnRealtimeDataPostDeals();

    static std::vector<fn_psm_explain_result_file_write_t> * getFnResultFileWriteDeals();

protected:
    int send_data( const char* pData, int iLength, int iTargetId );

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

#endif // GENERALACCESS_H
