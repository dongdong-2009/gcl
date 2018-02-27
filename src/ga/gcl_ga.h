#ifndef GCL_GA_H
#define GCL_GA_H

#include <gcl_common_sdk.h>
#include <psm/psm_explain.h>

class GclGa
{
public:
    static int init(int argc, const char * argv[]);

    static void cleanup();

    static int status();

    static bool connected();

    static int onlineMillisecond();

    static int lastReceivedProtocolDataTime();

    static char * statusInfos(int iUserId = 0);

    static int postMessageCommand( const char * sCommand, const char * sParam = 0, int iParamLength = 0, const PsmAttach * oAttach = NULL);

    static int postFile(const std::string & sFilePath, const PsmAttach * oAttach = NULL);

    static int postRealtimeDataRequest( const PsmAttach * oAttach = NULL );

    static int postRealtimeDataPost( int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

    static int setReceiveErrorNotify(gci_fn_error_t fn);

    static int setReceiveMessageNotify(gci_fn_message_command_t fn);

    static int setReceiveFileNotify(gci_fn_file_transfer_t fn);

    static int setReceiveRealtimeDataRequestNotify(gci_fn_realtime_data_request_t fn);

    static int setReceiveRealtimeDataPostNotify(gci_fn_realtime_data_post_t fn);

    static int setResultFileNotify(gci_fn_file_transfer_t fn);

    static int received_message_command( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );

    static int received_file_write( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach );

    static int received_realtime_data_request( const PsmExplainer * oPsm, const PsmAttach * oAttach );

    static int received_realtime_data_post( const PsmExplainer * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach );

    static int result_file_write( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const PsmAttach * oAttach );

    //*实时数据读写（以数组，连续，单个的方式）
    static int realtimeDataGetArray( int iBeginMid, int iMidCount, int iStructType, char * pOutResult, int iOutResultLength, const PsmAttach * oAttach = NULL );

    static int realtimeDataGetList( int iStructType, char * pOutResult, int iOutResultLength, int iStructCount, const PsmAttach * oAttach = NULL );

    static int realtimeDataGet( int iMid, int iStructType, char * pOutResult, int iOutResultLength, const PsmAttach * oAttach = NULL );

    static int realtimeDataSetList( int iStructType, char * pStructData, int iStructDataLength, int iStructCount, const PsmAttach * oAttach = NULL );

    static int realtimeDataSet( int iStructType, char * pStructData, int iStructDataLength, const PsmAttach * oAttach = NULL );

};

#endif // GCL_GA_H
