#ifndef GCL_LUA_EFILE_H
#define GCL_LUA_EFILE_H

#include "gcl_lua_base.h"

class GclLuaEFile:public GclLuaBase
{
public:
    static void registerLua();
public:
    GclLuaEFile();
    //业务处理
    static bool dealBuffer(const char *fc, const char* pBuf, int len);
   //业务处理
    static bool dealFile(const char *fc, const char *fullPath);

};

#endif // GCL_LUA_EFILE_H
