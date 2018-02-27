
#include <ccxx/ccxx.h>
#include <vdi/terminal_exe.h>
#include <vdi/terminal_manager.h>
#include <psm/general_access.h>
#include <script/gcl_script_vxd.h>
#include <vdi/common_service.h>

using namespace std;

int main(int argc, const char*argv[])
{
    if(!CxApplication::singleInstance(argc, argv))
    {
        return -1;
    }

    cxPrompt() << CxTime::currentSepochString()<< " begin";



    //应用初始化
    CxApplication::init(argc, argv);

    CommonService::init();

    //终端加载
    TerminalExe::load(argc, argv);

    //开始执行
    TerminalExe::start();

    //脚本执行
    GclScriptVxd::init();

    //
    //开始数据库检查
    CxTimerManager::startTimer(CxDatabaseManager::connectCheck,60000);

    int iResult = CxApplication::exec();

    //停止数据库检查
    CxTimerManager::stopTimer(CxDatabaseManager::connectCheck);

    //停止
    TerminalExe::stop();

    //
    GclScriptVxd::uninit();

    return iResult;
}

