#include "gcl_lua_vxd.h"
#include "gcl_lua_global.h"


#include <vdi/terminal_manager.h>
#include <vdi/send_custom_data_cases.h>
#include <vdi/monsb_measure.h>
#include <vdi/ycadd_measure.h>
#include <vdi/straw_measure.h>

using namespace std;

GclLuaVxd * fn_oGclLuaVxd()
{
    GclLua::registInitFunction(GclLuaVxd::registerLua);
    static GclLuaVxd m;
    return &m;
}

static GclLuaVxd * f_oGclLuaVxd = fn_oGclLuaVxd();

//lua_function : gcs_vxd.send(int iTargetId, int iCommandId, const std::map<std::string, std::string> & sParams, string sDataString, int iSourceId)
//lua_function : gcs_vxd.send(int iTargetId, int iCommandId, const std::map<std::string, std::string> & sParams, int iSourceId)
//lua_function : gcs_vxd.send(int iTargetId, int iCommandId, string sDataString, int iSourceId)
//lua_function : gcs_vxd.send(int iTargetId, int iCommandId, int iSourceId)
//lua_function : gcs_vxd.send(int iTargetId, int iCommandId)
static int gcs_vxd_send(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 2) //最少2个参数
    {
        return 0;
    }

    int iTargetId = 0;
    int iCommandId = 0;
    std::map<std::string, std::string> sParams;
    string sDataString;
    int iSourceId = 0;

    LuaCommon::getValue(L, 1, iTargetId);
    LuaCommon::getValue(L, 2, iCommandId);

    if (top > 2) {
        int t3 = lua_type(L, 3);
        if (t3 == LUA_TTABLE)
        {
            lua_pushnil(L);
            while( lua_next(L,3) )
            {
                string sKey;
                string sValue;
                LuaCommon::getValue(L, -2, sKey);
                LuaCommon::getValue(L, -1, sValue);
                sParams[sKey] = sValue;
                lua_pop(L,1); // 弹出 value，让 key 留在栈顶
            }
        }
        else if (t3 == LUA_TSTRING) {
            LuaCommon::getValue(L, 3, sDataString);
        }
        else if (t3 == LUA_TNUMBER) {
            LuaCommon::getValue(L, 3, iSourceId);
        }
    }

    if (top > 3) {
        int t4 = lua_type(L, 4);
        if (t4 == LUA_TSTRING) {
            LuaCommon::getValue(L, 4, sDataString);
        }
        else if (t4 == LUA_TNUMBER) {
            LuaCommon::getValue(L, 4, iSourceId);
        }
    }

    if (top > 4) {
        int t5 = lua_type(L, 5);
        if (t5 == LUA_TNUMBER) {
            LuaCommon::getValue(L, 5, iSourceId);
        }
    }

    lua_pop(L, top);//清空
    /* return the number of results */
    int iResult = FALSE;

    if(iTargetId>0)
    {
        //找目标
        TerminalBase* oTerminal = TerminalManager::terminal(iTargetId);
        if(oTerminal!=NULL)
        {
            ProtocolBase* oProtocol = oTerminal->protocol();
            if(oProtocol!=NULL)
            {
                SendCustomDataCases * oCases = oProtocol->casesSon<SendCustomDataCases>();
                if (oCases)//处理LUA调用的数据
                {
                    iResult = oCases->sendCustomData(iSourceId, iCommandId, sParams, sDataString.size(), sDataString.data());
                }
            }
        }
    }
    else //循环找
    {
         const std::vector<TerminalBase *> *ot = TerminalManager::terminals();
         if(ot && ot->size()>0)
         {
             for(size_t i=0;i<ot->size();++i)
             {
                TerminalBase* oTerminal = ot->at(i);
                if(oTerminal!=NULL)
                {
                    ProtocolBase* oProtocol = oTerminal->protocol();
                    if(oProtocol!=NULL)
                    {
                        SendCustomDataCases * oCases = oProtocol->casesSon<SendCustomDataCases>();
                        if (oCases)//
                        {
                            iResult = oCases->sendCustomData(iSourceId, iCommandId, sParams, sDataString.size(), sDataString.data());
                            if(iResult>0)break;
                        }
                    }
                }
             }
         }
    }
    lua_pushinteger(L, iResult);

    return 1;
}
//同步发送
static int gcs_vxd_send_syn(lua_State *L)
{
    int top = lua_gettop(L);
    if (top < 2) //最少2个参数
    {
        return 0;
    }

    int iTargetId = 0;
    int iCommandId = 0;
    std::map<std::string, std::string> sParams;
    string sDataString;
    int iSourceId = 0;

    LuaCommon::getValue(L, 1, iTargetId);
    LuaCommon::getValue(L, 2, iCommandId);

    if (top > 2) {
        int t3 = lua_type(L, 3);
        if (t3 == LUA_TTABLE)
        {
            lua_pushnil(L);
            while( lua_next(L,3) )
            {
                string sKey;
                string sValue;
                LuaCommon::getValue(L, -2, sKey);
                LuaCommon::getValue(L, -1, sValue);
                sParams[sKey] = sValue;
                lua_pop(L,1); // 弹出 value，让 key 留在栈顶
            }
        }
        else if (t3 == LUA_TSTRING) {
            LuaCommon::getValue(L, 3, sDataString);
        }
        else if (t3 == LUA_TNUMBER) {
            LuaCommon::getValue(L, 3, iSourceId);
        }
    }

    if (top > 3) {
        int t4 = lua_type(L, 4);
        if (t4 == LUA_TSTRING) {
            LuaCommon::getValue(L, 4, sDataString);
        }
        else if (t4 == LUA_TNUMBER) {
            LuaCommon::getValue(L, 4, iSourceId);
        }
    }

    if (top > 4) {
        int t5 = lua_type(L, 5);
        if (t5 == LUA_TNUMBER) {
            LuaCommon::getValue(L, 5, iSourceId);
        }
    }

    lua_pop(L, top);//清空
    /* return the number of results */
    int iResult = FALSE;
    //返回结果
    map<string,string> mapData;
    if(iTargetId>0)
    {
        //找目标
        TerminalBase* oTerminal = TerminalManager::terminal(iTargetId);
        if(oTerminal!=NULL)
        {
            ProtocolBase* oProtocol = oTerminal->protocol();
            if(oProtocol!=NULL)
            {
                SendCustomDataCases * oCases = oProtocol->casesSon<SendCustomDataCases>();
                if (oCases)//处理LUA调用的数据
                {
                    iResult = oCases->sendCustomData_syn(iSourceId, iCommandId, sParams, sDataString.size(), sDataString.data(),mapData);
                }
            }
        }
    }
    else //循环找
    {
        const std::vector<TerminalBase *> *ot = TerminalManager::terminals();
        if(ot && ot->size()>0)
        {
            for(size_t i=0;i<ot->size();++i)
            {
                TerminalBase* oTerminal = ot->at(i);
                if(oTerminal!=NULL)
                {
                    ProtocolBase* oProtocol = oTerminal->protocol();
                    if(oProtocol!=NULL)
                    {
                        SendCustomDataCases * oCases = oProtocol->casesSon<SendCustomDataCases>();
                        if (oCases)//
                        {
                            iResult = oCases->sendCustomData_syn(iSourceId, iCommandId, sParams, sDataString.size(), sDataString.data(),mapData);
                            if(iResult>0)break;
                        }
                    }
                }
            }
        }
    }


//返回结果
    lua_pop(L,top);//清空
    lua_newtable(L);
    for (map<string, string>::const_iterator it = mapData.begin(); it != mapData.end(); ++it)
    {
        LuaCommon::setTable(L, it->first,it->second);
    }
    return 1;
}



