#include "gcl_lua_dispatch.h"




using namespace std;


size_t f_monsbControlIndex = 0;
size_t f_ycaddControlIndex = 0;
size_t f_strawControlIndex = 0;


map<int, MonsbControlData> f_monsbControlDatas;
map<int, YcaddControlData> f_ycaddControlDatas;
map<int, StrawControlData> f_strawControlDatas;


GclLuaDispatch * fn_oGclLuaDispatch()
{
    GclLua::registInitFunction(GclLuaDispatch::registerLuaDispatch);

    CxApplication::registStartFunction(GclLuaDispatch::start);
    CxApplication::registStopFunction(GclLuaDispatch::stop);

    static GclLuaDispatch m;
    return &m;
}


static GclLuaDispatch * f_oGclLuaDispatch = fn_oGclLuaDispatch();



void GclLuaDispatch::registerLuaDispatch()
{
    return;
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
//    LuaCommon::setTable(L, "postMessageCommand", psm_postMessageCommand);
    lua_setglobal(L, "gcs_dispatch");    /* 'name' = table */
}

void GclLuaDispatch::measureShareMemoryAfterLoad(int iMeasureType)
{
    switch (iMeasureType)
    {
    case MeasureType_Monsb:
        f_monsbControlIndex = MonsbShareMemory::memoryManager()->controlCount()-1;
        break;
    case MeasureType_Ycadd:
        f_ycaddControlIndex = YcaddShareMemory::memoryManager()->controlCount()-1;
        break;
    case MeasureType_Straw:
        f_strawControlIndex = StrawShareMemory::memoryManager()->controlCount()-1;
        break;
    default:
        break;
    }
}

void GclLuaDispatch::measureShareMemoryBeforeClose(int iMeasureType)
{

}

void GclLuaDispatch::start()
{
    CxTimerManager::startTimer(GclLuaDispatch::timerTimeout, 100);
}

void GclLuaDispatch::stop()
{
    CxTimerManager::stopTimer(GclLuaDispatch::timerTimeout);
}


template<typename T>
void fn_dispatchControls_impl(const typename MeasureShareMemory<T>::ControlData * const oControlBegin, const typename MeasureShareMemory<T>::ControlData * const oControlEnd, map<int, typename MeasureShareMemory<T>::ControlData> & localControlDatas)
{
    const typename MeasureShareMemory<T>::ControlData * oControlData = oControlBegin;
    while (oControlData < oControlEnd)
    {
        localControlDatas[oControlData->measureId] = * oControlData;
        oControlData++;
    }
}


void fn_dispatchMonsbControls(const MonsbShareMemory::ControlData * const oControlBegin, const typename MonsbShareMemory::ControlData * const oControlEnd)
{
    fn_dispatchControls_impl<int>(oControlBegin, oControlEnd, f_monsbControlDatas);
}

void fn_dispatchYcaddControls(const YcaddShareMemory::ControlData * const oControlBegin, const typename YcaddShareMemory::ControlData * const oControlEnd)
{
    fn_dispatchControls_impl<double>(oControlBegin, oControlEnd, f_ycaddControlDatas);
}

void fn_dispatchStrawControls(const StrawShareMemory::ControlData * const oControlBegin, const typename StrawShareMemory::ControlData * const oControlEnd)
{
    fn_dispatchControls_impl<StrawValue>(oControlBegin, oControlEnd, f_strawControlDatas);
}

