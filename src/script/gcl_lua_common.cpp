#include "gcl_lua_common.h"
#include "gcl_lua_global.h"

using namespace std;


void LuaCommon::stackDump(lua_State *L)
{
    int i;

    int top = lua_gettop(L);

    printf("\nlua_gettop - count=%d : \n", top);

    for (i = 1; i <= top; i++) {  /* repeat for each level */

        int t = lua_type(L, i);

        printf("%03d : typeid=%d   typename=%s   value=", i, t, lua_typename(L, i));

        switch (t) {

        case LUA_TSTRING:  /* strings */

            printf("`%s'", lua_tostring(L, i));

            break;

        case LUA_TBOOLEAN:  /* booleans */

            printf(lua_toboolean(L, i) ? "true" : "false");

            break;

        case LUA_TNUMBER:  /* numbers */

            printf("%g", lua_tonumber(L, i));

            break;

        default:  /* other values */

            printf("%s", lua_typename(L, t));

            break;

        }

        printf("  \n");  /* put a separator */
    }

    printf("\n");     /* end the listing */
}


void LuaCommon::setTable (lua_State *L , const string & sKey, bool value, const string & sTable)
{
    if (sTable.empty())
    {
        lua_pushstring(L, sKey.c_str());
        lua_pushboolean(L, value);
        lua_settable(L, -3);
    }
    else
    {

    }
}

void LuaCommon::setTable (lua_State *L , const string & sKey, int value, const string & sTable)
{
    if (sTable.empty())
    {
        lua_pushstring(L, sKey.c_str());
        lua_pushinteger(L, value);
        lua_settable(L, -3);
    }
    else
    {

    }
}

void LuaCommon::setTable(lua_State *L, const string &sKey, long long value, const string &sTable)
{
    if (sTable.empty())
    {
        lua_pushstring(L, sKey.c_str());
        lua_pushinteger(L, value);
        lua_settable(L, -3);
    }
    else
    {

    }
}

void LuaCommon::setTable (lua_State *L , const string & sKey, double value, const string & sTable)
{
    if (sTable.empty())
    {
        lua_pushstring(L, sKey.c_str());
        lua_pushnumber(L, value);
        lua_settable(L, -3);
    }
    else
    {

    }
}

void LuaCommon::setTable (lua_State *L , const string & sKey, const string & value, const string & sTable)
{
    if (sTable.empty())
    {
        lua_pushstring(L, sKey.c_str());
        lua_pushlstring(L, value.data(), value.size());
        lua_settable(L, -3);
    }
    else
    {

    }
}

void LuaCommon::setTable (lua_State *L , const string & sKey, lua_CFunction value, const string & sTable)
{
    if (sTable.empty())
    {
        lua_pushstring(L, sKey.c_str());
        lua_pushcfunction(L, value);
        lua_settable(L, -3);
    }
    else
    {

    }
}












