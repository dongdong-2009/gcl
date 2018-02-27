#ifndef GCL_MACRO_H
#define GCL_MACRO_H


const char CS_GCL_MARK_VL = '|';
const char CS_GCL_MARK_SPIT = ';';


/* Macro of array length and last*/
#define ARRAY_LENGTH(x)    (sizeof(x)/sizeof(x[0]))
#define ARRAY_LAST(x)      (sizeof(x)/sizeof(x[0]) - 1)
#define _strlen(x)         strlen((const char *)x)

#define U8BCDtoHEX(x)      (x>>4)*10 + (x & 0xf)
#define HEXtoU8BCD(x)      ((x/10)<<4) + (x % 10)

#define HEX_2_ASC(x)        (x>9) ? ((x-0x0a)+'A') : (x + '0')
#define ASC_2_HEX(x)        (x>'9') ? (0x0a+x-'A') : (x - '0')

#define HEX_2_BCD(x)        (((x/10)<< 4) + (x%10))
#define BCD_2_HEX(x)        (((x>>4)*10) + (x&0xf))

#define SWAP_U16(x)         (((x>>8)&0xff)|((x<<8)&0xff00))

#define  MAX(x,y) (((x)>(y)) ? (x) : (y))
#define  MIN(x,y) (((x) < (y)) ? (x) : (y))


//产品代码
#define GCL_APP_CICS  0x0001
#define GCL_APP_ICS   0x0002

#define GCL_APP_RTDB      0x2000

#define GCL_APP_MJ        0x2010
#define GCL_APP_MJ_NABO   (GCL_APP_MJ+0x00)

#define GCL_APP_HKVS      0x2020
#define GCL_APP_HKVS_DEC  (GCL_APP_HKVS+0x00)
#define GCL_APP_HKVS_IPC  (GCL_APP_HKVS+0x01)
#define GCL_APP_HKVS_NVR  (GCL_APP_HKVS+0x02)


#define GCL_APP_HDID      0x2030
#define GCL_APP_HDID_RA   (GCL_APP_HDID+0x00)

#define GCL_APP_LED       0x2040
#define GCL_APP_LED_LH    (GCL_APP_LED+0x00)


#define GCL_APP_ASM700       0x2070
#define GCL_APP_ASM700_V100  (GCL_APP_ASM700+0x01)

#define GCL_APP_ASM900       0x2090
#define GCL_APP_ASM900_V100  (GCL_APP_ASM900+0x00)
#define GCL_APP_ASM901_LJL   (GCL_APP_ASM900+0x01)
#define GCL_APP_ASM902_STA   (GCL_APP_ASM900+0x02)



//配置路径
#define GCL_PATH_CFG  "../cfg/"
#define GCL_PATH_LOG  "../log/"
#define GCL_PATH_TMP  "../tmp/"
#define GCL_PATH_HIS  "../his/"
#define GCL_PATH_DOC  "../doc/"
#define GCL_PATH_BIN  "../bin/"

//
#define GCL_STRING_EMPTY  ""
#define GCL_BACK_SLASH  "/"  //反斜线
//分隔符
#define GCL_MARK_SEPARATOR  ":"  //冒号分隔符
#define GCL_MARK_BACKSLASH  "/"  //反斜线
#define GCL_MARK_COLON    ":"  //冒号分隔符
#define GCL_MARK_VL       "|"  //竖线分隔符

//无效值定义
#define GC_VAL_INT_INVALID -1 //无效地址
//文件名
#define GCL_NAME_LOG_DEFAULT  "system"
#define GCL_NAME_LOG_SYSTEM   "system"
#define GCL_FILE_DEFAULT      "system"

//变量定义
#define	GCL_VT_YX					0x01
#define	GCL_VT_YC					0x02
#define	GCL_VT_YK					0x03
#define	GCL_VT_YT					0x04
#define	GCL_VT_YS					0x05
#define	GCL_VT_STR					0x06

#define	GCL_VT_P0					0x10  //单脉冲
#define	GCL_VT_P1					0x11  //周期脉冲


//Measure类型
#define	GCL_MEAS_MAX    			8
#define	GCL_MEAS_YX_SEND			0
#define	GCL_MEAS_YX_REV		    	1
#define	GCL_MEAS_YC_SEND			2
#define	GCL_MEAS_YC_REV		    	3
#define	GCL_MEAS_YK_SEND			4
#define	GCL_MEAS_YK_REV		    	5
#define	GCL_MEAS_SC_SEND 			6 //字符串
#define	GCL_MEAS_SC_REV 			7

