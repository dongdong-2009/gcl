#ifndef GCL_COMMON_SDK_H
#define GCL_COMMON_SDK_H


//true : int>0
//false : int<0
//nothind : int==0
//const char * s ： 默认是C字符串风格，以'\0'(0x00)为结束符
//FilePath : 表示全路径
//sContainerCode ：仪器编号


//注意：
//回调函数默认情况是在子线程中运行，如果有全局处理的，请用异步的方式（像 postmessage)


#ifndef GM_API_IMPL
#define GM_API_IMPL __declspec(dllexport)
//#define GM_API_IMPL __declspec(dllimport)
//#define GM_API_IMPL
#endif
#define GM_API_CALL __stdcall
//#define GM_API_CALL __cdecl

#ifdef __cplusplus
extern "C" {
#endif

//LS6828D





//初始化
GM_API_IMPL int GM_API_CALL gci_init(int argc, char * argv[]);
//清除
GM_API_IMPL int GM_API_CALL gci_cleanup();
//测试
GM_API_IMPL int GM_API_CALL gci_test(int iTest);




//*系统错误、程序错误
typedef void ( GM_API_CALL * gci_fn_error_t)( int iErrorid );
//注册回调
GM_API_IMPL int GM_API_CALL gci_register_error_notify( gci_fn_error_t fn );
GM_API_IMPL int GM_API_CALL gci_unregister_error_notify( gci_fn_error_t fn );
//获取最后错误
GM_API_IMPL int GM_API_CALL gci_last_error_id();
GM_API_IMPL int GM_API_CALL gci_last_error_info(const char * pInfo, int * iLength);
//获取错误信息
char * gci_error_msg(int iErrorId = 0);





//连接ICS
//注意： char sOutFilePath[255]
/// sOutFilePath : userInfo.ini
GM_API_IMPL int GM_API_CALL gci_connect(const char * sInFilePath, char * sOutFilePath);
//判断连接状态
///
/// \brief gci_is_connected : 判断连接状态
/// \return 连接与断开
GM_API_IMPL int GM_API_CALL gci_is_connected();
//断开ICS
GM_API_IMPL int GM_API_CALL gci_disconnect();
//当前活动时间
GM_API_IMPL long long GM_API_CALL gci_connected_active_time(int iTag = 0);



//设置默认仪器编号
GM_API_IMPL int GM_API_CALL gci_set_default_containercode(const char * sContainerCode);





//*用户
///
/// \brief gci_user_login : 用户登录
/// \param sInCardId : 输入，IC卡号
/// \param sOutUserId : 输出，用户ID
/// \return 成功与否
/// \sample : char sInCardId[255] = "aaaaaaaaaaaa"; char sOutUserId[255];
GM_API_IMPL int GM_API_CALL gci_user_login( const char * sInCardId, char * sOutUserId );
///
/// \brief gci_user_is_logined : 判断用户是否已经登录
/// \param sUserId : 输入，用户ID
/// \return 登录与否
///
GM_API_IMPL int GM_API_CALL gci_user_is_logined( const char * sUserId );
///
/// \brief gci_user_logout : 用户登出
/// \param sUserId : 输入，用户ID
/// \return
///
GM_API_IMPL int GM_API_CALL gci_user_logout( const char * sUserId );





//*命令 - 用消息形式收发
typedef void ( GM_API_CALL * gci_fn_message_command_t)( const char * sCommand, const char * sParam, int iParamLength, const char * sContainerCode, int iTag );
//注册回调
///
/// \brief gci_register_message_command_notify : 注册消息回调
/// \param fn : 回调指针
/// \sample :
/// void message_command( const char * sCommand, const char * sParam, int iParamLength ) { /* 收到消息，在这处理收到的消息 if ( sCommand == "leadto_huanyang" ) postmessge(hd, iLeadto_huanyang); */ }
/// gci_register_message_command_notify( message_command );
GM_API_IMPL int GM_API_CALL gci_register_message_command_notify( gci_fn_message_command_t fn );
GM_API_IMPL int GM_API_CALL gci_unregister_message_command_notify( gci_fn_message_command_t fn );
//主动发送
GM_API_IMPL int GM_API_CALL gci_message_command_send( const char * sCommand, const char * sParam = 0, int iParamLength = 0, const char * sContainerCode = 0, int iTag = 0 );
GM_API_IMPL int GM_API_CALL gci_message_command_response( const char * sCommand, const char * sParam = 0, int iParamLength = 0, const char * sContainerCode = 0, int iTag = 0 );





//*留 刷卡，数据作废确认





//*文件传送
typedef void ( GM_API_CALL * gci_fn_file_transfer_t)( const char * sFilePath, int iTag );
//注册回调
GM_API_IMPL int GM_API_CALL gci_register_file_transfer_notify( gci_fn_file_transfer_t fn );
GM_API_IMPL int GM_API_CALL gci_unregister_file_transfer_notify( gci_fn_file_transfer_t fn );
//主动发送
GM_API_IMPL int GM_API_CALL gci_file_transfer_send( const char * sFilePath, int iTag = 0 );
//被动、回应、响应发送
GM_API_IMPL int GM_API_CALL gci_file_transfer_response( const char * sFilePath, int iTag = 0 );





//*sdk端的实时区、遥信、遥测调用的
//quality （质量）： 1 表示好的、有效；0 表示无效
//datatime （采集时间）: 系统当前时间 now（当前毫秒数)
//value : 当前值
//address : 传送数据结构体每个都要带地址信息
//firstaddress : 表示以地址起始、结束的方式，后面传送数据结构体时就不用带地址信息
typedef struct
{
    int addressBegin;
    int addressEnd;
} gct_firstaddress_t;

//*yx
typedef struct
{
    int address;
    int quality;
    long long datetime;
    char value[2];
} gct_address_vqt_int_text1_t;

typedef struct
{
    int quality;
    long long datetime;
    char value[2];
} gct_vqt_int_text1_t;

//*yc
//real_text 只有15个有效字符
typedef struct
{
    int address;
    int quality;
    long long datetime;
    char value[16];
} gct_address_vqt_real_text15_t;

typedef struct{
    int quality;
    long long datetime;
    char value[16];
} gct_vqt_real_text15_t;


typedef void ( GM_API_CALL * gci_fn_realtime_container_data_t)( int iTag, const char * sContainerCode );

//注册回调函数，例如：对方发送 read、write、lock……等 方法过来时会触发此回调
GM_API_IMPL int GM_API_CALL gci_register_realtime_container_data_notify( gci_fn_realtime_container_data_t fn );
GM_API_IMPL int GM_API_CALL gci_unregister_realtime_container_data_notify( gci_fn_realtime_container_data_t fn );
//*主动发送 遥信 部分
GM_API_IMPL int GM_API_CALL gci_yx_address_vqt_text1_send( const gct_address_vqt_int_text1_t * pData, int iCount, const char * sContainerCode = 0 );
//(PE)
GM_API_IMPL int GM_API_CALL gci_yx_firstaddress_vqt_text1_send( gct_firstaddress_t firstAddress, const gct_vqt_int_text1_t * pData, int iCount, const char * sContainerCode = 0 );
//响应发送：由于收到对方的主动读、写、锁、解锁
//*被动动发送 遥信 部分
GM_API_IMPL int GM_API_CALL gci_yx_address_vqt_text1_response( const gct_address_vqt_int_text1_t * pData, int iCount, const char * sContainerCode = 0 );
GM_API_IMPL int GM_API_CALL gci_yx_firstaddress_vqt_text1_response( gct_firstaddress_t firstAddress, const gct_vqt_int_text1_t * pData, int iCount, const char * sContainerCode = 0 );

//*主动发送 遥测 部分
GM_API_IMPL int GM_API_CALL gci_yc_address_vqt_text15_send( const gct_address_vqt_real_text15_t * pData, int iCount, const char * sContainerCode = 0 );
//(PE)
GM_API_IMPL int GM_API_CALL gci_yc_firstaddress_vqt_text15_send( gct_firstaddress_t firstAddress, const gct_vqt_real_text15_t * pData, int iCount, const char * sContainerCode = 0 );
//*被动发送 遥测 部分
GM_API_IMPL int GM_API_CALL gci_yc_address_vqt_text15_response( const gct_address_vqt_real_text15_t * pData, int iCount, const char * sContainerCode = 0 );
GM_API_IMPL int GM_API_CALL gci_yc_firstaddress_vqt_text15_response( gct_firstaddress_t firstAddress, const gct_vqt_real_text15_t * pData, int iCount, const char * sContainerCode = 0 );


#ifdef __cplusplus
}
#endif

#endif // GCL_COMMON_SDK_H
