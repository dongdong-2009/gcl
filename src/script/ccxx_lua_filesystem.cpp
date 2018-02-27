#include "ccxx_lua_filesystem.h"

using namespace std;

CXLuaFileSystem * fn_oCXLuaFileSystem()
{
    GclLua::registInitFunction(CXLuaFileSystem::registerLua);
    static CXLuaFileSystem m;
    return &m;
}

static CXLuaFileSystem * f_oCXLuaFileSystem = fn_oCXLuaFileSystem();

static int fn_getPathInfo(lua_State *L)
{
    lua_newtable(L);     /* creates a table */

    int iTop = lua_gettop(L);

    string sFilePath;

    if(iTop >= 1)
    {
        LuaCommon::getValue(L, 1, sFilePath);
    }

    CxFilePathInfo pathInfo = CxFileSystem::getPathInfo(sFilePath);

//    PathTypeEnum pathType;
//    std::string path;
//    std::string fileName;
//    int64 fileSize;
//    msepoch_t creationTime;
//    msepoch_t lastWriteTime;

    LuaCommon::setTable(L, "pathType",      (int)pathInfo.pathType);
    LuaCommon::setTable(L, "path",          pathInfo.path);
    LuaCommon::setTable(L, "fileName",      pathInfo.fileName);
    LuaCommon::setTable(L, "fileSize",      pathInfo.fileSize);
    LuaCommon::setTable(L, "creationTime",  pathInfo.creationTime);
    LuaCommon::setTable(L, "lastWriteTime", pathInfo.lastWriteTime);

    return 1;
}

static int fn_createFile(lua_State *L)
{
    lua_newtable(L);     /* creates a table */

    int iTop = lua_gettop(L);

    string sFileName;

    if(iTop >= 1)
    {
        LuaCommon::getValue(L, 1, sFileName);
    }

    bool ctFileFlag = CxFileSystem::createFile(sFileName);
    if (!ctFileFlag)
        return -1;
    return 1;
}

static int fn_deleteFile(lua_State *L)
{
    lua_newtable(L);     /* creates a table */

    int iTop = lua_gettop(L);

    string sFileName;

    if(iTop >= 1)
    {
        LuaCommon::getValue(L, 1, sFileName);
    }

    bool dlFileFlag = CxFileSystem::deleteFile(sFileName);
    if (!dlFileFlag)
        return -1;
    return 1;
}

static int fn_copyFile(lua_State *L)
{
    lua_newtable(L);     /* creates a table */

    int iTop = lua_gettop(L);

    string sSource="";
    string sTarget="";

    if(iTop >= 2)
    {
        LuaCommon::getValue(L, 1, sSource);
        LuaCommon::getValue(L, 2, sTarget);
    }
    //清空
    lua_pop(L,lua_gettop(L));

    CxFileSystem::copyFile(sSource, sTarget);
    return 1;
}

static int fn_renameFile(lua_State *L)
{
    lua_newtable(L);     /* creates a table */

    int iTop = lua_gettop(L);

    string sOldPath="";
    string sNewPath="";

    if(iTop >= 2)
    {
        LuaCommon::getValue(L, 1, sOldPath);
        LuaCommon::getValue(L, 2, sNewPath);
    }
    //清空
    lua_pop(L,lua_gettop(L));

    CxFileSystem::renameFile(sOldPath, sNewPath);
    return 1;
}

static int fn_isExist(lua_State *L)
{
    lua_newtable(L);     /* creates a table */

    int iTop = lua_gettop(L);

    string sFileName;

    if(iTop >= 1)
    {
        LuaCommon::getValue(L, 1, sFileName);
    }
    lua_pop(L,lua_gettop(L));

    bool bFlag = CxFileSystem::isExist(sFileName);
    if (!bFlag)
        return 0;
    return 1;
}

