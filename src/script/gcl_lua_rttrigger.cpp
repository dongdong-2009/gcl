
#include "gcl_lua_rttrigger.h"



using namespace std;


static string CS_CHANGED_NAME = "changed_";


GclLuaRTTRrigger *fn_oGclLuaRTTrigger()
{
    static GclLuaRTTRrigger m;
    return &m;
}

static GclLuaRTTRrigger *f_oGclLuaRTTrigger = fn_oGclLuaRTTrigger();


int GclLuaRTTRrigger::dealChanged(const string sMeasureID)
{
    int iResult = FALSE;

    string sFunctionName = CS_CHANGED_NAME + sMeasureID;

    lua_State *L = GclLua::luaState();
    //
    int iLuaType = lua_getglobal(L, sFunctionName.c_str());
    if (LUA_TFUNCTION == iLuaType)
    {
        lua_pushstring(L, sMeasureID.c_str());
        LuaCommon::callLuaFunction(L, 1, 0, iResult);
    }

    return iResult;
}

