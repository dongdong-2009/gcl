
#ifndef PLC_FINS_GLOBAL_H
#define PLC_FINS_GLOBAL_H

///////////////////////////////////////////////////////////////////////////

#define ASM570_CMD_FLOW_AUTO   1   //自动 存样/弃样/取样
#define ASM570_CMD_FLOW_MANU   2   //手动
#define ASM570_CMD_FLOW_DEBUG  3   //调试
#define ASM570_CMD_FLOW_MANU_SAVE  4   //手动存
#define ASM570_CMD_FLOW_MANU_GIVE  5   //手动取
#define ASM570_CMD_FLOW_MANU_DISCARD  6   //手动弃

///////////////////////////////////////////////////////////////////////////

#define ASM570_FINS_FRM_LEN_MIN  16  //最小长度
#define ASM570_FINS_FRM_LEN_FIX  8  //固定长度
#define ASM570_FINS_DATA_LEN_MAX 2000 //数据域最长长度

#define ASM570_FINS_FRM_LEADFlAG 0x46494e53

#define ASM570_FINS_POS_LEAD     0  //引导
#define ASM570_FINS_POS_DATA_LEN 4  //用户数据长度位置
#define ASM570_FINS_POS_CMD      8  //用户数据长度位置
#define ASM570_FINS_POS_ERR      12  //用户数据长度位置


#define ASM570_FINS_CMD_REG         0x00000000  //注册包
#define ASM570_FINS_CMD_REG_RESP    0x00000001  //注册响应
#define ASM570_FINS_CMD_WORK        0x00000002  //工作

#define ASM570_FINS_MS_CPU_RD    0x0101  //CPU 读
#define ASM570_FINS_MS_CPU_WR    0x0102  //CPU 写

#define ASM570_FINS_SEND         0x80 //发送
#define ASM570_FINS_RESP         0xC0 //响应

#define ASM570_FINS_ERR_SUCCED   0x00000000 //成功
////////////////////////////////////////////////////////////
#define GM_FaceA 1
#define GM_FaceB 2


#define GM_STATUS_DOOR 1
#define GM_STATUS_HANDSAVE_FINISH 2
#define GM_STATUS_HANDGET_FINISH  3


//entry 口定义
#define GM_PLC_STEP_TOTAL_SAVE        7
#define GM_PLC_STEP_TOTAL_GIVE        6
#define GM_PLC_STEP_TOTAL_DISCARD     7

///////////////////////////////////////////////////////////
#define PLC_RETRY_TIME     3    //重试次数
#define PLC_WATING_PERIOD  500  //等待周期
#define PLC_WATING_IDLE    100  //等待周期
#define PLC_WATING_TIME    3    //等待倍数

#define PLC_ST_ON    1    //ON
#define PLC_ST_OFF   0    //ON
//error 定义
#define ASM570_SUCCESS         0x0000


#define ASM570_WARN_BASE       0x0100
#define ASM570_ERR_PLC_EMPTY   (ASM570_WARN_BASE+1)   //空


#define ASM570_ERR_BASE        0x0200
#define ASM570_ERR_PLC_AXIX          (ASM570_ERR_BASE+1)   //轴故障
#define ASM570_ERR_PLC_FEED_MANU     (ASM570_ERR_BASE+2)   //手动进料故障
#define ASM570_ERR_PLC_FEED_AUTO     (ASM570_ERR_BASE+3)   //自动动进料故障
#define ASM570_ERR_PLC_SAMP_BLK      (ASM570_ERR_BASE+4)   //样品卡

#define PLC_BUF_LEN  4096  //PLC缓冲区长度
//////////////////////////////////////////////////////////
#define IR2000_00 6
#define WR5_01    7
#define IR50_01   8
#define IR50_02   9
#define WR4_00    10
#define WR5_00    11
#define WR1_00    12
#define WR1_01    13
#define WR1_02    14
#define WR1_03    15
#define WR1_04    16
#define WR1_05    17
#define WR1_06    18
#define WR1_07    19
#define WR1_09    20
#define WR6_01    21
#define WR6_02    22
#define WR5_02    23
#define IR2000_01 24
#define IR2000_02 25
#define IR2000_03 26
#define IR2000_04 27
#define IR2000_05 28
#define IR2000_06 29
#define IR2000_07 30
#define IR2000_08 31
#define IR2000_09 32
#define IR2000_010 33
#define IR2000_011 34
#define IR2000_012 35
#define IR1219_00 36
#define IR110_01  37
#define IR100_03  38
#define IR1601_01 39
#define IR1218_03 44
#define IR100_05  45
#define IR102_01  46
#define IR102_00  47
#define IR30_05   48
#define IR30_06   49
#define IR1000_00 50
#define IR1000_02 51
#define IR1213_00 52
#define IR1600_02 53
#define IR1600_03 54
#define IR4_01    55
#define IR1215_00 56
#define IR1215_01 57
#define WR6_06    58
#define IR1000_03 59
#define IR1600_04 60
#define IR50_00   61
#define IR50_04   62
#define IR51_040  63
#define IR51_02   64
#define IR51_00   65
#define IR51_01   66
#define IR10_11   67
#define IR1202_05 68
#define IR1213_09 69
#define IR1213_11 70
#define IR1217_00 71
#define IR1600_10 72
#define IR1216_08 73
#define IR1211_01 74
#define IR100_01  75
#define IR100_00  76
#define IR1216_13 77
#define IR101_01  78
#define IR101_00  79
#define IR110_00  80
#define IR1600_05 81
#define IR1600_11 82
#define IR1600_12 83
#define IR1217_02 84
#define IR1217_03 85
#define IR1601_05 86
#define IR1601_06 87
#define IR1217_04 88
#define HR5_00    89
#define IR2000_13 90
#define IR2000_14 91
#define IR1218_15 92
#define IR2000_15 93
#define IR1213_10 94
#define IR1601_02 95
#define IR100_06  96
#define IR100_07  97
#define IR100_08  98
#define IR1211_06 99
#define WR5_03    100




#endif // PLC_FINS_GLOBAL_H
