#include "ga_api.h"

#include "gcl_ga.h"

#define GCI_SUCCESS (1)
#define GCI_NONE (0)
#define GCI_FALSE (-1)

using namespace std;


static gci_fn_error_t fn_error = 0;
static gci_fn_message_command_t fn_message_command = 0;
static gci_fn_file_transfer_t fn_file_transfer = 0;
static gci_fn_realtime_data_request_t fn_realtime_data_request = 0;
static gci_fn_realtime_data_post_t fn_realtime_data_Post = 0;

//初始化
int GCI_API_CALL gci_init(int argc, char * argv[])
{
    if (GclGa::init(argc, (const char **)argv))
    {
        return GCI_SUCCESS;
    }
    else
    {
        return GCI_FALSE;
    }
}

//清除
int GCI_API_CALL gci_cleanup()
{
    GclGa::cleanup();
    return TRUE;
}

int GCI_API_CALL gci_init_status()
{
    return GclGa::status();
}

int GCI_API_CALL gci_terminal_status(int iId)
{
    return GclGa::connected();
}

int GCI_API_CALL gci_terminal_online_millisecond(int iId)
{
    return GclGa::lastReceivedProtocolDataTime();
}

char * GCI_API_CALL gci_terminal_info(int iId)
{
    return GclGa::statusInfos();
}

int GCI_API_CALL gci_test(int iTest)
{
    switch (iTest)
    {
    case 1:
        if (fn_error)
        {
            fn_error(10001);
            return 10001;
        }
        break;
    case 2:
        if (fn_message_command)
        {
            std::string sContainerCode = "m700";
            std::string sCommand = "leadto_home";
            std::string sParam = "";
            int iReason = 0;
            int iContainerId = 0;
            int iSourceId = 0;
            int iTargetId = 0;
            int iTag = 22;
            PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceId , iTargetId, iTag);
            fn_message_command(sCommand.c_str(), sParam.c_str(), sParam.size(), &attach);
            return 10002;
        }
        break;
    case 3:
        if (fn_file_transfer)
        {
            std::string sFilePath = "c:/0.txt";
            int iReason = 0;
            int iContainerId = 0;
            int iSourceId = 0;
            int iTargetId = 0;
            int iTag = 33;
            PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceId , iTargetId, iTag);
            fn_file_transfer(sFilePath.c_str(), &attach);
            return 10003;
        }
        break;
    case 4:
        if (fn_realtime_data_request)
        {
            int iReason = 0;
            int iContainerId = 0;
            int iSourceId = 0;
            int iTargetId = 0;
            int iTag = 33;
            PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceId , iTargetId, iTag);
            fn_realtime_data_request(&attach);
            return 10004;
        }
        break;
    case 5:
        if (fn_realtime_data_Post)
        {
            gcd_address_vqt_intText1_t yxs[10];
            for (size_t i = 0; i < sizeof(yxs) / sizeof(yxs[0]); ++i)
            {
                gcd_address_vqt_intText1_t & yx = yxs[i];
                yx.address = i;
                yx.datetime = CxTime::currentSystemTime();
                yx.quality = 1;
                yx.value[0] = (i % 2) + 0x30;
                yx.value[1] = 0;
            }
            int iReason = 0;
            int iContainerId = 0;
            int iSourceId = 0;
            int iTargetId = 0;
            int iTag = 33;
            PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceId , iTargetId, iTag);
            fn_realtime_data_Post(gcd_address_vqt_intText1_t::getType(), (char*)yxs, sizeof(yxs), sizeof(yxs) / sizeof(yxs[0]), &attach);
            return 10004;
        }
        break;
    default:
        break;
    }
    return 102;
}


//*系统错误、程序错误
//注册回调

int GCI_API_CALL gci_register_error_notify( gci_fn_error_t fn )
{
    fn_error = fn;
    return GclGa::setReceiveErrorNotify(fn);
}

int GCI_API_CALL gci_unregister_error_notify( gci_fn_error_t fn )
{
    fn_error = 0;
    return 104;
}

//获取最后错误
int GCI_API_CALL gci_last_error_id()
{
    return errno;
}

//获取错误信息
char * gci_error_info(int iErrorId)
{
    return strerror(iErrorId);
}




