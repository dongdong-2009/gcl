#ifndef GCL_MEASURE_SERVER_H
#define GCL_MEASURE_SERVER_H


#include <common/gccommon.h>

class GCLMeasureServer
{
public:
    GCLMeasureServer();
    //启动
    static void start();
    //停止
    static void stop();
    //
    static int getIDbyUrl(std::string &sUrl);
    //获取值通过ID
    static bool getValueByID(int id,gcl_stu_lua_value &value);
    //获取值通过URL
    static bool getValueByUrl(std::string &url,gcl_stu_lua_value &value);
    //根据ID设置值
    static void setValueByID(gcl_stu_lua_value &value);
    //根据url设置值
    static void setValueByUrl(std::string &sUrl,gcl_stu_lua_value &value);

    static void loadUrlToID();


};

#endif // GCL_MEASURE_SERVER_H
