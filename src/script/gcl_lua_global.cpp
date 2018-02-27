#include "gcl_lua_global.h"
#include <common/gcl_config.h>


using namespace std;


GclLua * fn_gclLuaInit()
{
    CxApplication::registStartFunction(GclLua::init, 15000);
    CxInterinfoIn::addObserver(GclLua::interinfoIn_line);
    CxInterinfoIn::addObserver(GclLua::interinfoIn_cmd);
    static GclLua m;
    return & m;
}


static lua_State * f_oLuaState = NULL;

static GclLua * f_oGclLua = fn_gclLuaInit();

static string f_deal_filePath;
static int f_iTimeOutTimes = 0;
static map<string, int> f_task_filePathTimeouts;
static map<string, CxFilePathInfo> f_deal_filePaths;
static map<string, CxFilePathInfo> f_common_filePaths;




std::vector<fn_void_t> * GclLua::getFnInits()
{
    static vector<fn_void_t> fnInits;
    return & fnInits;
}

std::vector<fn_void_pchar_t> * GclLua::getFnLuaFileLoadeds()
{
    static vector<fn_void_pchar_t> fnLuaFileLoadeds;
    return & fnLuaFileLoadeds;
}

int GclLua::beforeLoadLuaFile(const string &sFilePath)
{
    return 0;
}

int GclLua::afterLoadLuaFile(const string &sFilePath, int iResultLoad, const string &sResultString)
{
    // Fail: iResultLoad != 0
//    if (iResultLoad == 0)

        vector<fn_void_pchar_t> * oFnLuaFileLoadeds = getFnLuaFileLoadeds();
        for (size_t i = 0; i < oFnLuaFileLoadeds->size(); ++i)
        {
            fn_void_pchar_t fn = oFnLuaFileLoadeds->at(i);
            fn(sFilePath.c_str(), iResultLoad);
        }

    return 0;
}

void GclLua::registInitFunction(fn_void_t fn)
{
    vector<fn_void_t> * oFnInits = getFnInits();
    if (std::find(oFnInits->begin(), oFnInits->end(), fn) == oFnInits->end())
        oFnInits->push_back(fn);
}

void GclLua::init()
{
    f_deal_filePath = GclConfig::scriptPath();

    f_oLuaState = luaL_newstate();

    luaL_openlibs(f_oLuaState);

    vector<fn_void_t> * oFnInits = getFnInits();
    for (size_t i = 0; i < oFnInits->size(); ++i)
    {
        fn_void_t fn = oFnInits->at(i);
        fn();
    }

    loadLuaFiles(true, false);

    CxTimerManager::startTimer(GclLua::timer_timeout, 1000);
}

lua_State * GclLua::luaState()
{
    return f_oLuaState;
}

void GclLua::start()
{
    GclScript::outScriptPrompt() << CxTime::currentSepochString() << "GclLua::start().";
}

void GclLua::stop()
{
    int iResult = -1;
    lua_State * L = GclLua::luaState();
    if(L!=NULL)
    {
        //获取全局函数
        int iLuaType = lua_getglobal(L, "gcs_exit");
        if (iLuaType == LUA_TFUNCTION)
        {
            //        lua function : function gcs_exit()
            //函数调用
            LuaCommon::callLuaFunction(L,0,0,iResult);
        }
    }
    GclScript::outScriptPrompt() << CxTime::currentSepochString() << "GclLua::stop()."<<iResult;
}


void GclLua::update(const string &sFilePath)
{
    vector<map<string, string> > rows;
    CxXml::loadTable4Level(sFilePath, rows, "", "lua");
    update(rows);
}

void GclLua::update(const char *pData, int iLength)
{
    vector<map<string, string> > rows;
    CxXml::loadTable4Level(pData, iLength, rows, "", "lua");
    update(rows);
}

void GclLua::update(const std::vector<std::map<string, string> > &rows)
{
    for (size_t i = 0; i < rows.size(); ++i)
    {
        const map<string, string> & row = rows.at(i);

        update(row);
    }
}

void GclLua::update(const std::map<string, string> &row)
{
    int sourceTerminalId = 0;
    int iPort = 0;

    sourceTerminalId = CxContainer::valueTo(row, string("CS_SourceTerminalId"), sourceTerminalId);
    iPort = CxContainer::valueTo(row, string("port"), iPort);

    update( sourceTerminalId, iPort );
}

