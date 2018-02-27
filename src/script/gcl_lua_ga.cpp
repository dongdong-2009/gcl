#include "gcl_lua_ga.h"

#include <vdi/psm_manager.h>


#include "gcl_lua_global.h"
#include <ccxx/ccxx.h>

using namespace std;

GclLuaGA * fn_oGclLuaGA()
{
    GeneralAccess::registerMessageCommandDeal(GclLuaGA::dealMessage);
    GeneralAccess::registerFileWriteDeal(GclLuaGA::dealFileWrite);
    GeneralAccess::registerResultFileWriteDeal(GclLuaGA::dealResultFileWrite);
    GclLua::registInitFunction(GclLuaGA::registerLua);
    static GclLuaGA m;
    return &m;
}

static GclLuaGA * f_oGclLuaGA = fn_oGclLuaGA();

//*LUA 脚本可调用的全局函数
//gcl_psm.postMessageCommand(sCommand, sParam, iLocalTerminalId, iRemoteTerminalId)
//gcl_psm.postMessageCommand(sCommand, iLocalTerminalId, iRemoteTerminalId)
static int ga_postMessageCommand(lua_State *L)
{
    int iTop = lua_gettop(L);
    if(iTop<3) return 0;

    string sCommand="";
    string sParam="";
    int iLocalTerminalId = 0;
    int iRemoteTerminalId = 0;

    sCommand = f_oGclLuaGA->fromTable(L,1);

    if(iTop == 3)
    {
        LuaCommon::getValue(L, 2, iLocalTerminalId);
        LuaCommon::getValue(L, 3, iRemoteTerminalId);
    }
    else if(iTop >= 4)
    {
        sParam = f_oGclLuaGA->fromTable(L,2);
        LuaCommon::getValue(L, 3, iLocalTerminalId);
        LuaCommon::getValue(L, 4, iRemoteTerminalId);
    }

    int iSend = 0;
    if (sCommand.size() > 0)
    {
        if(iLocalTerminalId>0)
        {
            PsmAttach o(0,0,iLocalTerminalId,iRemoteTerminalId,0);
            iSend = GeneralAccess::postMessage(sCommand.c_str(),sParam.c_str(),sParam.length(),&o);
        }
        else
        {
            iSend = GeneralAccess::postMessage(sCommand.c_str(),sParam.c_str(),sParam.length());
        }
    }

    lua_pushinteger(L, iSend);
    return 1;
}

//gcl_psm.postFile(sFilePath, iRemoteTerminalId)
static int ga_postFile(lua_State *L)
{
    int iTop = lua_gettop(L);

    string sFilePath;
    int iLocalTerminalId = 0;
    int iRemoteTerminalId = 0;

    if(iTop >= 3)
    {
        LuaCommon::getValue(L, 1, sFilePath);
        LuaCommon::getValue(L, 2, iLocalTerminalId);
        LuaCommon::getValue(L, 3, iRemoteTerminalId);
    }

    int iSend = 0;
    if (sFilePath.size() > 0 )
    {
        if(iLocalTerminalId>0)
        {
            PsmAttach o(0,0,iLocalTerminalId,iRemoteTerminalId,0);
            iSend = GeneralAccess::postFile(sFilePath,&o);
        }
        else
        {
            iSend = GeneralAccess::postFile(sFilePath);
        }
    }
    lua_pushinteger(L, iSend);
    return 1;
}
static int ga_postFileByCommand(lua_State *L)
{
    int iTop = lua_gettop(L);

    string sFilePath;
    string sCommand1;
    int split;
    int iLocalTerminalId = 0;
    int iRemoteTerminalId = 0;

    if(iTop >= 5)
    {
        LuaCommon::getValue(L, 1, sFilePath);
        LuaCommon::getValue(L, 2, sCommand1);
        LuaCommon::getValue(L, 3, split);
        LuaCommon::getValue(L, 4, iLocalTerminalId);
        LuaCommon::getValue(L, 5, iRemoteTerminalId);
    }

    if(split<=0 || split>=3500)   split = 3500;

    std::vector<std::string> _sendFileTexts;

    CxFile::load(sFilePath,_sendFileTexts,split);

    int iSend = 0;
    if(_sendFileTexts.size()>0)
    {
        for(size_t i=0;i<_sendFileTexts.size();++i)
        {
            string sCommand = sCommand1 + CxString::format(";count=%d;index=%d",_sendFileTexts.size(),i+1);
            string sParam =  _sendFileTexts.at(i);
            //            if (sCommand.size() > 0 && iLocalTerminalId > 0 && iRemoteTerminalId > 0)
            if (sCommand.size() > 0)
            {
                if(iLocalTerminalId > 0)
                {
                    PsmAttach o(0,0,iLocalTerminalId,iRemoteTerminalId,0);
                    iSend += GeneralAccess::postMessage(sCommand.c_str(),sParam.c_str(),sParam.length(),&o);
                }
                else
                {
                    iSend += GeneralAccess::postMessage(sCommand.c_str(),sParam.c_str(),sParam.length());
                }
            }
        }
    }
    lua_pushinteger(L, iSend);
    return 1;
}