void GclLuaVxd::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "send", gcs_vxd_send); //缺省异步发送
    LuaCommon::setTable(L, "send_syn", gcs_vxd_send_syn);
    LuaCommon::setTable(L, "send_asy", gcs_vxd_send);
    lua_setglobal(L, "gcs_vxd");    /* 'name' = table */
}

int GclLuaVxd::dealCustomData(int iSourceId, int iCommandId, const std::map<std::string, std::string> & sParams, int iDataLength, const char * pData, int iTargetId)
{
    int iResult = -1;

    lua_State * L = GclLua::luaState();
    if(L==NULL) return false;

    //获取全局函数
    int iLuaType = lua_getglobal(L, "gcs_deal_vxd_data");
    if (iLuaType == LUA_TFUNCTION)
    {
//        lua function : function gcs_deal_vxd_data(iSourceId, iTypeId, sParams, sDataString, iTargetId)
        //参数压栈
        lua_pushinteger(L, iSourceId);
        lua_pushinteger(L, iCommandId);
        lua_newtable(L);
        for (std::map<std::string, std::string>::const_iterator it = sParams.begin(); it != sParams.end(); ++it)
        {
            LuaCommon::setTable(L, it->first, it->second);
        }
        lua_pushlstring(L, pData, iDataLength);

        lua_pushinteger(L, iTargetId);
        //函数调用
        if(LuaCommon::callLuaFunction(L,5,1,iResult))
        {
            return iResult;
        }
    }
    return iResult;
}
