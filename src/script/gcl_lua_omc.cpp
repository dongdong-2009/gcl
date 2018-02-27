#include "gcl_lua_omc.h"
#include "gcl_lua_global.h"
#include <OmcApi/Omc_NotifyAlarm.h>

using namespace std;

GclLuaOmc * fn_oGclLuaOmc()
{
    GclLua::registInitFunction(GclLuaOmc::registerLua);
    static GclLuaOmc m;
    return &m;
}

static GclLuaOmc * f_oGclLuaOmc = fn_oGclLuaOmc();

//*LUA 脚本可调用的全局函数
static int GclNotifyAlarm(lua_State *L)
{
    int iResult = 0;
    int n = lua_gettop(L);

    string sAddr;
    int nPort = 0;

    int nNeID = 0;
    int nAlarmType = 0;

    int nModuleNo = 0;
    int nCardNo = 0;
    int nPortNo = 0;

    int nAction = 1;

    bool bOk = true;
    if (n >= 1)
    {
        bOk = LuaCommon::getValue(L, 1, sAddr);
    }
    if (n >= 2)
    {
        bOk = LuaCommon::getValue(L, 2, nPort);
    }
    if (n >= 3)
    {
        bOk = LuaCommon::getValue(L, 3, nNeID);
    }
    if (n >= 4)
    {
        bOk = LuaCommon::getValue(L, 4, nAlarmType);
    }
    if (n >= 5)
    {
         bOk = LuaCommon::getValue(L, 5, nModuleNo);
    }
    if (n >= 6)
    {
        bOk = LuaCommon::getValue(L, 6, nCardNo);
    }
    if (n >= 7)
    {
        bOk = LuaCommon::getValue(L, 7, nPortNo);
    }
    if (n >= 8)
    {
        bOk = LuaCommon::getValue(L, 8, nAction);
    }

    lua_pop(L,n);

    if (bOk == false)
    {
        return iResult;
    }

    if ((n >= 4) && (sAddr.size() > 0) && (nPort > 0) && (nNeID > 0) && (nAlarmType > 0))
    {
        msepoch_t epoch = CxTime::currentMsepoch();
        iResult = NotifyAlarm(sAddr, nPort, nNeID, nAlarmType,  //neid=11 alarmvalue=2
                                  nModuleNo, nCardNo, nPortNo,  //nModuleNo=1 nCardNo=1 nPortNo=1
                                  nAction);
        cxDebug()<<"NotifyAlarm: cost_time="<<CxTime::milliSecondDifferToNow(epoch)<<",result="<<iResult;
        lua_pushinteger(L, iResult);
    }

    return iResult;
}

static int GclGetAlarmInfoMode1(lua_State *L)
{
    int iResult = 0;
    int n = lua_gettop(L);

    string sAddr;
    int nPort = 0;
    int nAlarmNo = 0;

    bool bOk = true;

    if (n >= 1)
    {
        bOk = LuaCommon::getValue(L, 1, sAddr);
    }
    if (n >= 2)
    {
        bOk = LuaCommon::getValue(L, 2, nPort);
    }
    if (n >= 3)
    {
        bOk = LuaCommon::getValue(L, 3, nAlarmNo);
    }

    lua_pop(L,n);

    if (bOk == false)
    {
        return 0;
    }

    if ((n == 3) && (sAddr.size() > 0) && (nPort > 0) && (nAlarmNo > 0))
    {
        iResult = 1;
        msepoch_t epoch = CxTime::currentMsepoch();
        TAlarmInfo * tAlarm = new TAlarmInfo();
        GetAlarmInfo(sAddr, nPort, 0, nAlarmNo, 0, 0, 0, tAlarm);
        if (tAlarm != NULL) {
            lua_newtable(L);
            lua_newtable(L);
            LuaCommon::setTable(L,CS_Lua_Succed,1);
            LuaCommon::setTable(L,CS_Lua_Count,15);
            LuaCommon::setTable(L,"m_nAlarmNo",tAlarm->m_nAlarmNo);
            LuaCommon::setTable(L,"m_nNeID",tAlarm->m_nNeID);
            LuaCommon::setTable(L,"m_nAlarmType",tAlarm->m_nAlarmType);
            LuaCommon::setTable(L,"m_nModuleNo",tAlarm->m_nModuleNo);
            LuaCommon::setTable(L,"m_nCardNo",tAlarm->m_nCardNo);
            LuaCommon::setTable(L,"m_nAlarmValue",tAlarm->m_nAlarmValue);
            LuaCommon::setTable(L,"m_nAlarmTime",tAlarm->m_nAlarmTime);
            LuaCommon::setTable(L,"m_nResumeTime",tAlarm->m_nResumeTime);
            LuaCommon::setTable(L,"m_nStatus",tAlarm->m_nStatus);
            LuaCommon::setTable(L,"m_nConfirm",tAlarm->m_nConfirm);
            LuaCommon::setTable(L,"m_nConfirmTime",tAlarm->m_nConfirmTime);
            LuaCommon::setTable(L,"m_sConfirmUser",tAlarm->m_sConfirmUser);
            LuaCommon::setTable(L,"m_nEliminate",tAlarm->m_nEliminate);
            LuaCommon::setTable(L,"m_nEliminateTime",tAlarm->m_nEliminateTime);
            LuaCommon::setTable(L,"m_sEliminateUser",tAlarm->m_sEliminateUser);
            cxDebug()<<"GetAlarmInfo: cost_time="<<CxTime::milliSecondDifferToNow(epoch)<<",result="<<iResult;
            delete tAlarm;
            tAlarm = NULL;
        }
        else
        {
            lua_newtable(L);
            LuaCommon::setTable(L,CS_Lua_Succed,0);
            LuaCommon::setTable(L,CS_Lua_Count,0);
        }
    }
    return iResult;
}