static int fn_isFile(lua_State *L)
{
    lua_newtable(L);     /* creates a table */

    int iTop = lua_gettop(L);

    string sFileName;

    if(iTop >= 1)
    {
        LuaCommon::getValue(L, 1, sFileName);
    }
    lua_pop(L,lua_gettop(L));

    bool bFlag = CxFileSystem::isFile(sFileName);
    if (!bFlag)
        return 0;
    return 1;
}

static int fn_isDir(lua_State *L)
{
    lua_newtable(L);     /* creates a table */

    int iTop = lua_gettop(L);

    string sFileName;

    if(iTop >= 1)
    {
        LuaCommon::getValue(L, 1, sFileName);
    }
    lua_pop(L,lua_gettop(L));

    bool bFlag = CxFileSystem::isDir(sFileName);
    if (!bFlag)
        return 0;
    return 1;
}

static int fn_createDir(lua_State *L)
{
    int iTop = lua_gettop(L);
    string sFileName;
    if(iTop >= 1)
    {
        LuaCommon::getValue(L, 1, sFileName);
    }
    lua_pop(L,iTop);
    return CxFileSystem::createDir(sFileName) ? 1 : 0;
}

static int fn_removeDir(lua_State *L)
{
    int iTop = lua_gettop(L);
    string sFileName;

    int nRes = -1;
    if(iTop >= 1)
    {
        LuaCommon::getValue(L, 1, sFileName);
        nRes = CxFileSystem::removeDir(sFileName);
    }
    //清空
    lua_pop(L,iTop);
    //返回结果
    lua_pushnumber(L, nRes);
    return 1;
}

static int fn_scanDir(lua_State *L)
{
    int iTop = lua_gettop(L);
    string sScanpath;
    int mode = 0;
    int nRes = -1;

    if(iTop >= 2)
    {
        LuaCommon::getValue(L, 1, sScanpath);
        LuaCommon::getValue(L, 2, mode);

        bool bFlag = false;
        if(mode>0) bFlag = true;

        vector<CxFileSystem::PathInfo> pathInfos;
        CxFileSystem::scanDir(sScanpath, pathInfos, bFlag);

        int nCount = pathInfos.size();
        //清空
        lua_pop(L,iTop);

        lua_newtable(L);
        LuaCommon::setTable(L,CS_Lua_Succed,1);
        LuaCommon::setTable(L,CS_Lua_Count,nCount);
        for(size_t i=0;i<pathInfos.size();++i)
        {
            CxFileSystem::PathInfo mInfo = pathInfos.at(i);
            lua_newtable(L);
            LuaCommon::setTable(L,"path",mInfo.path);
            LuaCommon::setTable(L,"type",(int)mInfo.pathType);
            LuaCommon::setTable(L,"name",mInfo.fileName);
            LuaCommon::setTable(L,"create_time",(long long)mInfo.creationTime);
            LuaCommon::setTable(L,"last_time",(long long)mInfo.lastWriteTime);
            LuaCommon::setTable(L,"full_path",mInfo.filePath());

            lua_rawseti(L,-2,i+1);
        }
    }
    return 1;
}

void CXLuaFileSystem::registerLua()
{
    lua_State * L = GclLua::luaState();
    lua_newtable(L);
    LuaCommon::setTable(L, "getPathInfo",   fn_getPathInfo);
    LuaCommon::setTable(L, "createFile",    fn_createFile);
    LuaCommon::setTable(L, "deleteFile",    fn_deleteFile);
    LuaCommon::setTable(L, "copyFile",      fn_copyFile);
    LuaCommon::setTable(L, "renameFile",    fn_renameFile);
    LuaCommon::setTable(L, "isExist",       fn_isExist);
    LuaCommon::setTable(L, "isFile",        fn_isFile);
    LuaCommon::setTable(L, "isDir",         fn_isDir);
    LuaCommon::setTable(L, "createDir",     fn_createDir);
    LuaCommon::setTable(L, "removeDir",     fn_removeDir);
    LuaCommon::setTable(L, "scanDir",       fn_scanDir);

    lua_setglobal(L, "cx_fs");    /* 'name' = table */
}

CXLuaFileSystem::CXLuaFileSystem()
{
}