//*命令 - 用消息形式收发
//注册回调
///
/// \brief gci_register_message_command_notify : 注册消息回调
/// \param fn : 回调指针
/// \sample :
/// void message_command( const char * sCommand, const char * sParam, int iParamLength ) { /* 收到消息，在这处理收到的消息 if ( sCommand == "leadto_huanyang" ) postmessge(hd, iLeadto_huanyang); */ }
/// gci_register_message_command_notify( message_command );
int GCI_API_CALL gci_register_message_command_notify( gci_fn_message_command_t fn )
{
    fn_message_command = fn;
    return GclGa::setReceiveMessageNotify(fn);
}

int GCI_API_CALL gci_unregister_message_command_notify( gci_fn_message_command_t fn )
{
    fn_message_command = 0;
    return GclGa::setReceiveMessageNotify(gci_fn_message_command_t(0));
}

//主动发送
int GCI_API_CALL gci_message_command_send( const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach )
{
    return GclGa::postMessageCommand(sCommand, sParam, iParamLength, oAttach);
}



//*文件传送
//注册回调
int GCI_API_CALL gci_register_file_transfer_notify( gci_fn_file_transfer_t fn )
{
    return GclGa::setReceiveFileNotify(fn);
}

int GCI_API_CALL gci_unregister_file_transfer_notify( gci_fn_file_transfer_t fn )
{
    return GclGa::setReceiveFileNotify(NULL);
}

int GCI_API_CALL gci_register_file_transfer_result_notify( gci_fn_file_transfer_t fn )
{
    return GclGa::setResultFileNotify(fn);
}

int GCI_API_CALL gci_unregister_file_transfer_result_notify( gci_fn_file_transfer_t fn )
{
    return GclGa::setResultFileNotify(NULL);
}

//主动发送
int GCI_API_CALL gci_file_transfer_send( const char * sFilePath, const PsmAttach * oAttach )
{
    return GclGa::postFile(sFilePath, oAttach);
}





//注册回调函数，例如：对方发送 request、Post、lock……等 方法过来时会触发此回调
int GCI_API_CALL gci_register_realtime_data_request_notify( gci_fn_realtime_data_request_t fn )
{
    return GclGa::setReceiveRealtimeDataRequestNotify(fn);
}

int GCI_API_CALL gci_unregister_realtime_data_request_notify( gci_fn_realtime_data_request_t fn )
{
    return GclGa::setReceiveRealtimeDataRequestNotify(NULL);
}

int GCI_API_CALL gci_register_realtime_data_post_notify( gci_fn_realtime_data_post_t fn )
{
    return GclGa::setReceiveRealtimeDataPostNotify(fn);
}

int GCI_API_CALL gci_unregister_realtime_data_post_notify( gci_fn_realtime_data_post_t fn )
{
    return GclGa::setReceiveRealtimeDataPostNotify(NULL);
}

int GCI_API_CALL gci_realtime_data_request( const PsmAttach * oAttach )
{
    return GclGa::postRealtimeDataRequest(oAttach);
}

int GCI_API_CALL gci_realtime_data_post( int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach )
{
    return GclGa::postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, oAttach);
}








//*实时数据读（以数组，连续，单个的方式）
int GCI_API_CALL gci_realtime_data_get_array( int iBeginMid, int iMidCount, int iStructType, char * pOutResult, int iOutResultLength, const PsmAttach * oAttach )
{
    return GclGa::realtimeDataGetArray(iBeginMid, iMidCount, iStructType, pOutResult, iOutResultLength, oAttach);
}

int GCI_API_CALL gci_realtime_data_get_list( int iStructType, char * pOutResult, int iOutResultLength, int iStructCount, const PsmAttach * oAttach )
{
    return GclGa::realtimeDataGetList(iStructType, pOutResult, iOutResultLength, iStructCount, oAttach);
}

int GCI_API_CALL gci_realtime_data_get( int iMid, int iStructType, char * pOutResult, int iOutResultLength, const PsmAttach * oAttach )
{
    return GclGa::realtimeDataGet(iMid, iStructType, pOutResult, iOutResultLength, oAttach);
}


//*实时数据写（以数组，连续，单个的方式）
int GCI_API_CALL gci_realtime_data_set_list( int iStructType, char * pStructData, int iStructDataLength, int iStructCount, const PsmAttach * oAttach )
{
    return GclGa::realtimeDataSetList(iStructType, pStructData, iStructDataLength, iStructCount, oAttach);
}

int GCI_API_CALL gci_realtime_data_set( int iStructType, char * pStructData, int iStructDataLength, const PsmAttach * oAttach )
{
    return GclGa::realtimeDataSet(iStructType, pStructData, iStructDataLength, oAttach);
}
