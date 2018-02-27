#ifndef FRID_YGW_GLOBAL_H
#define FRID_YGW_GLOBAL_H

#define RFID_YWG_FRM_LEN_MIN  6  //最小长度
#define RFID_YWG_FRM_LEN_FIX  5  //固定长度  0x55（1）+长度（2）+校验（2）

#define ASM570_RFID_ERR_SUCCED   0x00000000 //成功
#define RFID_INQRY_POWER     0
#define RFID_READ_GENERAL    1
#define RFID_WAITING         2

#define RFID_LEN_TMP          32
#define RFID_LEN_BUF          1024
#define RFID_YWG_FRM_LEADFlAG 0x55
#define RFID_LEN_TID           12
#define RFID_LEN_USER_DATA     16
#define RFID_LEN_MM            5


#define RFID_YWG_POS_LEAD     0  //引导
#define RFID_YWG_POS_DATA_LEN 1  //用户数据长度位置
#define RFID_YWG_POS_CMD      3  //用户数据命令位置
//命令字
#define RFID_YWG_CMD_POEWR_OFF    0x61 //功率关闭
#define RFID_YWG_CMD_POEWR_QUERY   0x62 //功率查询
#define RFID_YWG_CMD_GM_READ       0x97 //通用读

//#define RFID_YWG_POS_ERR      12  //用户数据长度位置

#endif // FRID_YGW_GLOBAL_H
