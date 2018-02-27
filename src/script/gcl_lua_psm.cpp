#include "gcl_lua_psm.h"

#include <vdi/terminal_exe.h>
#include <vdi/terminal_manager.h>
#include <vdi/psm_manager.h>


#include "gcl_lua_global.h"


using namespace std;

GclLuaPsm * fn_oGclLuaPsm()
{
    TerminalExe::registAfterLoad(GclLuaPsm::registerPsmProtocolNotify);
    GclLua::registInitFunction(GclLuaPsm::registerLuaPsm);
    static GclLuaPsm m;
    return &m;
}


static GclLuaPsm * f_oGclLuaPsm = fn_oGclLuaPsm();
static string f_deal_message_lua_filePath;

void GclLuaPsm::registerPsmProtocolNotify()
{
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase* oTerminal = oTerminals->at(i);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
        {
            PsmProtocol* oPsmProtocol = (PsmProtocol*)oProtocol;
            oPsmProtocol->addObserver(f_oGclLuaPsm);
        }
    }
    f_deal_message_lua_filePath =  CxFileSystem::mergeFilePath(GclConfig::scriptPath(),"psm_deal_message.lua");
}

//*LUA 脚本可调用的全局函数
//gcl_psm.postMessageCommand(sCommand, sParam, iLocalTerminalId, iRemoteTerminalId)
//gcl_psm.postMessageCommand(sCommand, iLocalTerminalId, iRemoteTerminalId)
static int psm_postMessageCommand(lua_State *L)
{
    int iTop = lua_gettop(L);

    string sCommand;
    string sParam;
    int iLocalTerminalId = 0;
    int iRemoteTerminalId = 0;

    if(iTop == 3)
    {
        LuaCommon::getValue(L, 1, sCommand);
        LuaCommon::getValue(L, 2, iLocalTerminalId);
        LuaCommon::getValue(L, 3, iRemoteTerminalId);
    }
    else if(iTop >= 4)
    {
        LuaCommon::getValue(L, 1, sCommand);
        LuaCommon::getValue(L, 2, sParam);
        LuaCommon::getValue(L, 3, iLocalTerminalId);
        LuaCommon::getValue(L, 4, iRemoteTerminalId);
    }

    int iSend = 0;
    if (sCommand.size() > 0 && iLocalTerminalId > 0)
    {
        iSend = PsmManager::postMessageByTerminalId(iLocalTerminalId, iRemoteTerminalId, sCommand, sParam);
    }
    lua_pushinteger(L, iSend);
    return 1;
}

//gcl_psm.postFile(sFilePath, iRemoteTerminalId)
static int psm_postFile(lua_State *L)
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
    if (sFilePath.size() > 0 && iLocalTerminalId > 0)
    {
        iSend = PsmManager::postFileByTerminalId(iLocalTerminalId, iRemoteTerminalId, sFilePath);
    }
    lua_pushinteger(L, iSend);
    return 1;
}
//gcl_psm.psm_postFileByCommand(sFilePath,sCommand,split,iRemoteTerminalId)
static int psm_postFileByCommand(lua_State *L)
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
             if (sCommand.size() > 0 && iLocalTerminalId > 0 && iRemoteTerminalId > 0)
             {
                 iSend += PsmManager::postMessageByTerminalId(iLocalTerminalId, iRemoteTerminalId, sCommand, _sendFileTexts.at(i));
             }
        }
    }
    lua_pushinteger(L, iSend);
    return 1;
}

void GclLuaPsm::registerLuaPsm()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "postMessageCommand", psm_postMessageCommand);
    LuaCommon::setTable(L, "postFile", psm_postFile);
    LuaCommon::setTable(L, "postFileByCommand", psm_postFileByCommand);
    lua_setglobal(L, "gcs_psm");    /* 'name' = table */
}

void GclLuaPsm::received_message_command(const PsmProtocol * oPsm, const char *sCommand, const char *sParam, int iParamLength, const PsmAttach * oAttach)
{
//    cxPromptCheck(oAttach);
    int iLocalTerminalId = oAttach->targetId;
    int iRemoteTerminalId = oAttach->sourceId;
    TerminalBase * oTerminal = TerminalManager::terminal(iLocalTerminalId);
//    cxPromptCheck(oTerminal);

    lua_State * L = GclLua::luaState();
    //获取全局函数
    int iLuaType = lua_getglobal(L, "gcs_psm_dealReceivedMessage");
    if (iLuaType == LUA_TFUNCTION)
    {
//        lua function : function psm_dealReceivedMessage(iLocalTerminalId, iLocalTerminalId, sCommand, sParam)
//        参数压栈
        lua_pushinteger(L, iLocalTerminalId);
        lua_pushinteger(L, iRemoteTerminalId);
        lua_pushstring(L, sCommand);
        lua_pushlstring(L, sParam, iParamLength);
        LuaCommon::callLuaFunction(L, 4, 0);
    }
}

void GclLuaPsm::received_file_write(const PsmProtocol * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach)
{
//    cxPromptCheck(oAttach);
    int iLocalTerminalId = oAttach->targetId;
    int iRemoteTerminalId = oAttach->sourceId;
    TerminalBase * oTerminal = TerminalManager::terminal(iLocalTerminalId);
//    cxPromptCheck(oTerminal);

    lua_State * L = GclLua::luaState();
    if (pathInfo.path.size()>0 && fileData.empty())
    {
        string sFilePath = pathInfo.filePath();
        int iLuaType = lua_getglobal(L, "gcs_psm_dealReceivedFile");
        if (iLuaType == LUA_TFUNCTION)
        {
    //        lua function : function psm_dealReceivedFile(iLocalTerminalId, iLocalTerminalId, sFileName, sSavedFilePath, iSavedSize)
    //        参数压栈
            lua_pushinteger(L, iLocalTerminalId);
            lua_pushinteger(L, iLocalTerminalId);
            lua_pushstring(L, pathInfo.fileName.c_str());
            lua_pushstring(L, sFilePath.c_str());
            lua_pushinteger(L, pathInfo.fileSize);
            LuaCommon::callLuaFunction(L, 5, 0);
        }
    }
    else
    {
        //获取全局函数
        int iLuaType = lua_getglobal(L, "gcs_psm_dealReceivedFileInfo");
        if (iLuaType == LUA_TFUNCTION)
        {
    //        lua function : function psm_dealReceivedFileInfo(iLocalTerminalId, iLocalTerminalId, sFileName, iFileType, iFileSize, dtLastWriteTime)
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
                int iLuaType = lua_getglobal(L, "gcs_psm_dealReceivedFile");
                if (iLuaType == LUA_TFUNCTION)
                {
            //        lua function : function psm_dealReceivedFile(iLocalTerminalId, iLocalTerminalId, sFileName, sSavedFilePath, iSavedSize)
            //        参数压栈
                    lua_pushinteger(L, iLocalTerminalId);
                    lua_pushinteger(L, iLocalTerminalId);
                    lua_pushstring(L, pathInfo.fileName.c_str());
                    lua_pushstring(L, sFilePath.c_str());
                    lua_pushinteger(L, iSaveSize);
                    LuaCommon::callLuaFunction(L, 5, 0);
                }
            }
        }
    }
}

void GclLuaPsm::received_realtime_data_request(const PsmProtocol *oPsm, const PsmAttach * oAttach)
{
}

void GclLuaPsm::received_realtime_data_post(const PsmProtocol *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach * oAttach)
{
}
