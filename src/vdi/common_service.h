#ifndef COMMON_SERVICE_H
#define COMMON_SERVICE_H


#include <common/gccommon.h>

class CommonService
{
public:
    CommonService();
    //初始化
    static void init();
    //启动
    static void start();
    //停止
    static void stop();
    //运行日志
    static void runLog();
    //发送心跳信号到看守进程
    static int sendLiveToDamon();
   //调试等级
    inline static int getDebugLevel();
    //调试等级
    inline static void setDebugLevel(int value);

    //增加全局值
    static void addGlobalValue(std::string key,std::string value);

    static void addGlobalValue(std::string key,GCCommon::GCDataElement* value);
    //加载全局值
    static GCCommon::GCDataElement *getGlobalValue(std::string key);
   //设置心跳信息
    static void setLiveInfo(std::string sLocalIp,int nLocolPort,std::string sRemoteIp,int nRemotePort);
    static void setLiveInfo();
    static void task60s(int interval); //分钟任务
    static void taskLive(int interval);  //保活任务
};

#endif // COMMON_SERVICE_H

