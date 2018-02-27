#include "gcl_lua_code.h"

using namespace std;

GclLuaCode * fn_oGclLuaCode()
{
    GclLua::registInitFunction(GclLuaCode::registerLua);
    static GclLuaCode m;
    return &m;
}

static GclLuaCode * f_oGclLuaCode = fn_oGclLuaCode();

//only win32
static int GBKToUTF8(lua_State *L)
{
//    int n = lua_gettop(L);
//    std::string strGBK = luaL_checkstring(L,1);
    string strGBK;
    bool bOk = LuaCommon::getValue(L, 1, strGBK);
    if (! bOk)
        return FALSE;
    std::string strOutUTF8 = "";
    WCHAR * str1;
    int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
    str1 = new WCHAR[n];
    MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
    n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
    char * str2 = new char[n];
    WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
    strOutUTF8 = str2;
    delete[]str1;
    str1 = NULL;
    delete[]str2;
    str2 = NULL;
    //LuaCommon::setTable(L,UTF8,strOutUTF8);
    lua_pushstring(L, strOutUTF8.c_str());
    return 1;
}

//only win32
static int UTF8ToGBK(lua_State *L)
{
//    int n = lua_gettop(L);
//    std::string strUTF8 = luaL_checkstring(L,1);
    string strUTF8; bool bOk = LuaCommon::getValue(L, 1, strUTF8);
    if (! bOk)
        return FALSE;

    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
    //unsigned short * wszGBK = new unsigned short[len + 1];
    wchar_t * wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUTF8.c_str(), -1, wszGBK, len);

    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP,0, wszGBK, -1, szGBK, len, NULL, NULL);
    std::string strTemp(szGBK);
    delete[]szGBK;
    delete[]wszGBK;
    //LuaCommon::setTable(L,GBK,strTemp.c_str());
    lua_pushstring(L, strTemp.c_str());
    return 1;
}

void GclLuaCode::registerLua()
{
    lua_State * L = GclLua::luaState();
    //创建 gcl_psm 表
    lua_newtable(L);
    LuaCommon::setTable(L, "GBKToUTF8", GBKToUTF8);
    LuaCommon::setTable(L, "UTF8ToGBK", UTF8ToGBK);
    lua_setglobal(L, "gcs_code");    /* 'name' = table */
}

GclLuaCode::GclLuaCode()
{
}
