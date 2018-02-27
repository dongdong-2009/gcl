#ifndef GCL_COMMON_SDK_H
#define GCL_COMMON_SDK_H


//true : int>0
//false : int<0
//nothind : int==0
//const char * s ： 默认是C字符串风格，以'\0'(0x00)为结束符
//FilePath : 表示全路径
//sContainerCode ：仪器编号
//发送有两种： 1)，send : 主动发送。 2），response : 响应发送，由于收到对方的主动读、写、锁、解锁，而发送出去的


//注意：
//回调函数默认情况是在子线程中运行，如果有全局处理的，请用异步的方式（像 postmessage)

//外给第三方厂家时，psm_struct.h可以不用 include
#include "psm_struct.h"

#if defined(GCI_API_IMPL_EXPORT)
    #define GCI_API_IMPL __declspec(dllexport)
#elif defined(GCI_API_IMPL_SRC)
    #define GCI_API_IMPL
#else
    #define GCI_API_IMPL __declspec(dllimport)
#endif

//#define GCI_API_CALL __stdcall
#define GCI_API_CALL __cdecl

#ifdef __cplusplus
extern "C" {
#endif



//必须*用此库前必须初始化
//参数 ：argc 与 argv 是 C语言 main(int argc, char * argv[]) 意思一样
//返回初始化状态 0：表示没有初始化，大于0表示已经初始化，小于0表示初始失败原因
GCI_API_IMPL int GCI_API_CALL gci_init(int argc, char * argv[]);

//必须*用完此库后必须清除
//可以不用理会返回值
GCI_API_IMPL int GCI_API_CALL gci_cleanup();

//返回初始化状态 0：表示没有初始化，大于0表示已经初始化，小于0表示初始失败原因
GCI_API_IMPL int GCI_API_CALL gci_init_status();

//返回是否加载通道并通道连接上
GCI_API_IMPL int GCI_API_CALL gci_terminal_status(int iId = 0);

//返回终端离线毫秒数 例如： gci_terminal_status() > HeartJumpTime 表示已经有X毫秒没收到报文
GCI_API_IMPL int GCI_API_CALL gci_terminal_online_millisecond(int iId = 0);

//获取错误信息
GCI_API_IMPL char * GCI_API_CALL gci_terminal_info(int iId = 0);

//测试
GCI_API_IMPL int GCI_API_CALL gci_test(int iTest);




//*系统错误、程序错误的回调函数指针类型
typedef void ( GCI_API_CALL * gci_fn_error_t)( int iErrorid );
//注册回调
GCI_API_IMPL int GCI_API_CALL gci_register_error_notify( gci_fn_error_t fn );
GCI_API_IMPL int GCI_API_CALL gci_unregister_error_notify( gci_fn_error_t fn );
//获取最后错误
GCI_API_IMPL int GCI_API_CALL gci_last_error_id();
//获取错误信息
GCI_API_IMPL char * GCI_API_CALL gci_error_info(int iErrorId = 0);





//*命令 - 用消息形式收发
//消息命令由Command区 + Param区组成，像函数中（函数名 + 参数）
//1）、sCommand是以\0结束
//2）、sParam是参数指针
//3）、iParamLength是参数长度
//4）、oAttach是数据结构PsmAttach的对象地址
//Command结构： aaa.bbb.ccc.ddd.eee v1=k1 v2=k2
//aaa : req resp send post
//bbb : 模块
//ccc : 主命令
//ddd : 子命令
//eee : 版本号
//v1=k1 v2=k2 : 命令补充
//Param结构：
//v001=k001;v002=k002;v003=k003\nv011=k011;v012=k012;v013=k013
//单个或多个对象时都以键值对组成，以分号分隔属性，以\n换行符分隔对象
typedef void ( GCI_API_CALL * gci_fn_message_command_t)( const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach );
//注册回调
///
/// \brief gci_register_message_command_notify : 注册消息回调
/// \param fn : 回调指针
/// \sample :
/// void message_command( const char * sCommand, const char * sParam, int iParamLength ) { /* 收到消息，在这处理收到的消息 if ( sCommand == "leadto_huanyang" ) postmessge(hd, iLeadto_huanyang); */ }
/// gci_register_message_command_notify( message_command );
GCI_API_IMPL int GCI_API_CALL gci_register_message_command_notify( gci_fn_message_command_t fn );
GCI_API_IMPL int GCI_API_CALL gci_unregister_message_command_notify( gci_fn_message_command_t fn );
//主动发送
GCI_API_IMPL int GCI_API_CALL gci_message_command_send( const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = 0 );





//*文件传送
//FileName结构： aaa_bbb_ccc_ddd.xxx
//aaa : 模块、业务种类
//bbb : 用途，用来做什么
//ccc: 日期时间 yyyymmddhhmmss
//ddd: req resp send post
//xxx: 文件类型名
//
//文件回调（当接收到文件 或 对方已经成功收到文件）
typedef void ( GCI_API_CALL * gci_fn_file_transfer_t)( const char * sFilePath, const PsmAttach * oAttach );
//注册回调 当接收到文件后
GCI_API_IMPL int GCI_API_CALL gci_register_file_transfer_notify( gci_fn_file_transfer_t fn );
GCI_API_IMPL int GCI_API_CALL gci_unregister_file_transfer_notify( gci_fn_file_transfer_t fn );
//注册回调 当对方已经成功收到文件
GCI_API_IMPL int GCI_API_CALL gci_register_file_transfer_result_notify( gci_fn_file_transfer_t fn );
GCI_API_IMPL int GCI_API_CALL gci_unregister_file_transfer_result_notify( gci_fn_file_transfer_t fn );
//主动发送
//>0：表示成功发送
//-1：表示链路打开不成功；-2：表示没有缓存了；-3、-4：表示文件不存在或加载不成功；0：表示文件太大或其它异常）
GCI_API_IMPL int GCI_API_CALL gci_file_transfer_send( const char * sFilePath, const PsmAttach * oAttach = 0 );



//*实时数据：遥信、遥测、遥文
typedef void ( GCI_API_CALL * gci_fn_realtime_data_request_t)( const PsmAttach * oAttach );
//iStructType数据结构体类型ID号（例如遥信、遥测结构体号参考psm_struct.h，pBuffer结构体数组指针，iBufferLength结构体数组长度，iCount结构体数组成员个数，oAttach扩展数据
typedef void ( GCI_API_CALL * gci_fn_realtime_data_post_t)( int iStructType, const char * pBuffer, int iBufferLength, int iCount, const PsmAttach * oAttach );

//注册回调函数，例如：对方发送 request、post、lock……等 方法过来时会触发此回调
GCI_API_IMPL int GCI_API_CALL gci_register_realtime_data_request_notify( gci_fn_realtime_data_request_t fn );
GCI_API_IMPL int GCI_API_CALL gci_unregister_realtime_data_request_notify( gci_fn_realtime_data_request_t fn );

GCI_API_IMPL int GCI_API_CALL gci_register_realtime_data_post_notify( gci_fn_realtime_data_post_t fn );
GCI_API_IMPL int GCI_API_CALL gci_unregister_realtime_data_post_notify( gci_fn_realtime_data_post_t fn );

////*主动发送 遥信 部分
//iStructType数据结构体类型ID号（例如遥信、遥测结构体号参考psm_struct.h，pBuffer结构体数组指针，iBufferLength结构体数组长度，iCount结构体数组成员个数，oAttach扩展数据
GCI_API_IMPL int GCI_API_CALL gci_realtime_data_request( const PsmAttach * oAttach = 0 );
GCI_API_IMPL int GCI_API_CALL gci_realtime_data_post( int iStructType, const char * pBuffer, int iBufferLength, int iCount = 1, const PsmAttach * oAttach = 0 );


//*实时数据读（以数组array，散列list，单个的方式）目前支持结构体类型：遥信（gcd_address_int32_vqt_t）， 遥测（gcd_address_double_vqt_t）， 遥文（gcd_firstAddress_vqt_string128_t）
/**
 * @brief gci_realtime_data_get_array 以MeasureId的连续（0x01000001，0x01000002，0x01000003……）方式来读
 * @param iBeginMid ： MeasureId （起始ID）
 * @param iMidCount ：要读的数量
 * @param iStructType ：结构体类型
 * @param pOutResult ：(输出)，查询结果存放的结构体指针
 * @param iOutResultLength　：查询结果存放的结构体数据长度
 * @param oAttach ：扩展数据
 * @return ： 0：什么都没发生，函数空白（函数没实现）； >0 ：表示成功读了多少个；； <0 ：表示错误；
 */
GCI_API_IMPL int GCI_API_CALL gci_realtime_data_get_array( int iBeginMid, int iMidCount, int iStructType, char * pOutResult, int iOutResultLength, const PsmAttach * oAttach = 0 );

/**
 * @brief gci_realtime_data_get_array 以MeasureId的散列（0x01000001，0x01000102，0x01000203……）方式来读
 * @param iStructType ：结构体类型
 * @param pOutResult ：(输出)，查询结果存放的结构体指针（结构体中要有Mid字段值，即 address）
 * @param iOutResultLength　：查询结果存放的结构体数据长度
 * @param iStructCount ：要读的数量
 * @param oAttach ：扩展数据
 * @return ： 0：什么都没发生，函数空白； >0 ：表示成功读了多少个；； <0 ：表示错误；
 */
GCI_API_IMPL int GCI_API_CALL gci_realtime_data_get_list( int iStructType, char * pOutResult, int iOutResultLength, int iStructCount, const PsmAttach * oAttach = 0 );

/**
 * @brief gci_realtime_data_get_array 以MeasureId的散列（0x01000001，0x01000102，0x01000203……）方式来读
 * @param iMid ： MeasureId （要读的Mid）
 * @param iStructType ：结构体类型
 * @param pOutResult ：(输出)，查询结果存放的结构体指针
 * @param iResultLength　：查询结果存放的结构体数据长度
 * @param oAttach ：扩展数据
 * @return ： 0：什么都没发生，函数空白； >0 ：表示成功读了多少个；； <0 ：表示错误；
 */
GCI_API_IMPL int GCI_API_CALL gci_realtime_data_get( int iMid, int iStructType, char * pOutResult, int iOutResultLength, const PsmAttach * oAttach = 0 );


//*实时数据写（散列list，单个的方式）目前支持结构体类型：遥信（gcd_address_int32_vqt_t）， 遥测（gcd_address_double_vqt_t）， 遥文（gcd_firstAddress_vqt_string128_t）
/**
 * @brief gci_realtime_data_set_list 以MeasureId的散列（0x01000001，0x01000102，0x01000203……）方式来写
 * @param iStructType ：结构体类型
 * @param pStructData　：存放的结构体指针
 * @param iStructDataLength　：结构体数据总长度
 * @param iStructCount ：结构体的数量
 * @param oAttach ：扩展数据
 * @return ： 0：什么都没发生，函数空白； >0 ：表示已经成功有多个个加入写的队列；注意！注意！注意：成功加入队列不代表实时写到实时库，返较可以通过读来实现； <0 ：表示错误；
 */
GCI_API_IMPL int GCI_API_CALL gci_realtime_data_set_list( int iStructType, char * pStructData, int iStructDataLength, int iStructCount, const PsmAttach * oAttach = 0 );

/**
 * @brief gci_realtime_data_set 以MeasureId的单个方式来写
 * @param iStructType ：结构体类型
 * @param pStructData　：存放的结构体指针
 * @param iStructDataLength　：结构体数据总长度
 * @param oAttach ：扩展数据
 * @return ： 0：什么都没发生，函数空白； >0 ：表示已经成功有多个个加入写的队列；注意！注意！注意：成功加入队列不代表实时写到实时库，返较可以通过读来实现； <0 ：表示错误；
 */
GCI_API_IMPL int GCI_API_CALL gci_realtime_data_set( int iStructType, char * pStructData, int iStructDataLength, const PsmAttach * oAttach = 0 );

#ifdef __cplusplus
}
#endif


#endif // GCL_COMMON_SDK_H
