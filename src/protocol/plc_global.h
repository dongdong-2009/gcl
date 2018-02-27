#ifndef PLC_GLOBAL_H
#define PLC_GLOBAL_H


#include <vdi/protocol_global.h>


#define GM_SAVEBOTTLE_HAND    (1)
#define GM_SAVEBOTTLE_AUTO    (2)
#define GM_GIVEBOTTLE_HAND     (3)
#define GM_GIVEBOTTLE_AUTO     (4)
#define GM_DROPBOTTLE_HAND     (5)
#define GM_DROPBOTTLE_AUTO     (6)

//Cabinet MaxRow : 柜子最大行数，行数从 1 开始
//Cabinet PerColumn : 每个柜子列数，列数从 1 开始
//Cabinet MaxFace : 柜子最大面数，面数从 1 开始
#define GM_Cabinet_MaxRow (16)
#define GM_Cabinet_PerColumn (7)
#define GM_Cabinet_MaxFace (2)

#define GM_PLC_NEEDRETRY (1)


#define GM_BOTTLE_FAIL  9
#define GM_BOTTLE_CANCEL  10
#define GM_BOTTLE_EMPTY  11
#define GM_BOTTLE_EXIST 12
#define GM_BOTTLE_MACHINE_FAIL  3

struct CoreObject
{
    std::string id;
    int boxNo;  //机柜
    int orderX; //x
    int orderY; //y
    int orderR; //r
    int x;   //x坐标
    int y;
    int r;
    int isUse; //使用标志
    int canUse;//是否能用
    int mode;  //模式
    bool flag;  //标志
    long long time;     //时间
    std::string rfidCode; //rfid
    std::string barCode;  //明码
    std::string pch;//批次号
    std::string type;  //类型

    CoreObject()
    {
        boxNo = 0;
        orderX = orderY = orderR = 0;
        x = y = r = 0;
        isUse = canUse = 0;
        mode = 0;
        flag = false;
        time = 0;
        id = rfidCode=barCode=pch=type="";
    }

};


#endif // PLC_GLOBAL_H
