#ifndef GCL_LUA_NET_MONITOR_H
#define GCL_LUA_NET_MONITOR_H

#include "gcl_lua_base.h"

class GclNetMonitor:public GclLuaBase
{
public:
    static void registerLua();

public:
    GclNetMonitor();

};

#endif // GCL_LUA_NET_MONITOR_H