static int GclGetAlarmInfoMode2(lua_State *L)
{
    int iResult = 0;
    int n = lua_gettop(L);

    string sAddr;
    int nPort = 0;

    int nNeID = 0;
    int nAlarmType = 0;

    int nModuleNo = 0;
    int nCardNo = 0;
    int nPortNo = 0;

    bool bOk = true;
    if (n >= 1)
    {
        bOk = LuaCommon::getValue(L, 1, sAddr);
    }
    if (n >= 2)
    {
        bOk = LuaCommon::getValue(L, 2, nPort);
    }
    if (n >= 3)
    {
        bOk = LuaCommon::getValue(L, 3, nNeID);
    }
    if (n >= 4)
    {
        bOk = LuaCommon::getValue(L, 4, nAlarmType);
    }
    if (n >= 5)
    {
         bOk = LuaCommon::getValue(L, 5, nModuleNo);
    }
    if (n >= 6)
    {
        bOk = LuaCommon::getValue(L, 6, nCardNo);
    }
    if (n >= 7)
    {
        bOk = LuaCommon::getValue(L, 7, nPortNo);
    }

    lua_pop(L,n);

    if (bOk == false)
    {
        return iResult;
    }

    if ((n == 7) && (sAddr.size() > 0) && (nPort > 0) && (nNeID > 0) && (nAlarmType > 0))
    {
        iResult = 1;
        msepoch_t epoch = CxTime::currentMsepoch();
        TAlarmInfo * tAlarm = new TAlarmInfo();
        GetAlarmInfo(sAddr, nPort, nNeID, nAlarmType,
                     nModuleNo, nCardNo, nPortNo,
                     tAlarm);
        if (tAlarm != NULL) {
            lua_newtable(L);
            lua_newtable(L);
            LuaCommon::setTable(L,CS_Lua_Succed,1);
            LuaCommon::setTable(L,CS_Lua_Count,15);
            LuaCommon::setTable(L,"m_nAlarmNo",tAlarm->m_nAlarmNo);
            LuaCommon::setTable(L,"m_nNeID",tAlarm->m_nNeID);
            LuaCommon::setTable(L,"m_nAlarmType",tAlarm->m_nAlarmType);
            LuaCommon::setTable(L,"m_nModuleNo",tAlarm->m_nModuleNo);
            LuaCommon::setTable(L,"m_nCardNo",tAlarm->m_nCardNo);
            LuaCommon::setTable(L,"m_nAlarmValue",tAlarm->m_nAlarmValue);
            LuaCommon::setTable(L,"m_nAlarmTime",tAlarm->m_nAlarmTime);
            LuaCommon::setTable(L,"m_nResumeTime",tAlarm->m_nResumeTime);
            LuaCommon::setTable(L,"m_nStatus",tAlarm->m_nStatus);
            LuaCommon::setTable(L,"m_nConfirm",tAlarm->m_nConfirm);
            LuaCommon::setTable(L,"m_nConfirmTime",tAlarm->m_nConfirmTime);
            LuaCommon::setTable(L,"m_sConfirmUser",tAlarm->m_sConfirmUser);
            LuaCommon::setTable(L,"m_nEliminate",tAlarm->m_nEliminate);
            LuaCommon::setTable(L,"m_nEliminateTime",tAlarm->m_nEliminateTime);
            LuaCommon::setTable(L,"m_sEliminateUser",tAlarm->m_sEliminateUser);
            cxDebug()<<"GetAlarmInfo: cost_time="<<CxTime::milliSecondDifferToNow(epoch)<<",result="<<iResult;
            delete tAlarm;
            tAlarm = NULL;
        }
        else
        {
            lua_newtable(L);
            LuaCommon::setTable(L,CS_Lua_Succed,0);
            LuaCommon::setTable(L,CS_Lua_Count,0);
        }
    }
    return iResult;
}

void GclLuaOmc::registerLua()
{
    lua_State * L = GclLua::luaState();
    lua_newtable(L);
    LuaCommon::setTable(L, "NotifyAlarm", GclNotifyAlarm);
    LuaCommon::setTable(L, "GetAlarmInfoMode1", GclGetAlarmInfoMode1);
    LuaCommon::setTable(L, "GetAlarmInfoMode2", GclGetAlarmInfoMode2);
    lua_setglobal(L, "gcs_omc");
}

GclLuaOmc::GclLuaOmc()
{

}
