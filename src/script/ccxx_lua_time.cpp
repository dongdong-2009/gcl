#include "ccxx_lua_time.h"

using namespace std;

CXLuaTime * fn_oCXLuaTime()
{
    GclLua::registInitFunction(CXLuaTime::registerLua);
    static CXLuaTime m;
    return &m;
}

static CXLuaTime * f_oCXLuaTime = fn_oCXLuaTime();

static int fn_getTimeString(lua_State *L)
{
    string sGetTimeString = CxTime::currentSystemTimeString(0,0,0);
    lua_pushstring(L, sGetTimeString.c_str());
    return 1;
}

static int fn_getCurrentTimeMS(lua_State *L)
{
    msepoch_t tmCurr = CxTime::currentSystemTime();
    lua_pushinteger(L, tmCurr);
    return 1;
}
static int fn_getCurrentTimeS(lua_State *L)
{
    sepoch_t tmCurr = CxTime::currentSepoch();
    lua_pushinteger(L, tmCurr);
    return 1;
}

void CXLuaTime::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "getTimeString", fn_getTimeString);
    LuaCommon::setTable(L, "getCurrentTimeMS", fn_getCurrentTimeMS);
    LuaCommon::setTable(L, "getCurrentTimeS", fn_getCurrentTimeS);
    lua_setglobal(L, "cx_time");    /* 'name' = table */
}

CXLuaTime::CXLuaTime()
{
}
