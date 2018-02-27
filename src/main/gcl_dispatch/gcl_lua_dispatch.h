#ifndef GCL_LUA_CONTROL_H
#define GCL_LUA_CONTROL_H


#include <script/gcl_lua_global.h>

#include <vdi/measure_sharememory_all.h>


class GclLuaDispatch
{
public:
    static void registerLuaDispatch();

    static void start();

    static void stop();

    static void measureShareMemoryAfterLoad(int iMeasureType);

    static void measureShareMemoryBeforeClose(int iMeasureType);

    //定时调度控制数据，建议定时为 100 毫秒
    static void timerTimeout(int iInterval);

//private:
//    static void dispatchMonsbControls(const MonsbShareMemory::ChangedData * const oChangedBegin, const typename MonsbShareMemory::ChangedData * const oChangedEnd);

//    static void dispatchYcaddControls(const YcaddShareMemory::ChangedData * const oChangedBegin, const typename YcaddShareMemory::ChangedData * const oChangedEnd);

//    static void dispatchStrawControls(const StrawShareMemory::ChangedData * const oChangedBegin, const typename StrawShareMemory::ChangedData * const oChangedEnd);

};

#endif // GCL_LUA_CONTROL_H