//传入参数:v,q,t的对象数组；
//传出参数,成功数目
static int ga_postRealtimeData(lua_State *L)
{
    int iResult = FALSE;

    int top = lua_gettop(L);
    if (top < 1)
    {
        return iResult;
    }

    vector<gcd_address_int32_vqt_t> yxs;
    gcd_address_int32_vqt_t yx;
    vector<gcd_address_double_vqt_t> ycs;
    gcd_address_double_vqt_t yc;
    vector<gcd_address_vqt_string128_t> yws;
    gcd_address_vqt_string128_t yw;

    int t = lua_type(L, 1);
    if (t == LUA_TTABLE)
    {
        size_t iCount = lua_rawlen(L, 1);
        int id;
        for (size_t i = 1; i <= iCount; ++i)
        {
            t = lua_rawgeti(L, 1, i);

            if (t == LUA_TTABLE)
            {
                LuaCommon::getTable(L, CS_Lua_Measure_ID, id, 2);
                switch(g_fn_getMeasureType(id))
                {
                    case MeasureType_Monsb:
                    {
                        yx.address = id;
                        LuaCommon::getTable(L, CS_Lua_Measure_V, yx.value, 2);
                        LuaCommon::getTable(L, CS_Lua_Measure_Q, yx.quality, 2);
                        LuaCommon::getTable(L, CS_Lua_Measure_T, yx.datetime, 2);
                        yxs.push_back(yx);
                    }
                        break;
                    case MeasureType_Ycadd:
                    {
                        yc.address = id;
                        LuaCommon::getTable(L, CS_Lua_Measure_V, yc.value, 2);
                        LuaCommon::getTable(L, CS_Lua_Measure_Q, yc.quality, 2);
                        LuaCommon::getTable(L, CS_Lua_Measure_T, yc.datetime, 2);
                        ycs.push_back(yc);
                    }
                        break;
                    case MeasureType_Straw:
                    {
                        yw.address = id;
                        string sValue;
                        LuaCommon::getTable(L, CS_Lua_Measure_V, sValue, 2);
                        if (sValue.size()<128)
                        {
                            memcpy(yw.value, sValue.data(), sValue.size());
                            yw.value[sValue.size()] = 0;
                        }
                        LuaCommon::getTable(L, CS_Lua_Measure_Q, yw.quality, 2);
                        LuaCommon::getTable(L, CS_Lua_Measure_T, yw.datetime, 2);
                        yws.push_back(yw);
                    }
                        break;
                    default:
                        break;
                }
            }
            lua_settop(L, 1);
        }
    }
    lua_pop(L,top);//清空
    /* return the number of results */
    iResult=0;
    int iSendBytes=-1;
    bool bErr = false;
    if (yxs.size()>0)
    {
        if(!bErr)
        {
            iSendBytes = GeneralAccess::postRealtimeDatas(yxs);
            if(iSendBytes>0)iResult += iSendBytes;
            else bErr = true;
        }
    }

    if (ycs.size()>0)
    {
        if(!bErr)
        {
            iSendBytes = GeneralAccess::postRealtimeDatas(ycs);
            if(iSendBytes>0)iResult += iSendBytes;
            else bErr = true;
        }
    }
    if (yws.size()>0)
    {
        if(!bErr)
        {
            iSendBytes = GeneralAccess::postRealtimeDatas(yws);
            if(iSendBytes>0)iResult += iSendBytes;
            else bErr = true;
        }
    }
    if(bErr) iResult = -1;

    lua_pushinteger(L,iResult);
    return TRUE;
}

void GclLuaGA::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "postMessageCommand", ga_postMessageCommand);
    LuaCommon::setTable(L, "postFile", ga_postFile);
    LuaCommon::setTable(L, "postFileByCommand", ga_postFileByCommand);
    LuaCommon::setTable(L, "postRealtimeData", ga_postRealtimeData);
    lua_setglobal(L, "gcs_ga");    /* 'name' = table */
}

GclLuaGA::GclLuaGA()
{
}


int GclLuaGA::dealMessage(const PsmExplainer *oPsm, const string &sCommand, const string &sParam, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    int iResult = FALSE;


    if(sCommand.find('=')==string::npos)
    {
        int iLocalTerminalId = oAttach->targetId;
        int iRemoteTerminalId = oAttach->sourceId;

        lua_State * L = GclLua::luaState();
        //获取全局函数
        int iLuaType = lua_getglobal(L, "gcs_ga_deal_receivedMessage");
        if (iLuaType == LUA_TFUNCTION)
        {
            //     lua function : function gcs_ga_deal_receivedMessage(iLocalTerminalId, iRemoteTerminalId, sCommand, sParam)
            //      参数压栈
            lua_pushinteger(L, iLocalTerminalId);
            lua_pushinteger(L, iRemoteTerminalId);
            lua_pushstring(L, sCommand.c_str());
            //sParam json文件格式
            if(CxString::contain(sCommand,".json"))
            {
                lua_pushstring(L, sParam.c_str());
            }
            else//sParam 其他格式
            {
                stringToTable(L,sParam);
            }
            LuaCommon::callLuaFunction(L, 4, 1, iResult);
        }
    }
    else
    {
        int iLocalTerminalId = oAttach->targetId;
        int iRemoteTerminalId = oAttach->sourceId;

        lua_State * L = GclLua::luaState();
        //获取全局函数
        int iLuaType = lua_getglobal(L, "gcs_ga_deal_receivedMessage_v2");
        if (iLuaType == LUA_TFUNCTION)
        {
            //     lua function : function gcs_ga_deal_receivedMessage(iLocalTerminalId, iRemoteTerminalId, sCommand, sParam)
            //      参数压栈
            lua_pushinteger(L, iLocalTerminalId);
            lua_pushinteger(L, iRemoteTerminalId);
            //3
            stringToTable(L,sCommand);
            //4
            if(CxString::contain(sCommand,"send.file"))
            {
                lua_pushstring(L, sParam.c_str());
            }
            else//sParam 其他格式
            {
                stringToTable(L,sParam);
            }


            LuaCommon::callLuaFunction(L, 4, 1, iResult);
        }
    }
    return iResult;
}

