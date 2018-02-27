#ifndef PSM_MANAGER_H
#define PSM_MANAGER_H


#include "psm_protocol.h"

class GM_GCL_VDI_API PsmManager : public IPsmProtocolNotify
{
public:
    static PsmProtocol * getPsmProtocol(int iLocalTerminalId);

    //*post messager
    static int postMessageByTerminalId(int iLocalTerminalId, int iRemoteTerminalId, const std::string &sCommand, const std::string &sParam);

    static int postMessage(const std::string &sCommand, const std::string &sParam, const PsmAttach * oAttach = NULL);


    //*post file
    static int postFileByTerminalId(int iLocalTerminalId,int iRemoteTerminalId, const std::string & sFilePath);

    static int postFile(const std::string & sFilePath, const PsmAttach * oAttach = NULL);


    //*post realtime data
    static int postRealtimeDataRequestByTerminalId(int iLocalTerminalId,int iRemoteTerminalId);

    static int postRealtimeDataRequest( const PsmAttach * oAttach = NULL );


    static int postRealtimeDataPostByTerminalId(int iLocalTerminalId, int iRemoteTerminalId, int iParamType, const char * pData, int iDataLength, int iParamCount = 1);

    static int postRealtimeDataPost( int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);


    //cmd input
    static int interinfoIn_line( const std::string & sInfo, const msepoch_t & dtIn, int iSource, int eInType, int iTag);

    static int interinfoIn_cmd( const std::string & sCommand, const std::map<std::string, std::string> & sParams, const msepoch_t & dtIn, int iSource, int eInType, int iTag);

    //
    static void timeOutTest(int iInterval);

    //psm register
    static void registerPsmProtocolNotify();

    static void addObserver(IPsmProtocolNotify * oNotify);

    static void removeObserver(IPsmProtocolNotify * oNotify);


    //登录，实时在线等待
    static int userLoginServer(int iLocalTerminalId, int iRemoteTerminalId, const std::string &sUserName, const std::string &sPassword, int iWaittime = 3000);

    static int userLoginServer(int iLocalTerminalId, int iRemoteTerminalId, const std::string &sIccardCode, int iWaittime = 3000);

    //本地登录到远端后，远端授权的 UserId
    static int localTerminalUserId(int iLocalTerminalId);

    //远端登录上来的 UserId
    static int remoteTerminalUserId(int iRemoteTerminalId);


protected:
    void received_message_command(const PsmProtocol * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL);

    void received_file_write(const PsmProtocol * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL);

    void received_realtime_data_request(const PsmProtocol * oPsm, const PsmAttach * oAttach = NULL);

    void received_realtime_data_post(const PsmProtocol * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

};

#endif // PSM_MANAGER_H
