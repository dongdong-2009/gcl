#include "ccxx_lua_application.h"

using namespace std;

CXLuaApplication * fn_oCXLuaApplication()
{
    GclLua::registInitFunction(CXLuaApplication::registerLua);
    static CXLuaApplication m;
    return &m;
}

static CXLuaApplication * f_oCXLuaApplication = fn_oCXLuaApplication();

//*LUA 脚本可调用的全局函数
//local sApplicationPath = cx_fs.getApplicationPath()
static int fn_getApplicationPath(lua_State *L)
{
    string sApplicationPath = CxApplication::applicationPath();
    lua_pushstring(L, sApplicationPath.c_str());
    return 1;
}

static int fn_getApplicationFileName(lua_State *L)
{
    string sApplicationFileName = CxApplication::applicationFileName();
    lua_pushstring(L, sApplicationFileName.c_str());
    return 1;
}

static int fn_getApplicationTargetName(lua_State *L)
{
    string sApplicationTargetName = CxApplication::applicationTargetName();
    lua_pushstring(L, sApplicationTargetName.c_str());
    return 1;
}

static int fn_getApplicationConfig(lua_State *L)
{
    int n = lua_gettop(L);
    string sSection;
    string sKey;
    string tDefault;

    bool bOk = true;
    if (n>=1)
    {
        bOk = LuaCommon::getValue(L,1,sSection);
    }
    if (n>=2)
    {
        bOk = LuaCommon::getValue(L,2,sKey);
    }
    if (n>=3)
    {
        bOk = LuaCommon::getValue(L,3,tDefault);
    }

    if (!bOk) return false;
    string sApplicationConfig = CxApplication::findConfig(sSection, sKey, tDefault);
    lua_pushstring(L, sApplicationConfig.c_str());
    return 1;
}

void CXLuaApplication::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "getApplicationPath", fn_getApplicationPath);
    LuaCommon::setTable(L,"getApplicationConfig",fn_getApplicationConfig);
    LuaCommon::setTable(L,"getApplicationFileName",fn_getApplicationFileName);
    LuaCommon::setTable(L,"getApplicationTargetName",fn_getApplicationTargetName);
    lua_setglobal(L, "cx_application");    /* 'name' = table */
}

CXLuaApplication::CXLuaApplication()
{
}
