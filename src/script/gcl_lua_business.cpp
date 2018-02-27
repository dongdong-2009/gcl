#include "gcl_lua_business.h"
#include "gcl_lua_global.h"
#include "scriptproxy.h"

using namespace std;

GclLuaBusiness * fn_oGclLuaBusiness()
{
    GclLua::registInitFunction(GclLuaBusiness::registerLua);
    static GclLuaBusiness m;
    return &m;
}

static GclLuaBusiness * f_oGclLuaBusiness = fn_oGclLuaBusiness();



//lua_function : gcs_bi.send_v1(string sSender,const std::map<std::string, std::string> &sCommand;const std::map<std::string, std::string> & sParams;)
//lua_function : gcs_bi.send_v1(string sSender,string &sCommand;string &sParams;string &attach)
static int gcs_bi_send(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 2) //最少2个参数
    {
        return 0;
    }

    string sSender  = "";
    string sCommand = "";
    string sParam   = "";
    string sAttach  = "";

    LuaCommon::getValue(L, 1, sSender);
    switch(top)
    {
    case 2:
        sCommand = f_oGclLuaBusiness->fromTable(L,2);
        break;
    case 3:
        sCommand = f_oGclLuaBusiness->fromTable(L,2);
        sParam   = f_oGclLuaBusiness->fromTable(L,3);
        break;
    case 4:
        sCommand = f_oGclLuaBusiness->fromTable(L,2);
        sParam   = f_oGclLuaBusiness->fromTable(L,3);
        sAttach  = f_oGclLuaBusiness->fromTable(L,4);
        break;
    default:
        break;
    }

    lua_pop(L, top);//清空
//    //找目标
    ScriptProxyAttach attach;
    attach.fromString(sAttach);
    ScriptProxy::raiseReceivedMessageCommand(sCommand,sParam,sSender,&attach);

    lua_pushinteger(L, 1);
    return 1;
}



GclLuaBusiness::GclLuaBusiness()
{

}

void GclLuaBusiness::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "send", gcs_bi_send);
    lua_setglobal(L, "gcs_bi");    /* 'name' = table */
}

int GclLuaBusiness::sendMessage(const string &sSender,const string &sCommand, const string &sParam,const std::map<string, string> &ottach)
{
    int iResult = -1;

    lua_State * L = GclLua::luaState();
    if(sCommand.find('=')==string::npos)
    {
        //获取全局函数
        int iLuaType = lua_getglobal(L, "gcs_bi_deal_receivedMessage");
        if (iLuaType == LUA_TFUNCTION)
        {
//     lua function : function gcs_bi_deal_receivedMessage(sCommand, sParam)
            lua_pushstring(L, sSender.c_str());
            lua_pushstring(L, sCommand.c_str());
            stringToTable(L,sParam);
            mapToTable(L,ottach);
            LuaCommon::callLuaFunction(L,4, 1, iResult);
        }
    }
    else
    {
        lua_State * L = GclLua::luaState();
        //获取全局函数
        int iLuaType = lua_getglobal(L, "gcs_bi_deal_receivedMessage_v2");
        if (iLuaType == LUA_TFUNCTION)
        {
//     lua function : function gcs_bi_deal_receivedMessage_v2(sCommand, sParam)
           lua_pushstring(L, sSender.c_str());
           stringToTable(L,sCommand);
           stringToTable(L,sParam);
           mapToTable(L,ottach);
           LuaCommon::callLuaFunction(L, 4, 1, iResult);
        }
    }
    return iResult;
}
