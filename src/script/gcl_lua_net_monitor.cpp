#include "gcl_lua_net_monitor.h"
#include "gcl_lua_global.h"
#include <ccxx/cxsocket.h>

using namespace std;

GclNetMonitor * fn_oGclLuaNetMonitor()
{
    GclLua::registInitFunction(GclNetMonitor::registerLua);
    static GclNetMonitor m;
    return &m;
}

static GclNetMonitor * f_oGclLuaNetMonitor = fn_oGclLuaNetMonitor();

//*LUA 脚本可调用的全局函数
static int GclPingIP(lua_State *L)
{
    int iResult = 0;
    int n = lua_gettop(L);

    string sAddr;
    bool bOk = true;
    if (n >= 1)
    {
        bOk = LuaCommon::getValue(L, 1, sAddr);
    }
    lua_pop(L,n);

    if (bOk == false)
    {
        return iResult;
    }

    if (sAddr.size() > 0)
    {
        msepoch_t epoch = CxTime::currentMsepoch();
        iResult = CxNetwork::ping(sAddr);
        cxDebug()<<"GclPingIP: cost_time="<<CxTime::milliSecondDifferToNow(epoch)<<",result="<<iResult;
        lua_pushinteger(L, iResult);
    }

    return iResult;
}

//*LUA 脚本可调用的全局函数
void GclNetMonitor::registerLua()
{
    lua_State * L = GclLua::luaState();
    lua_newtable(L);
    LuaCommon::setTable(L, "PingIP", GclPingIP);
    lua_setglobal(L, "gcs_net_monitor");
}

GclNetMonitor::GclNetMonitor()
{

}
