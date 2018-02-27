#ifndef VDI_SCRIPT_H
#define VDI_SCRIPT_H

#include "protocol_global.h"

class VdiScript
{
public:
    //测试
    static void test();
    //初始化
    static bool init();
    //反初始化
    static bool unInit();
    //注册函数
    static bool regFunction(char *funName);
    //运行脚本
    static bool runScript(const char *fileName);
    //XML业务处理
    static bool dealXMLBuffer(const char *fc, const char* pBuf, int len);
   //XML业务处理
    static bool dealXMLFile(const char *fc, const char *fullPath);
   //e文件业务处理
   static bool dealEfile(const char* fc,const char* fullPath);
   //命令处理
   static bool dealMsg(int iTerminalId, const char* cmd,int len,const char* pram);

};



#endif // VDI_SCRIPT_H
