
#ifndef GCL_SHAREMEM_SERVER_H
#define GCL_SHAREMEM_SERVER_H

#include <ccxx/ccxx.h>
#include <common/gccommon.h>

class GCLShareMemServer
{
public:
    GCLShareMemServer();
    //
    static int getIDbyUrl(std::string &sUrl);

    //获取值通过ID
    static bool getValueByID(int id,gcl_stu_lua_value &value);

    //获取值通过URL
    static bool getValueByUrl(std::string &url,gcl_stu_lua_value &value);

    //
    static void setValueByID(gcl_stu_lua_value &value);

    //
    static void setValueByUrl(std::string &sUrl,gcl_stu_lua_value &value);
};

#endif // GCL_SHAREMEM_SERVER_H
