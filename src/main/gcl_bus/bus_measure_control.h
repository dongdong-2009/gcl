#ifndef BUS_MEASURE_CONTROL_H
#define BUS_MEASURE_CONTROL_H


#include <ccxx/ccxx.h>
#include "bus_strings.h"

#include <psm_struct.h>


class BusMeasureControl
{
public:
    static void start();

    static void stop();

    //在主线程中把 ChangedPacketData 发送出去
    //定时 30 ~ 50 毫秒
    static void timerCheckPublish(int iInterval);

    //目前处理遥控做了两件事
    //1、根据遥控所在的TerminalId转发走
    //2、Push给共享内存
    //send.yk measure=0x01000011;value=1;reason=2;strategy=3;method=4\nk11=v11;k12=v12\nk21=v21;k22=v22\n
    static void dealPsmMeasureControl(const std::string & sCommand, const char *pParam, int iParamLength, const PsmAttach *oAttach);

    static const std::string & getTableName() {
        return CS_TableName_Bus;
    }

    //xml : <terminal="0x01830101" measures="0x02000001~0x02000100">
    static void update(const std::map<std::string, std::string> & row);

    static std::vector<std::string> reportSelf();

};

#endif // BUS_MEASURE_CONTROL_H
