#include "vxdcommon.h"
#include <ccxx/cxapplication.h>
#include <vdi/common_service.h>
#include <vdi/terminal_exe.h>
#include <script/gcl_script_vxd.h>

int VxdCommon::start(int argc, const char *argv[])
{
    //单实例检测
    if(!CxApplication::singleInstance(argc, argv))
    {
        return -1;
    }
    //应用初始化
    CxApplication::init(argc, argv);
    //心跳服务初始化
    CommonService::init();

    //终端加载
    TerminalExe::load(argc, argv);

    //开始执行
    TerminalExe::start();
    //脚本执行
    GclScriptVxd::init();

    int iResult = CxApplication::exec();

    //停止脚本
    GclScriptVxd::uninit();
    //停止终端
    TerminalExe::stop();

    return iResult;
}