//ASCII
#define GCL_ASCII_NULL   0x00   //空
#define GCL_ASCII_STX    0x02   //正文开始
#define GCL_ASCII_ETX    0x03   //正文结束
#define GCL_ASCII_ENQ    0x05   //请求
#define GCL_ASCII_ACK    0x06   //确认
#define GCL_ASCII_LF     0x0A   //换行
#define GCL_ASCII_CR     0x0D   //回车
#define GCL_ASCII_SYN    0x16   //同步空闲
#define GCL_ASCII_SP     0x20   //空格

//数据类型
#define GCL_VT_VOID  0   //void*
#define GCL_VT_U1    1   //uchar
#define GCL_VT_I1    2   //char
#define GCL_VT_U2    3   //usigned  short
#define GCL_VT_I2    4   //short
#define GCL_VT_U4    5   //usigned int32
#define GCL_VT_I4    6   //int32
#define GCL_VT_U8    7   //usigned int64
#define GCL_VT_I8    8   //int64
#define GCL_VT_F4    9   //float
#define GCL_VT_T4    10  //time
#define GCL_VT_D8    11  //double
#define GCL_VT_L8    12  //long

#define GCL_VT_STRING    32 //string


#define GCL_VT_VL    255  //变长
#define GCL_VT_STU   256 //结构体
#define GCL_VT_ARY   257 //数组


//长度定义
#define GCL_LEN_PROMPT      128    //提示
#define GCL_LEN_DESC        128    //描述
#define GCL_LEN_NAME        64     //名字
#define GCL_LEN_UNIT        32     //单位
#define GCL_LEN_DATA_EM     3980   //数据单元长度定义,最长4072，接收缓冲区4K(4096),发送缓冲区 4000 ,3984+16
#define GCL_LEN_TEMP        1024   //临时数据长度
//#define GCL_LEN_DGM         3999   //数据报文长度,最长4072，接收缓冲区4K(4096),发送缓冲区 4000
#define GCL_LEN_DT          1024   //数据长度
#define GCL_LEN_BUFFER      65536  //缓冲区长度
#define GCL_LEN_YW          128    //遥文长度

#define GCL_MAX_QUEUE       1024   //队列最大深度


#define GCL_LEN_BUFF_S      255    //单字节长度
#define GCL_LEN_FILE        255    //文件长度
#define GCL_LEN_PATH_FULL   255    //全路径
#define GCL_MAX_PATH        255    //全路径


//Debug
#define GCL_DEBUG_BASE    1
#define GCL_DEBUG_1       2
#define GCL_DEBUG_2       4
#define GCL_DEBUG_3       8
#define GCL_DEBUG_4       16
#define GCL_DEBUG_5       32
#define GCL_DEBUG_6       64
#define GCL_DEBUG_MAX     128

//
#define GCL_TYPE_HKVS    0x01  //海康威视
#define GCL_TYPE_DH      0x02  //大华


#define GCL_ERR(type,code) ((type<<24)|code)
//
//返回码定义
#define GCL_ERR_BASE   0x0000
#define GCL_SUCCED     (GCL_ERR_BASE+0x00)
#define GCL_ERR_NO     (GCL_ERR_BASE+0x00)
#define GCL_ERR_FAIL   (GCL_ERR_BASE+0x01)
#define GCL_ERR_PM     (GCL_ERR_BASE+0x02) //参数错误
#define GCL_ERR_MEM    (GCL_ERR_BASE+0x03) //内存分配错误
#define GCL_ERR_FUN    (GCL_ERR_BASE+0x04) //函数错误
#define GCL_ERR_ACCESS (GCL_ERR_BASE+0x05) //访问错误
#define GCL_ERR_SINGLE (GCL_ERR_BASE+0x06) //单实例错误
#define GCL_ERR_BUF_LEN (GCL_ERR_BASE+0x07) //缓冲区长度
#define GCL_ERR_TYPE    (GCL_ERR_BASE+0x08) //类型错误
#define GCL_ERR_STATUS  (GCL_ERR_BASE+0x09) //状态错误
#define GCL_ERR_SIZE    (GCL_ERR_BASE+0x0A) //大小
#define GCL_ERR_LEN     (GCL_ERR_BASE+0x0B) //长度
#define GCL_ERR_FORMAT  (GCL_ERR_BASE+0x0C) //格式


#define GCL_ERR_SHM     (GCL_ERR_BASE+0x10) //共享内存错误
#define GCL_ERR_SHM_FUN (GCL_ERR_BASE+0x11) //共享内存函数
#define GCL_ERR_SHM_ACC (GCL_ERR_BASE+0x12) //共享内存访问函数

#define GCL_ERR_CMD_NO_IMP  (GCL_ERR_BASE+0x20)//命令未实现
#define GCL_ERR_FUN_NO_IMP  (GCL_ERR_BASE+0x21)//功能未实现
#define GCL_ERR_MODE_NO_IMP (GCL_ERR_BASE+0x22)//功能未实现

