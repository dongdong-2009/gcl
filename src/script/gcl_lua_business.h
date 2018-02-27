#ifndef GCL_LUA_BUSINESS_H
#define GCL_LUA_BUSINESS_H

#include "gcl_lua_base.h"

class GclLuaBusiness:public GclLuaBase
{
public:
    GclLuaBusiness();
    static void registerLua();
public:
    //发送消息
    static int sendMessage(const std::string &sSender,const std::string & sCommand, const std::string & sParam,const std::map<std::string, std::string> & ottach = std::map<std::string, std::string>());

};

#endif // GCL_LUA_BUSINESS_H
