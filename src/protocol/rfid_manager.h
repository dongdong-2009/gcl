#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H


#include <ccxx/ccxx.h>


class RfidManager
{  
public:
    //阻塞型在线等待读取Rfid
    static bool waitRfid(std::string &sRfid, std::string &barcode, msepoch_t iTimeOut = 2*1000); //10 seconds

    static bool waitRfidEx(std::string &sRfid, std::string &barcode, msepoch_t iTimeOut = 1000*20);
    //是否chengg
    static bool isSucced();
    //返回查询时间
    static long long query();

    //返回最后获取到RFID号的时间
    static long long lastTimeUpdateRfid();

    //返回最后获取到的RFID号
    static std::string lastRfid();

    static std::string lastBarcode();

    static std::string lastResponse();

};

#endif // RFID_MANAGER_H