// 命令码定义
#define GCL_CMD_SYS_BASE  0x0000
#define GCL_CMD_SYS_DEBUG   (GCL_CMD_SYS_BASE+0x01) //调试

#define GCL_CMD_SYS_CLOCK   (GCL_CMD_SYS_BASE+0x10) //校时
#define GCL_CMD_SYS_TASK    (GCL_CMD_SYS_BASE+0x11) //任务
#define GCL_CMD_SYS_CALL    (GCL_CMD_SYS_BASE+0x12) //召唤


#define GCL_CMD_SYS_POWER   (GCL_CMD_SYS_BASE+0x20) //电源控制
#define GCL_CMD_SYS_SVR     (GCL_CMD_SYS_BASE+0x21) //服务控制
#define GCL_CMD_SYS_SCENCE  (GCL_CMD_SYS_BASE+0x22) //场景控制

#define GCL_CMD_SYS_SEND_FILE         (GCL_CMD_SYS_BASE+0x30) //发送文件
#define GCL_CMD_SYS_SEND_FILE_CMD     (GCL_CMD_SYS_BASE+0x31) //命令方式发送文件


#define GCL_CMD_SYS_SEND_MSG  (GCL_CMD_SYS_BASE+0x35) //发送消息




#define GCL_CMD_SYS_YK       (GCL_CMD_SYS_BASE+0x40) //遥控
#define GCL_CMD_SYS_YT       (GCL_CMD_SYS_BASE+0x41) //遥调



#define GCL_CMD_SYS_CMD           (GCL_CMD_SYS_BASE+0x45) //命令字

#define GCL_CMD_SYS_MSG_TRAN      (GCL_CMD_SYS_BASE+0x48) //消息转发

#define GCL_CMD_SYS_VTL_W       (GCL_CMD_SYS_BASE+0x4A) //VLT写1  标准模式
#define GCL_CMD_SYS_VTL_W_16    (GCL_CMD_SYS_BASE+0x4B) //VLT写2  标准16进制模式
#define GCL_CMD_SYS_VTL_B       (GCL_CMD_SYS_BASE+0x4C) //VLT写3  字节模式
#define GCL_CMD_SYS_VTL_B_16    (GCL_CMD_SYS_BASE+0x4D) //VLT写3  字节模式16进制模式


#define GCL_CMD_SYS_YX_S    (GCL_CMD_SYS_BASE+0x50) //遥信
#define GCL_CMD_SYS_YC_S    (GCL_CMD_SYS_BASE+0x55) //遥测
#define GCL_CMD_SYS_YK_S    (GCL_CMD_SYS_BASE+0x60) //遥测
#define GCL_CMD_SYS_SC_S    (GCL_CMD_SYS_BASE+0x65) //字符串刷新
#define GCL_CMD_SYS_SC_W    (GCL_CMD_SYS_BASE+0x66) //字符串写
#define GCL_CMD_SYS_SC_R    (GCL_CMD_SYS_BASE+0x67) //字符串读
#define GCL_CMD_SYS_SC_W2    (GCL_CMD_SYS_BASE+0x68) //字符串刷新格式2

#define GCL_CMD_SYS_MEAS_W_S2   (GCL_CMD_SYS_BASE+0x70) //变量写
#define GCL_CMD_SYS_MEAS_W_I4   (GCL_CMD_SYS_BASE+0x71) //变量写
#define GCL_CMD_SYS_MEAS_W_F4   (GCL_CMD_SYS_BASE+0x72) //变量写
#define GCL_CMD_SYS_MEAS_W_D8   (GCL_CMD_SYS_BASE+0x73) //变量写

#define GCL_CMD_SYS_INFO        (GCL_CMD_SYS_BASE+0x80) //二进制信息
#define GCL_CMD_SYS_INFO_TXT    (GCL_CMD_SYS_BASE+0x81) //文本信息

#define GCL_CMD_SYS_SQL     (GCL_CMD_SYS_BASE+0x90) //SQL语句

#define GCL_CMD_SYS_XML          (GCL_CMD_SYS_BASE+0x95) //XML语句

#define GCL_CMD_SYS_XML_CICS     (GCL_CMD_SYS_BASE+0x96) //CICS XML文件处理

#define GCL_CMD_SYS_EFILE        (GCL_CMD_SYS_BASE+0x98) //E文件处理

#define GCL_CMD_SYS_MESSAGE      (GCL_CMD_SYS_BASE+0x9A) //消息处理处理




#define GCL_CMD_SYS_LIVE    (GCL_CMD_SYS_BASE+0xA0) //心跳
#define GCL_CMD_SYS_REG     (GCL_CMD_SYS_BASE+0xA1) //注册
#define GCL_CMD_SYS_UNREG   (GCL_CMD_SYS_BASE+0xA2) //注销

