#ifndef BUS_MEASURE_PUBLISH_H
#define BUS_MEASURE_PUBLISH_H


#include <ccxx/ccxx.h>
#include "bus_strings.h"
#include <vdi/update_template.h>


#define ci_track_bus_publish (0x01010001)


class BusMeasurePublisher
{
public:
    static void start();

    static void stop();

    //在主线程中把 ChangedPacketData 发送出去
    //定时 30 ~ 50 毫秒
    static void timerCheckPublish(int iInterval);

    static const std::string & getTableName() {
        return CS_TableName_Bus;
    }

    //xml : <terminal="0x01830101" measures="0x02000001~0x02000100">
    static void update(const std::map<std::string, std::string> & row);

    static std::vector<std::string> reportSelf();

};


#endif // BUS_MEASURE_PUBLISH_H
