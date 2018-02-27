#include "gcl_common_sdk.h"

#include <string>
 
using namespace std;

gci_fn_error_t fn_error = 0;
gci_fn_message_command_t fn_message_command = 0;
gci_fn_file_transfer_t fn_file_transfer = 0;
gci_fn_realtime_container_data_t fn_realtime_container_data = 0;

string f_error_msg;

//初始化
int GM_API_CALL gci_init(int argc, char * argv[])
{
    if (argc > 0)
        return argv[0][0];
    else
        return argc;
}

//清除
int GM_API_CALL gci_cleanup()
{
    return 999;
}


int GM_API_CALL gci_test(int iTest)
{
//    gci_fn_error_t fn_error = 0;
//    gci_fn_message_command_t fn_message_command = 0;
//    gci_fn_file_transfer_t fn_file_transfer = 0;
//    gci_fn_realtime_container_data_t fn_realtime_container_data = 0;
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
            string sContainerCode = "m700";
            string sCommand = "leadto_home";
            string sParam = "";
            int iTag = 22;
            fn_message_command(sCommand.c_str(), sParam.c_str(), sParam.size(), sContainerCode.c_str(), iTag);
            return 10002;
        }
        break;
    case 3:
        if (fn_file_transfer)
        {
            string sFilePath = "c:/0.txt";
            int iTag = 33;
            fn_file_transfer(sFilePath.c_str(), iTag);
            return 10003;
        }
        break;
    default:
        break;
    }
    return 102;
}


//登陆
//注意： char sOutFilePath[255]
int GM_API_CALL gci_connect(const char * sInFilePath, char * sOutFilePath)
{
    return 201;
}

//登陆注销 ( iConnectHandle == 0 表示默认 Connect Handle )
int GM_API_CALL gci_is_connected()
{
    return 202;
}

//登陆注销
int GM_API_CALL gci_disconnect()
{
    return 203;
}

long long GM_API_CALL gci_connected_active_time(int iTag)
{
    return 204;
}

int GM_API_CALL gci_set_default_containercode(const char * sContainerCode)
{
    return 205;
}


//*系统错误、程序错误
//注册回调

int GM_API_CALL gci_register_error_notify( gci_fn_error_t fn )
{
    fn_error = fn;
//    return (int)fn;
    return 103;
}

int GM_API_CALL gci_unregister_error_notify( gci_fn_error_t fn )
{
    fn_error = 0;
    return 104;
}

//获取最后错误
int GM_API_CALL gci_last_error_id()
{
    return 105;
}

int GM_API_CALL gci_last_error_info(const char * pInfo, int * iLength)
{
    return 106;
}

//获取错误信息
char * gci_error_msg(int iErrorId)
{
    return (char *)( f_error_msg.c_str() );
}






//*辅助





//*用户
///
/// \brief gci_user_login : 用户登录
/// \param sInCardId : 输入，IC卡号
/// \param sOutUserId : 输出，用户ID
/// \return 成功与否
/// \sample : char sInCardId[255] = "aaaaaaaaaaaa"; char sOutUserId[255];
int GM_API_CALL gci_user_login( const char * sInCardId, char * sOutUserId )
{
    return 301;
}

///
/// \brief gci_user_is_logined : 判断用户是否已经登录
/// \param sUserId : 输入，用户ID
/// \return 登录与否
///
int GM_API_CALL gci_user_is_logined( const char * sUserId )
{
    return 302;
}

///
/// \brief gci_user_logout : 用户登出
/// \param sUserId : 输入，用户ID
/// \return
///
int GM_API_CALL gci_user_logout( const char * sUserId )
{
    return 303;
}






//*命令 - 用消息形式收发
//注册回调
///
/// \brief gci_register_message_command_notify : 注册消息回调
/// \param fn : 回调指针
/// \sample :
/// void message_command( const char * sCommand, const char * sParam, int iParamLength ) { /* 收到消息，在这处理收到的消息 if ( sCommand == "leadto_huanyang" ) postmessge(hd, iLeadto_huanyang); */ }
/// gci_register_message_command_notify( message_command );
int GM_API_CALL gci_register_message_command_notify( gci_fn_message_command_t fn )
{
    fn_message_command = fn;
    return 401;
}

int GM_API_CALL gci_unregister_message_command_notify( gci_fn_message_command_t fn )
{
    fn_message_command = 0;
    return 402;
}

//主动发送
int GM_API_CALL gci_message_command_send( const char * sCommand, const char * sParam, int iParamLength, const char * sContainerCode, int iTag )
{
    return 403;
}

int GM_API_CALL gci_message_command_response( const char * sCommand, const char * sParam, int iParamLength, const char * sContainerCode, int iTag )
{
    return 404;
}






//*留 刷卡，数据作废确认





//*文件传送
//注册回调
int GM_API_CALL gci_register_file_transfer_notify( gci_fn_file_transfer_t fn )
{
    fn_file_transfer = fn;
    return 501;
}

int GM_API_CALL gci_unregister_file_transfer_notify( gci_fn_file_transfer_t fn )
{
    fn_file_transfer = 0;
    return 502;
}

//主动发送
int GM_API_CALL gci_file_transfer_send( const char * sFilePath, int iTag )
{
    return 503;
}

//被动、回应、响应发送
int GM_API_CALL gci_file_transfer_response( const char * sFilePath, int iTag )
{
    return 504;
}







//注册回调函数，例如：对方发送 read、write、lock……等 方法过来时会触发此回调
int GM_API_CALL gci_register_realtime_container_data_notify( gci_fn_realtime_container_data_t fn )
{
    return 601;
}

int GM_API_CALL gci_unregister_realtime_container_data_notify( gci_fn_realtime_container_data_t fn )
{
    return 602;
}

//*主动发送 遥信 部分
int GM_API_CALL gci_yx_address_vqt_text1_send( const gct_address_vqt_int_text1_t * pData, int iCount, const char * sContainerCode )
{
    return 603;
}

//(PE)
int GM_API_CALL gci_yx_firstaddress_vqt_text1_send( gct_firstaddress_t firstAddress, const gct_vqt_int_text1_t * pData, int iCount, const char * sContainerCode )
{
    return 604;
}

//响应发送：由于收到对方的主动读、写、锁、解锁
//*被动动发送 遥信 部分
int GM_API_CALL gci_yx_address_vqt_text1_response( const gct_address_vqt_int_text1_t * pData, int iCount, const char * sContainerCode )
{
    return 607;
}

int GM_API_CALL gci_yx_firstaddress_vqt_text1_response( gct_firstaddress_t firstAddress, const gct_vqt_int_text1_t * pData, int iCount, const char * sContainerCode )
{
    return 608;
}

//*主动发送 遥测 部分
int GM_API_CALL gci_yc_address_vqt_text15_send( const gct_address_vqt_real_text15_t * pData, int iCount, const char * sContainerCode )
{
    return 605;
}

//(PE)
int GM_API_CALL gci_yc_firstaddress_vqt_text15_send( gct_firstaddress_t firstAddress, const gct_vqt_real_text15_t * pData, int iCount, const char * sContainerCode )
{
    return 606;
}

//*被动发送 遥测 部分
int GM_API_CALL gci_yc_address_vqt_text15_response( const gct_address_vqt_real_text15_t * pData, int iCount, const char * sContainerCode )
{
    return 609;
}

int GM_API_CALL gci_yc_firstaddress_vqt_text15_response( gct_firstaddress_t firstAddress, const gct_vqt_real_text15_t * pData, int iCount, const char * sContainerCode )
{
    return 610;
}

