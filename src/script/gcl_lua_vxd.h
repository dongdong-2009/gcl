#ifndef GCL_LUA_VXD_H
#define GCL_LUA_VXD_H

#include  <ccxx/ccxx.h>

class GclLuaVxd
{
public:
    static void registerLua();

public:
    //业务处理
    static int dealCustomData( int iSourceId, int iCommandId, const std::map<std::string, std::string> & sParams = std::map<std::string, std::string>(), int iDataLength = 0, const char * pData = NULL, int iTargetId = 0 );
};

#endif // GCL_LUA_VXD_H
