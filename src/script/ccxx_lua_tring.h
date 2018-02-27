#ifndef CCXX_LUA_TRING_H
#define CCXX_LUA_TRING_H

#include "gcl_lua_global.h"
#include "gcl_lua_base.h"

class CXLuaString:public GclLuaBase
{
public:
    static void registerLua();
public:
    CXLuaString();
};

#endif // CCXX_LUA_TRING_H
