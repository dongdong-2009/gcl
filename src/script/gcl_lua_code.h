#ifndef GCL_LUA_CODE_H
#define GCL_LUA_CODE_H

#include "gcl_lua_global.h"

const std::string UTF8 = "UTF8";
const std::string GBK = "GBK";

class GclLuaCode
{
public:
    static void registerLua();

public:
    GclLuaCode();
};

#endif // GCL_LUA_CODE_H
