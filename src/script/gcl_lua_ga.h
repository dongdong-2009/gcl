#ifndef GCL_LUA_GA_H
#define GCL_LUA_GA_H

#include <psm/general_access.h>
#include "gcl_lua_base.h"

class GclLuaGA: public GclLuaBase
{
public:
    GclLuaGA();

    static void registerLua();

public:
    //GA收到消息的处理
    static int dealMessage( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );

    //GA收到文件的处理
    static int dealFileWrite( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach );

    //GA已经发出去文件的处理
    static int dealResultFileWrite( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const PsmAttach * oAttach );

};

#endif // GCL_LUA_GA_H
