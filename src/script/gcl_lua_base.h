#ifndef GCL_LUA_BASE_H
#define GCL_LUA_BASE_H

#include <lua-5.3.4/lua.hpp>
#include <ccxx/ccxx.h>

class GclLuaBase
{
public:
    GclLuaBase();

public:
    static void stringToTable(lua_State * L,const std::string &data);

    static void mapToTable(lua_State * L, const std::map< std::string, std::string> &data);

    static std::string fromTable(lua_State * L,int idx);

};

#endif // GCL_LUA_BASE_H
