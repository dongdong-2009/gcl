#ifndef GCL_LUA_COMMON_H
#define GCL_LUA_COMMON_H


//#ifndef LUA_BUILD_AS_DLL
//#define LUA_BUILD_AS_DLL
//#endif

// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++
#include <lua-5.3.4/lua.hpp>
#include "gcl_script_global.h"

class LuaCommon
{
public:
//* 通用
    static void stackDump (lua_State *L);

    //*set value by table[idxTable=-1]
    static void setTable (lua_State *L , const std::string & sKey, bool value, const std::string & sTable = std::string());

    static void setTable (lua_State *L , const std::string & sKey, int value, const std::string & sTable = std::string());

    static void setTable (lua_State *L , const std::string & sKey, long long value, const std::string & sTable = std::string());

    static void setTable (lua_State *L , const std::string & sKey, double value, const std::string & sTable = std::string());

    static void setTable (lua_State *L , const std::string & sKey, const std::string & value, const std::string & sTable = std::string());

    static void setTable (lua_State *L , const std::string & sKey, lua_CFunction value, const std::string & sTable = std::string());

    //*get value by table[idxTable]
    static bool getTable (lua_State *L , const std::string & sKey, bool & value, int idxTable = 0, const std::string & sTable = std::string());

    static bool getTable (lua_State *L , const std::string & sKey, int & value, int idxTable = 0, const std::string & sTable = std::string());

    static bool getTable (lua_State *L , const std::string & sKey, long long & value, int idxTable = 0, const std::string & sTable = std::string());

    static bool getTable (lua_State *L , const std::string & sKey, double & value, int idxTable = 0, const std::string & sTable = std::string());

    static bool getTable (lua_State *L , const std::string & sKey, std::string & value, int idxTable = 0, const std::string & sTable = std::string());

    static bool getTable (lua_State *L , const std::string & sKey, lua_CFunction & value, int idxTable = 0, const std::string & sTable = std::string());

    //*get value by table[idxTable]
    static bool getTable (lua_State *L , int idxKey, bool & value, int idxTable = 0, const std::string & sTable = std::string());

    static bool getTable (lua_State *L , int idxKey, int & value, int idxTable = 0, const std::string & sTable = std::string());

    static bool getTable (lua_State *L , int idxKey, long long & value, int idxTable = 0, const std::string & sTable = std::string());

    static bool getTable (lua_State *L , int idxKey, double & value, int idxTable = 0, const std::string & sTable = std::string());

    static bool getTable (lua_State *L , int idxKey, std::string & value, int idxTable = 0, const std::string & sTable = std::string());

    static bool getTable (lua_State *L , int idxKey, lua_CFunction & value, int idxTable = 0, const std::string & sTable = std::string());

    //*get value by top
    static bool getValue(lua_State *L , int idx, bool& b);

    static bool getValue(lua_State *L , int idx, int& i);

    static bool getValue(lua_State *L , int idx, uint32& n);

    static bool getValue(lua_State *L , int idx, int64& n);

    static bool getValue(lua_State *L , int idx, uint64& n);

    static bool getValue(lua_State *L , int idx, double& d);

    static bool getValue(lua_State *L , int idx, float& f);

    static bool getValue(lua_State *L , int idx, std::string& s, bool bCString = false);

    static bool getValue(lua_State *L , int idx, std::vector<std::string>& ss, bool bCString = false);

    static bool getValue(lua_State *L , int idx, std::vector<std::vector<std::string> >& sRows, bool bCString = false);

    //*call lua function
    static bool callLuaFunction(lua_State *L , int iParamCount, int iResultCount);

    static bool callLuaFunction(lua_State *L , int iParamCount, int iResultCount, int & iResult);

    static bool callLuaFunction(lua_State *L , int iParamCount, int iResultCount, std::string & sResult);

    //*dofile : run lua file
    static bool runScript(lua_State *L , const std::string & sFilePath);

};

#endif // GCL_LUA_COMMON_H