void GclLua::update(int ssid,int iPort)
{
    GM_INVALID_RETURE(ssid>0);
    GM_INVALID_RETURE(iPort>0);
}

std::vector<string> GclLua::reportSelf()
{
    vector<string> sReports;
    sReports.push_back("*** Gcl Lua Report ***");
    return sReports;
}

int GclLua::interinfoIn_line(const string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

int GclLua::interinfoIn_cmd(const string &sCommand, const std::map<string, string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    if (sCommand == "script")
    {
        return CxInterinfo::InDeal_Ed;
    }
    return 0;
}

void GclLua::timer_timeout(int iInterval)
{
    loadLuaFiles(true, true);
}

void GclLua::loadLuaFiles(bool bLocalCommon, bool bLocadTask)
{
    if (f_iTimeOutTimes % 3 == 0)
    {
        vector<CxFileSystem::PathInfo> pathInfos;
        CxFilePathInfo emptyPathInfo;
        CxFileSystem::scanDir(f_deal_filePath, pathInfos, false);
        vector<string> sLoadFilePaths;
        vector<string> sLoadInitFilePaths;
        for (size_t i = 0; i < pathInfos.size(); ++i)
        {
            const CxFileSystem::PathInfo & pathInfo = pathInfos.at(i);
            if (pathInfo.pathType != CxFileSystem::PathTypeFile) continue;
            const string & sFileName = pathInfo.fileName;
            if (bLocalCommon)
            {
                if (CxString::endWith(sFileName, ".init.lua"))
                {
                    const CxFilePathInfo & oldPathInfo = CxContainer::value(f_deal_filePaths, sFileName, emptyPathInfo);
                    if (pathInfo.lastWriteTime > oldPathInfo.lastWriteTime)
                    {
                        sLoadInitFilePaths.push_back(pathInfo.filePath());
                        f_deal_filePaths[sFileName] = pathInfo;
                    }
                    continue;
                }
                //C来调用LUA的公共
                else if (CxString::endWith(sFileName, ".common.lua"))
                {
                    const CxFilePathInfo & oldPathInfo = CxContainer::value(f_common_filePaths, sFileName, emptyPathInfo);
                    if (pathInfo.lastWriteTime > oldPathInfo.lastWriteTime)
                    {
                        sLoadFilePaths.push_back(pathInfo.filePath());
                        f_common_filePaths[sFileName] = pathInfo;
                    }
                    continue;
                }
            }
            if (bLocadTask)
            {
                //定时任务
                if (CxString::endWith(sFileName, "s.lua"))
                {
                    string sFileName2 = CxString::newString(sFileName);
                    string sTimeOut = CxString::tokenRight(sFileName2, 's');
                    sTimeOut = CxString::tokenRight(sFileName2, '.');
                    bool bOk;
                    int iTimeOut = CxString::toInt32(sTimeOut, &bOk);
                    if (bOk && iTimeOut>0)
                    {
                        f_task_filePathTimeouts[pathInfo.fileName] = iTimeOut;
                    }
                    continue;
                }
                //C来调用LUA的处理
                else if (CxString::endWith(sFileName, ".deal.lua"))
                {
                    const CxFilePathInfo & oldPathInfo = CxContainer::value(f_deal_filePaths, sFileName, emptyPathInfo);
                    if (pathInfo.lastWriteTime > oldPathInfo.lastWriteTime)
                    {
                        sLoadFilePaths.push_back(pathInfo.filePath());
                        f_deal_filePaths[sFileName] = pathInfo;
                    }
                    continue;
                }
            }
        }
        //init全局初始化 init.lua common.lua
        for (size_t i = 0; i < sLoadInitFilePaths.size(); ++i)
        {
            const string & sFilePath = sLoadInitFilePaths.at(i);
            lua_State * L = GclLua::luaState();
            beforeLoadLuaFile(sFilePath);
            int iResult = luaL_dofile(L, sFilePath.c_str());
            if( iResult != 0 )
            {
                string sError = CxString::format(" luaL_dofile[%s] failed : [%s]", sFilePath.c_str(), lua_tostring(L,-1));
                GclScript::outScriptPrompt() << CxTime::currentSepochString() << sError;
                afterLoadLuaFile(sFilePath, iResult, sError);
            }
            else
            {
                string sInfo = CxString::format(" luaL_dofile[%s] success !!!", sFilePath.c_str());
                GclScript::outScriptPrompt() << CxTime::currentSepochString() << sInfo;
                afterLoadLuaFile(sFilePath, iResult, sInfo);
            }
        }
        //其他回調加載
        for (size_t i = 0; i < sLoadFilePaths.size(); ++i)
        {
            const string & sFilePath = sLoadFilePaths.at(i);
            lua_State * L = GclLua::luaState();
            beforeLoadLuaFile(sFilePath);
            int iResult = luaL_dofile(L, sFilePath.c_str());
            if( iResult != 0 )
            {
                string sError = CxString::format(" luaL_dofile[%s] failed : [%s]", sFilePath.c_str(), lua_tostring(L,-1));
                GclScript::outScriptPrompt() << CxTime::currentSepochString() << sError;
                afterLoadLuaFile(sFilePath, iResult, sError);
            }
            else
            {
                string sInfo = CxString::format(" luaL_dofile[%s] success !!!", sFilePath.c_str());
                GclScript::outScriptPrompt() << CxTime::currentSepochString() << sInfo;
                afterLoadLuaFile(sFilePath, iResult, sInfo);
            }
        }
    }
    //週期任務加載
    vector<string> sDeleteFileNames;
    for (map<string, int>::const_iterator it = f_task_filePathTimeouts.begin(); it != f_task_filePathTimeouts.end(); ++it)
    {
        int iTimeOut = it->second;
        if (f_iTimeOutTimes % iTimeOut == 0)
        {
            const string & sFileName = it->first;
            string sFilePath = CxFileSystem::mergeFilePath(f_deal_filePath, sFileName);
            if (CxFileSystem::isExist(sFilePath))
            {
                msepoch_t dtNow = CxTime::currentSystemTime();
                lua_State * L = GclLua::luaState();
                beforeLoadLuaFile(sFilePath);
                int iResult = luaL_dofile(L, sFilePath.data());
                if( iResult != 0 )
                {
                    sDeleteFileNames.push_back(sFileName);
                    string sError = CxString::format(" luaL_dofile[%s] failed : [%s]", sFilePath.c_str(), lua_tostring(L,-1));
                    GclScript::outScriptPrompt() << CxTime::currentSepochString() << sError;
                    afterLoadLuaFile(sFilePath, iResult, sError);
                }
                else
                {
                    string sInfo = CxString::format(" luaL_dofile[%s] success !!!", sFilePath.c_str());
//                    GclScript::outScriptPrompt() << CxTime::currentSepochString() << sInfo;
                    afterLoadLuaFile(sFilePath, iResult, sInfo);
                }
                timems_t dtDiff = CxTime::currentSystemTime() - dtNow;
                if (dtDiff > 100)
                {
                    GclScript::outScriptPrompt() << sFilePath << " from " << CxTime::toString(dtNow) << " to "  << CxTime::currentSystemTimeString() << " spend time millsecond : " << dtDiff ;
                }
//                else
//                {
//                    string sInfo = CxString::format(" luaL_dofile[%s] success !!!", sFilePath.c_str());
//                    GclScript::outScriptDebug() << CxTime::currentSepochString() << sInfo;
//                }
            }
            else
            {
                sDeleteFileNames.push_back(sFileName);
                string sError = CxString::format(" lua timer task fileName[%s] do not exist !!!", sFileName.data());
                GclScript::outScriptDebug() << CxTime::currentSepochString() << sError;
            }
        }
    }
    if (sDeleteFileNames.size()>0)
    {
        CxContainer::remove(f_task_filePathTimeouts, sDeleteFileNames);
    }
    f_iTimeOutTimes++;
}

void GclLua::registLuaFileLoaded(fn_void_pchar_t fn)
{
    vector<fn_void_pchar_t> * oFnLuaFileLoadeds = getFnLuaFileLoadeds();
    if (std::find(oFnLuaFileLoadeds->begin(), oFnLuaFileLoadeds->end(), fn) == oFnLuaFileLoadeds->end())
        oFnLuaFileLoadeds->push_back(fn);
}