msepoch_t f_dtLuaGa = CxTime::currentMsepoch();

int GclLuaGA::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

//    return 0;
//    if (CxTime::milliSecondDifferToNow(f_dtLuaGa) < 20000)
//        return 0;

    int iResult = FALSE;

    int iLocalTerminalId = oAttach->targetId;
    int iRemoteTerminalId = oAttach->sourceId;

    lua_State * L = GclLua::luaState();
    if (pathInfo.path.size()>0 && fileData.empty())
    {
        string sFilePath = pathInfo.filePath();
        int iLuaType = lua_getglobal(L, "gcs_ga_deal_receivedFile");
        if (iLuaType == LUA_TFUNCTION)
        {
            //        lua function : function gcs_ga_deal_receivedFile(iLocalTerminalId, iLocalTerminalId, sFileName, sSavedFilePath, iSavedSize)
            //        参数压栈
            lua_pushinteger(L, iLocalTerminalId);
            lua_pushinteger(L, iRemoteTerminalId);
            lua_pushstring(L, pathInfo.fileName.c_str());
            lua_pushstring(L, sFilePath.c_str());
            lua_pushinteger(L, pathInfo.fileSize);

            LuaCommon::callLuaFunction(L, 5, 1,iResult);
        }
    }
    else
    {
        //获取全局函数
        int iLuaType = lua_getglobal(L, "gcs_ga_deal_receivedFileInfo");
        if (iLuaType == LUA_TFUNCTION)
        {
            //       lua function : function gcs_ga_deal_receivedFileInfo(iLocalTerminalId, iLocalTerminalId, sFileName, iFileType, iFileSize, dtLastWriteTime)
            //        参数压栈
            lua_pushinteger(L, iLocalTerminalId);
            lua_pushinteger(L, iRemoteTerminalId);
            lua_pushstring(L, pathInfo.fileName.c_str());
            lua_pushinteger(L, pathInfo.pathType);
            lua_pushinteger(L, pathInfo.fileSize);
            lua_pushinteger(L, pathInfo.lastWriteTime);
            string sFilePath;
            if (LuaCommon::callLuaFunction(L, 6, 1, sFilePath) && sFilePath.size()>0)
            {
                int iSaveSize = CxFile::save(sFilePath, fileData, "");
                int iLuaType = lua_getglobal(L, "gcs_ga_deal_receivedFile");
                if (iLuaType == LUA_TFUNCTION)
                {
                    //        lua function : function gcs_ga_deal_receivedFile(iLocalTerminalId, iLocalTerminalId, sFileName, sSavedFilePath, iSavedSize)
                    //        参数压栈
                    lua_pushinteger(L, iLocalTerminalId);
                    lua_pushinteger(L, iRemoteTerminalId);
                    lua_pushstring(L, pathInfo.fileName.c_str());
                    lua_pushstring(L, sFilePath.c_str());
                    lua_pushinteger(L, iSaveSize);
                    LuaCommon::callLuaFunction(L, 5, 1,iResult);
                }
            }
            else
            {
                GclScript::outScriptPrompt() << "sFilePath=" << sFilePath;
            }

        }
    }
    return iResult;
}

int GclLuaGA::dealResultFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    int iResult = FALSE;

    int iLocalTerminalId = oAttach->targetId;
    int iRemoteTerminalId = oAttach->sourceId;

    lua_State * L = GclLua::luaState();
    string sFilePath = pathInfo.filePath();
    int iLuaType = lua_getglobal(L, "gcs_ga_deal_resultFile");
    if (iLuaType == LUA_TFUNCTION)
    {
//        lua function : function gcs_ga_deal_receivedFile(iLocalTerminalId, iLocalTerminalId, sFilePath, iFileSize)
//        参数压栈
        lua_pushinteger(L, iLocalTerminalId);
        lua_pushinteger(L, iRemoteTerminalId);
        lua_pushstring(L, sFilePath.c_str());
        lua_pushinteger(L, pathInfo.fileSize);

        LuaCommon::callLuaFunction(L, 4, 1,iResult);
    }

    return iResult;
}
