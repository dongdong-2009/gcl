#ifndef GCL_LUA_RTTRIGGER
#define GCL_LUA_RTTRIGGER

#include "gcl_lua_global.h"

class GclLuaRTTRrigger:public GclLuaBase
{
public:
//    GclLuaRTTRrigger();

public:
    //处理实时变化
    static int dealChanged(const std::string sMeasureID);

};

#endif // GCL_LUA_RTTRIGGER