bool LuaCommon::getTable(lua_State *L, const string &sKey, bool &value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_getfield(L, idx, sKey.c_str());
        if (t == LUA_TBOOLEAN)
        {
            value = lua_toboolean(L, -1);
            bIs = TRUE;
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {

    }
    return bIs;
}

bool LuaCommon::getTable(lua_State *L, const string &sKey, int &value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_getfield(L, idx, sKey.c_str());
        if (t != LUA_TNIL)
        {
            lua_Integer v = lua_tointegerx(L, -1, &bIs);
            if (bIs)
            {
                value = v;
            }
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {
    }
    return bIs;
}

bool LuaCommon::getTable(lua_State *L, const string &sKey, long long &value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_getfield(L, idx, sKey.c_str());
        if (t != LUA_TNIL)
        {
            lua_Integer v = lua_tointegerx(L, -1, &bIs);
            if (bIs)
            {
                value = v;
            }
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {
    }
    return bIs;
}

bool LuaCommon::getTable(lua_State *L, const string &sKey, double &value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_getfield(L, idx, sKey.c_str());
        if (t != LUA_TNIL)
        {
            lua_Number v = lua_tonumberx(L, -1, &bIs);
            if (bIs)
            {
                value = v;
            }
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {
    }
    return bIs;
}

bool LuaCommon::getTable(lua_State *L, const string &sKey, string &value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_getfield(L, idx, sKey.c_str());
        if (t != LUA_TNIL)
        {
            size_t iParam = 0;
            const char * pParam = lua_tolstring(L, -1, & iParam);
            if(pParam)
            {
                value = string(pParam, iParam);
                bIs = TRUE;
            }
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {
    }
    return bIs;
}

bool LuaCommon::getTable(lua_State *L, const string &sKey, lua_CFunction & value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_getfield(L, idx, sKey.c_str());
        if (t == LUA_TFUNCTION)
        {
            value = lua_tocfunction(L, -1);
            bIs = (value != NULL);
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {
    }
    return bIs;
}




















bool LuaCommon::getTable(lua_State *L, int idxField, bool &value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_rawgeti(L, idx, idxField);
        if (t == LUA_TBOOLEAN)
        {
            value = lua_toboolean(L, -1);
            bIs = TRUE;
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {

    }
    return bIs;
}

bool LuaCommon::getTable(lua_State *L, int idxField, int &value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_rawgeti(L, idx, idxField);
        if (t != LUA_TNIL)
        {
            lua_Integer v = lua_tointegerx(L, -1, &bIs);
            if (bIs)
            {
                value = v;
            }
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {
    }
    return bIs;
}

bool LuaCommon::getTable(lua_State *L, int idxField, long long &value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_rawgeti(L, idx, idxField);
        if (t != LUA_TNIL)
        {
            lua_Integer v = lua_tointegerx(L, -1, &bIs);
            if (bIs)
            {
                value = v;
            }
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {
    }
    return bIs;
}

bool LuaCommon::getTable(lua_State *L, int idxField, double &value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_rawgeti(L, idx, idxField);
        if (t != LUA_TNIL)
        {
            int bIs;
            lua_Number v = lua_tonumberx(L, -1, &bIs);
            if (bIs)
            {
                value = v;
            }
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {
    }
    return bIs;
}

bool LuaCommon::getTable(lua_State *L, int idxField, string &value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_rawgeti(L, idx, idxField);
        if (t != LUA_TNIL)
        {
            size_t iParam = 0;
            const char * pParam = lua_tolstring(L, -1, & iParam);
            if(pParam)
            {
                value = string(pParam, iParam);
                bIs = TRUE;
            }
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {
    }
    return bIs;
}

bool LuaCommon::getTable(lua_State *L, int idxField, lua_CFunction & value, int idxTable, const string &sTable)
{
    int bIs = FALSE;
    if (sTable.empty())
    {
        int idx = idxTable;
        if (idx == 0) idx = -1;
        int t = lua_rawgeti(L, idx, idxField);
        if (t == LUA_TFUNCTION)
        {
            value = lua_tocfunction(L, -1);
            bIs = (value != NULL);
        }
        //保证 idxTable 不变
        if (idx < 0)
        {
            lua_pop(L, 1);
        }
    }
    else
    {
    }
    return bIs;
}











bool LuaCommon::getValue(lua_State *L , int idx, bool &b)
{
    if (lua_isboolean(L, idx))
    {
        b = lua_toboolean(L, idx);
        return true;
    }
    return false;
}

bool LuaCommon::getValue(lua_State *L , int idx, int& i)
{
    int bIs;
    lua_Integer value = lua_tointegerx(L, idx, &bIs);
    if (bIs)
    {
        i = value;
        return true;
    }
    return false;
}

bool LuaCommon::getValue(lua_State *L , int idx, uint32& n)
{
    int bIs;
    lua_Integer value = lua_tointegerx(L, idx, &bIs);
    if (bIs)
    {
        n = value;
        return true;
    }
    return false;
}

bool LuaCommon::getValue(lua_State *L , int idx, int64& n)
{
    int bIs;
    lua_Integer value = lua_tointegerx(L, idx, &bIs);
    if (bIs)
    {
        n = value;
        return true;
    }
    return false;
}

bool LuaCommon::getValue(lua_State *L , int idx, uint64& n)
{
    int bIs;
    lua_Integer value = lua_tointegerx(L, idx, &bIs);
    if (bIs)
    {
        n = value;
        return true;
    }
    return false;
}

bool LuaCommon::getValue(lua_State *L , int idx, double& d)
{
    int bIs;
    lua_Number value = lua_tonumberx(L, idx, &bIs);
    if (bIs)
    {
        d = value;
        return true;
    }
    return false;
}

bool LuaCommon::getValue(lua_State *L , int idx, float& f)
{
    int bIs;
    lua_Number value = lua_tonumberx(L, idx, &bIs);
    if (bIs)
    {
        f = value;
        return true;
    }
    return false;
}

bool LuaCommon::getValue(lua_State *L , int idx, std::string& s, bool bCString)
{
    size_t iParam = 0;
    const char * pParam = lua_tolstring(L, idx, & iParam);
    if(pParam)
    {
        if (bCString)
            s = pParam;
        else
            s = string(pParam, iParam);
        return true;
    }
    return false;
}

bool LuaCommon::getValue(lua_State *L, int idx, std::vector<string> &ss, bool bCString)
{
    int iTop = lua_gettop(L);
    if (iTop < 1) return false;
    int t = lua_type(L, idx);
    if (t == LUA_TTABLE)
    {
        size_t iCount = lua_rawlen(L, idx);
        for (size_t i = 1; i <= iCount; ++i)
        {
            t = lua_rawgeti(L, idx, i);
            if (t != LUA_TNIL)
            {
                size_t iParam = 0;
                const char * pParam = lua_tolstring(L, -1, & iParam);
                if(pParam)
                {
                    if (bCString)
                        ss.push_back(string(pParam));
                    else
                        ss.push_back(string(pParam, iParam));
                }
            }
            //保证 idx 不变
            lua_pop(L, 1);
        }
    }
    lua_settop(L, iTop);
    return true;
}

bool LuaCommon::getValue(lua_State *L, int idx, std::vector<std::vector<string> > &sRows, bool bCString)
{
    int iTop = lua_gettop(L);
    if (iTop < 1) return false;
    int t = lua_type(L, idx);
    if (t == LUA_TTABLE)
    {
        size_t iParam = 0;
        const char * pParam = NULL;
        size_t iRowCount = lua_rawlen(L, idx);
        for (size_t i = 1; i <= iRowCount; ++i)
        {
            t = lua_rawgeti(L, idx, i);
            if (t == LUA_TTABLE)
            {
                std::vector<string> sRow;
                size_t iColumnCount = lua_rawlen(L, -1);
                for (size_t j = 0; j <= iColumnCount; ++j)
                {
                    t = lua_rawgeti(L, -1, j);
                    if (t != LUA_TNIL)
                    {
                        pParam = lua_tolstring(L, -1, & iParam);
                        if(pParam)
                        {
                            if (bCString)
                                sRow.push_back(string(pParam));
                            else
                                sRow.push_back(string(pParam, iParam));
                        }
                    }
                    //保证 idx 不变
                    lua_pop(L, 1);
                }
                sRows.push_back(sRow);
            }
            //保证 idx 不变
            lua_pop(L, 1);
        }
    }
    lua_settop(L, iTop);
    return true;
}



bool LuaCommon::callLuaFunction(lua_State *L , int iParamCount, int iResultCount)
{
    bool r = false;
    if (lua_pcall(L,iParamCount,iResultCount,0) == 0)
    {
        r = true;
    }
    else
    {
        string sError = lua_tostring(L, -1);
        GclScript::outScriptPrompt() << "error running function `f': " << sError;
        r = false;
    }
    //清空
    lua_settop(L, 0);
    return r;
}

bool LuaCommon::callLuaFunction(lua_State *L , int iParamCount, int iResultCount, int &iResult)
{
    bool r = false;
    if (callLuaFunction(L, iParamCount, iResultCount))
    {
        LuaCommon::getValue(L, -1, iResult);
        r = true;
    }
    //清空
    lua_settop(L, 0);
    return r;
}

bool LuaCommon::callLuaFunction(lua_State *L , int iParamCount, int iResultCount, string &sResult)
{
    bool r = false;
    if (callLuaFunction(L, iParamCount, iResultCount))
    {
        LuaCommon::getValue(L, -1, sResult);
        r = true;
    }
    //清空
    lua_settop(L, 0);
    return r;
}

bool LuaCommon::runScript(lua_State *L, const string &sFilePath)
{
    assert(L != NULL);

    int iResult = luaL_dofile(L, sFilePath.c_str());
    if( iResult != 0 )
    {
        string sError = CxString::format(" luaL_dofile[%s] failed : [%s]", sFilePath.c_str(), lua_tostring(L,-1));
        GclScript::outScriptPrompt() << CxTime::currentSepochString() << sError;
        return false;
    }
    else
    {
        string sInfo = CxString::format(" luaL_dofile[%s] success !!!", sFilePath.c_str());
        GclScript::outScriptDebug() << CxTime::currentSepochString() << sInfo;
    }
    return true;
}
