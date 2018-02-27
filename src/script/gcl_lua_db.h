#ifndef GCL_LUA_DB_H
#define GCL_LUA_DB_H

#include "gcl_lua_base.h"

class GclLuaDB:public GclLuaBase
{
public:
    static void registerLua();

public:
    GclLuaDB();

};

#endif // GCL_LUA_DB_H
