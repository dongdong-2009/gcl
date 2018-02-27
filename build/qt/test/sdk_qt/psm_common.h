#ifndef PSM_COMMON_H
#define PSM_COMMON_H


#include <string>

/*
命令码：分127个区
核心区有：7个，其它是扩展区
核心区0x01000000：平台环境、网络环境（局域网、广域网）
核心区0x02000000：系统、框架、程序运行环境
核心区0x03000000：APP、应用程序、进程、消息、文件
核心区0x04000000：（所在的APP产生的）配置、固化、整参
核心区0x05000000：（所在的APP产生的）实时、动态数据、日常活动、事件、告警
核心区0x06000000：（所在的APP产生的）控制、命令、任务
核心区0x07000000：（所在的APP产生的）日志、历史查询、分析、业务支持
*/

#define gct_core1_base (0x01000000)
#define gct_core2_base (0x02000000)
#define gct_core3_base (0x03000000)
#define gct_core4_base (0x04000000)
#define gct_core5_base (0x05000000)
#define gct_core6_base (0x06000000)
#define gct_core7_base (0x07000000)


//link check
#define gct_channel_base (gct_core1_base + (0x050000))
#define gct_channel_hand (gct_channel_base + (0x0202))
#define gct_channel_check (gct_channel_base + (0x0302))


//message
#define gct_message_base (gct_core3_base + (0x050000))
//0x03050101 03 05 01 01
#define gct_message_command_param (gct_message_base + (0x0101))

//file
#define gct_file_base (gct_core3_base + (0x060000))
//#define gct_file_information_read (gct_file_base + (0x0101))
//0x03060102 03 06 01 02
#define gct_file_information_write (gct_file_base + (0x0102))
//#define gct_file_content_read (gct_file_base + (0x0201))
//0x03060202 03 06 02 02
#define gct_file_content_write (gct_file_base + (0x0202))
//0x03060302 03 06 03 02
#define gct_file_shell (gct_file_base + (0x0302))


//realtime
#define gct_realtime_base (gct_core5_base + (0x050000))
//0x05050101 05 05 01 01
#define gct_realtime_data_request (gct_realtime_base + (0x0101))
//0x05050101 05 05 01 02
#define gct_realtime_data_post (gct_realtime_base + (0x0102))




const std::string GM_req_sys_login_1 = "req.sys.user.login.1";
const std::string GM_req_db_sql_execute_1 = "req.db.sql.execute.1";
const std::string GM_req_db_sql_select_1 = "req.db.sql.select.1";

const std::string GM_resp_sys_login_1 = "resp.sys.user.login.1";
const std::string GM_resp_db_sql_execute_1 = "resp.db.sql.execute.1";
const std::string GM_resp_db_sql_select_1 = "resp.db.sql.select.1";

const std::string CS_EntryPsmStationNumSource  = "PsmStationNumSource";
const std::string CS_EntryPsmStationNumTarget  = "PsmStationNumTarget";

const std::string CS_EntryPsmSentReason        = "PsmSentReason";
const std::string CS_EntryPsmSentContainerId   = "PsmSentContainerId";
const std::string CS_EntryPsmSentSourceId      = "PsmSentSourceId";
const std::string CS_EntryPsmSentTargetId      = "PsmSentTargetId";
const std::string CS_EntryPsmSentTag           = "PsmSentTag";
const std::string CS_EntryPsmHeartJumpInterval = "PsmHeartJumpInterval";
const std::string CS_EntryPsmYxSendInterval    = "PsmSendYxInterval";
const std::string CS_EntryPsmYcSendInterval    = "PsmSendYcInterval";
const std::string CS_EntryPsmYwSendInterval    = "PsmSendYwInterval";




#endif // PSM_COMMON_H