void GclLuaDispatch::timerTimeout(int iInterval)
{
    static int iTimes = 0;
    int iCase;
    iCase = iTimes % 10;
    if (iCase == 0)
    {
    }
    //*放到容器内
    else if (iCase == 1 || iCase == 7)
    {
        if (MonsbShareMemory::memoryManager()->isValid())
        {
            f_monsbControlIndex = MonsbShareMemory::memoryManager()->checkControls(f_monsbControlIndex, fn_dispatchMonsbControls);
        }
    }
    else if (iCase == 2 || iCase == 8)
    {
        if (YcaddShareMemory::memoryManager()->isValid())
        {
            f_ycaddControlIndex = YcaddShareMemory::memoryManager()->checkControls(f_ycaddControlIndex, fn_dispatchYcaddControls);
        }
    }
    else if (iCase == 3 || iCase == 9)
    {
        if (StrawShareMemory::memoryManager()->isValid())
        {
            f_strawControlIndex = StrawShareMemory::memoryManager()->checkControls(f_strawControlIndex, fn_dispatchStrawControls);
        }
    }
    //*给脚本处理去
    else if (iCase == 4)
    {
        if (MonsbShareMemory::memoryManager()->isValid())
        {
            lua_State * L = GclLua::luaState();
            //获取全局函数
            int iLuaType = lua_getglobal(L, "gcs_dispatch_deal_yk");
//        lua function : function gcs_dispatch_deal_yk(measureId, value, controlTime, controlTime, controlSourceId, controlReasonId, controlStrategyId, params, transferCount)
            if (iLuaType == LUA_TFUNCTION)
            {
                for(std::map<int, MonsbControlData>::const_iterator it = f_monsbControlDatas.begin(); it != f_monsbControlDatas.end(); ++it)
                {
                    const MonsbControlData & controlData = it->second;
//                    int controlId; // 控制ID : 1,2,3...0x7fffffff
//                    int measureId; // 量ID
//                    T value; // 控制值
//                    msepoch_t controlTime; // 值的控制时间
//                    int controlSourceId; // 控制的源ID
//                    int controlReasonId; // 控制的原因
//                    int controlStrategyId; // 控制的策略
//                    int controlMethodId; // 控制的方法
//                    char params[512];
//                    int transferCount; // 从BUS转发数量
//                    int res; // 保留

                    //        参数压栈
                    lua_pushinteger(L, controlData.controlId);
                    lua_pushinteger(L, controlData.value);
                    lua_pushinteger(L, controlData.controlTime);
                    lua_pushinteger(L, controlData.controlSourceId);
                    lua_pushinteger(L, controlData.controlReasonId);
                    lua_pushinteger(L, controlData.controlStrategyId);
                    lua_pushstring(L, controlData.params);
                    lua_pushinteger(L, controlData.transferCount);
//                    lua_pushlstring(L, controlData.params, strlen(controlData.params));
                    LuaCommon::callLuaFunction(L, 8, 0);
                }
            }
        }
    }
    else if (iCase == 5)
    {
        if (YcaddShareMemory::memoryManager()->isValid())
        {
            lua_State * L = GclLua::luaState();
            //获取全局函数
            int iLuaType = lua_getglobal(L, "gcs_dispatch_deal_yt");
//        lua function : function gcs_dispatch_deal_yt(measureId, value, controlTime, controlTime, controlSourceId, controlReasonId, controlStrategyId, params, transferCount)
            if (iLuaType == LUA_TFUNCTION)
            {
                for(std::map<int, YcaddControlData>::const_iterator it = f_ycaddControlDatas.begin(); it != f_ycaddControlDatas.end(); ++it)
                {
                    const YcaddControlData & controlData = it->second;
//                    int controlId; // 控制ID : 1,2,3...0x7fffffff
//                    int measureId; // 量ID
//                    T value; // 控制值
//                    msepoch_t controlTime; // 值的控制时间
//                    int controlSourceId; // 控制的源ID
//                    int controlReasonId; // 控制的原因
//                    int controlStrategyId; // 控制的策略
//                    int controlMethodId; // 控制的方法
//                    char params[512];
//                    int transferCount; // 从BUS转发数量
//                    int res; // 保留

                    //        参数压栈
                    lua_pushinteger(L, controlData.controlId);
                    lua_pushinteger(L, controlData.value);
                    lua_pushinteger(L, controlData.controlTime);
                    lua_pushinteger(L, controlData.controlSourceId);
                    lua_pushinteger(L, controlData.controlReasonId);
                    lua_pushinteger(L, controlData.controlStrategyId);
                    lua_pushstring(L, controlData.params);
                    lua_pushinteger(L, controlData.transferCount);
//                    lua_pushlstring(L, controlData.params, strlen(controlData.params));
                    LuaCommon::callLuaFunction(L, 8, 0);
                }
            }
        }
    }
    else if (iCase == 6)
    {
        if (StrawShareMemory::memoryManager()->isValid())
        {
            lua_State * L = GclLua::luaState();
            //获取全局函数
            int iLuaType = lua_getglobal(L, "gcs_dispatch_deal_ys");
//        lua function : function gcs_dispatch_deal_ys(measureId, value, controlTime, controlTime, controlSourceId, controlReasonId, controlStrategyId, params, transferCount)
            if (iLuaType == LUA_TFUNCTION)
            {
                for(std::map<int, StrawControlData>::const_iterator it = f_strawControlDatas.begin(); it != f_strawControlDatas.end(); ++it)
                {
                    const StrawControlData & controlData = it->second;
//                    int controlId; // 控制ID : 1,2,3...0x7fffffff
//                    int measureId; // 量ID
//                    T value; // 控制值
//                    msepoch_t controlTime; // 值的控制时间
//                    int controlSourceId; // 控制的源ID
//                    int controlReasonId; // 控制的原因
//                    int controlStrategyId; // 控制的策略
//                    int controlMethodId; // 控制的方法
//                    char params[512];
//                    int transferCount; // 从BUS转发数量
//                    int res; // 保留

                    //        参数压栈
                    lua_pushinteger(L, controlData.controlId);
                    lua_pushstring(L, controlData.value.value);
                    lua_pushinteger(L, controlData.controlTime);
                    lua_pushinteger(L, controlData.controlSourceId);
                    lua_pushinteger(L, controlData.controlReasonId);
                    lua_pushinteger(L, controlData.controlStrategyId);
                    lua_pushstring(L, controlData.params);
                    lua_pushinteger(L, controlData.transferCount);
//                    lua_pushlstring(L, controlData.params, strlen(controlData.params));
                    LuaCommon::callLuaFunction(L, 8, 0);
                }
            }
        }
    }
    ++iTimes;
}

