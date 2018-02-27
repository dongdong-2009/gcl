#include "gcl_lua_fs.h"
#include "gcl_lua_global.h"
#include <luatoolbox/lfs-1.6.3/lfs.c>

//extern int luaopen_lfs (lua_State *L);

GclLuaFS * fn_oGclLuaFS()
{
    GclLua::registInitFunction(GclLuaFS::registerLua);
    static GclLuaFS m;
    return &m;
}

static GclLuaFS * f_oGclLuaFS = fn_oGclLuaFS();

void GclLuaFS::registerLua()
{
    lua_State * L = GclLua::luaState();
    luaopen_lfs(L);
}
GclLuaFS::GclLuaFS()
{
}
