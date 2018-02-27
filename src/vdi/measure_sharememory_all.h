#ifndef MEASURE_SHAREMEMORY_ALL
#define MEASURE_SHAREMEMORY_ALL


#include "monsb_sharememory.h"
#include "ycadd_sharememory.h"
#include "straw_sharememory.h"


#include <psm_struct.h>


//共享内存（只读方式）
class MeasureShareMemoryAll
{
public:
    //定时尝试打开共享内存（只读方式），打开后回调 AfterLoadCallback
    static void open(bool bCanWrite = false, int iCheckInterval = 200);

    static void setAfterLoadCallback(fn_void_int_t fn);

    static void setBeforeCloseCallback(fn_void_int_t fn);

    //关闭共享内存（只读方式），打开后回调 AfterLoadCallback
    static void close();

public:
    static void timerCheckSharememory(int iInterval);

    static void dealRealtimeDataPost(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach);

};


#endif // MEASURE_SHAREMEMORY_ALL

