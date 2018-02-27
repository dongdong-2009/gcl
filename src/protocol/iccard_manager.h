#ifndef ICCARD_MANAGER_H
#define ICCARD_MANAGER_H


#include <ccxx/ccxx.h>


//处理跟踪函数，读到新Iccard时回调通知 : iProcess = 1, iStep = 1, sMsg = Iccard
typedef void (*fn_iccard_process_trace_t)(int iProcess, int iStep, const std::string & sMsg, int iTag);


class IccardManager
{
public:
    //阻塞型在线等待用户刷卡
    static std::string waitIccard(msepoch_t iTimeOut = 3000LL);

    //返回最后获取到Iccard号的时间
    static long long lastTimeUpdateIccard();

    //返回最后获取到的Iccard号
    static std::string lastIccard();

    //注册过程处理跟踪函数指针
    static void registProcessTrace( fn_iccard_process_trace_t fn );

    //反注册过程处理跟踪函数指针
    static void unregistProcessTrace( fn_iccard_process_trace_t fn );

private:
    static void setLastIccard(const std::string & value);

    friend class IccardProtocol;

};

#endif // ICCARD_MANAGER_H