#define GCL_CMD_SYS_MEAS_REG    (GCL_CMD_SYS_BASE+0xA4) //测点注册
#define GCL_CMD_SYS_MEAS_UNREG  (GCL_CMD_SYS_BASE+0xA5) //测点注销


//提示信息
#define GCL_CMD_PROMPT    0x1001  //提示信息
#define GCL_CMD_TITLE     0x1002  //title
#define GCL_CMD_APP_EXIT  0x1003  //程序退出
//监控系统
#define GCL_CMD_ICS_BASE  0x2000
#define GCL_CMD_ICS_MJ         (GCL_CMD_ICS_BASE+0x101)   //门禁系统
#define GCL_CMD_ICS_DID        (GCL_CMD_ICS_BASE+0x201)   //DID拼接屏
#define GCL_CMD_ICS_IPC        (GCL_CMD_ICS_BASE+0x801)   //网络摄像机
#define GCL_CMD_ICS_IPC_PRESET (GCL_CMD_ICS_BASE+0x801)   //网络摄像机预置位
#define GCL_CMD_ICS_IPC_LAMP   (GCL_CMD_ICS_BASE+0x802)   //网络摄像机灯光雨刷
#define GCL_CMD_ICS_IPC_CAPTURE_V1   (GCL_CMD_ICS_BASE+0x803)   //网络摄像机抓图
#define GCL_CMD_ICS_IPC_CAPTURE_V2   (GCL_CMD_ICS_BASE+0x804)   //网络摄像机抓图

#define GCL_CMD_ICS_DEC   (GCL_CMD_ICS_BASE+0x901)   //解码器
#define GCL_CMD_ICS_NVR   (GCL_CMD_ICS_BASE+0xA01)   //录像机
//ASM570段
#define GCL_CMD_ASM570_BASE  0x7000
#define GCL_CMD_ASM570_REQ_BOTTLE  (0x7000+0x01) //取样品申请

//ASM90段
#define GCL_CMD_ASM90_BASE  0x9000

#define GCL_CMD_ASM901_CARD (GCL_CMD_ASM90_BASE+0x101)   //取样终端刷卡
#define GCL_CMD_ASM902_SMP  (GCL_CMD_ASM90_BASE+0x201)   //检测终端监测

//功能码
#define GCL_FC_ALM_ERR  0x0001  //严重
#define GCL_FC_ALM_EVT  0x0002  //警告
#define GCL_FC_ALM_NML  0x0003  //普通

//电源控制方式
#define GCL_FC_POW_ON       0x0001  //开机
#define GCL_FC_POW_OFF      0x0002  //关机
#define GCL_FC_POW_REBOOT   0x0003  //重新启动
#define GCL_FC_POW_SLEEP    0x0004  //休眠
#define GCL_FC_POW_CLEAR    0x0005  //清屏

//数据库类型
#define GCL_DB_ODBC      0
#define GCL_DB_ORACLE    1
#define GCL_DB_MSSQL     2

//点状态定义
#define GCL_PT_YX_ON    2
#define GCL_PT_YX_OFF   1
#define GCL_PT_YX_NC    0
//品质定义
#define GCL_Q_GOOD      1
#define GCL_Q_BAD       0

#define GCL_PERIOD_RTDB  100 //实时数据周期
//
#define GCL_MSG_FLAG_LEAD  0xA55A5AA5

//变化原因
#define ci_tag_GeneralAccess (0x11000010)
#define ci_tag_ShareMemory   (0x11000011)

//ASM570
#define ASM570_CMD_FLOW_AUTO   1   //自动 存样/弃样/取样
#define ASM570_CMD_FLOW_MANU   2   //手动
#define ASM570_CMD_FLOW_DEBUG  3   //调试
#define ASM570_CMD_FLOW_MANU_SAVE  4   //手动存
#define ASM570_CMD_FLOW_MANU_GIVE  5   //手动取
#define ASM570_CMD_FLOW_MANU_DISCARD  6   //手动弃

//GCL系统功能
#define ci_reason_tm_config_req     (0x21020001)
#define ci_reason_tm_config_resp    (0x21020002)

//遥信定点
#define GCL_Measure_Index_HeartJump         (0)
#define GCL_Measure_Index_OnLine            (1)

//遥测定点
#define GCL_Measure_Index_NowTime           (0)
#define GCL_Measure_Index_LastReceivedTime  (1)
#define GCL_Measure_Index_LastSentTime      (2)
#define GCL_Measure_Index_ReceivedByteCount (3)
#define GCL_Measure_Index_SentByteCount     (4)

//遥文定点
#define GCL_Measure_Index_Info              (0)

#endif // GCL_MACRO_H
