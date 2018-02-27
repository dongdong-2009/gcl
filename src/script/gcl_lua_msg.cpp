#include "gcl_lua_msg.h"
#include <gcrtdb/gcl_data_server.h>
#include "gcl_lua_global.h"


using namespace std;

GclLuaMsg * fn_oGclLuaMsg()
{
    GclLua::registInitFunction(GclLuaMsg::registerLua);
    static GclLuaMsg m;
    return &m;
}

static GclLuaMsg * f_oGclLuaMsg = fn_oGclLuaMsg();

//发送消息
static int gcs_msg_send(lua_State *L)
{
    int n = lua_gettop(L);

    string sCommand="";
    string sPrameter="";
    bool bRet =false;

    if(n==1)
    {
        bRet = LuaCommon::getValue(L, 1, sCommand);
     }

    if(n==2)
    {
        bRet = LuaCommon::getValue(L, 1, sCommand);
        bRet = LuaCommon::getValue(L, 2, sPrameter);
    }
    //清空
    lua_pop(L,lua_gettop(L));

    //发送消息
    if(bRet)
    {
        GCLDataServer::sendMeaaage(sCommand,sPrameter);
    }
    //压栈
    lua_pushboolean(L,bRet);
    return 1;
}


void GclLuaMsg::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "send", gcs_msg_send);
    lua_setglobal(L, "gcs_msg");    /* 'name' = table */
}

GclLuaMsg::GclLuaMsg()
{

}

bool GclLuaMsg::dealBuffer(const char *cmd, const char *pBuf, int len)
{
    int ret = 0;

//    string scriptName =  CxFileSystem::mergeFilePath(GclConfig::scriptPath(),"ics_msg.lua");

//    cxPrompt()<<CxTime::currentSepochString()<<scriptName<<cxEndLine;
//    ret = luaL_dofile(L, scriptName.data());
//    if( ret != 0 )
//    {
//        string sErr = CxString::format("lua_pcall failed:[%s]",lua_tostring(L,-1));

//        cxPrompt()<<CxTime::currentSepochString()<<sErr<<cxEndLine;
//        return false;
//    }


    lua_State * L = GclLua::luaState();
    if(L==NULL) return false;

    //获取全局函数
    int iLuaType = lua_getglobal(L, "gcl_msg_dealReceiveMsg");
    if (iLuaType == LUA_TFUNCTION)
    {
        //参数压栈
        lua_pushstring(L,cmd);
        lua_pushlstring(L, pBuf, len);
        //函数调用
        if(LuaCommon::callLuaFunction(L,2,1))
        {
            return true;
        }
    }
    return false;
}
