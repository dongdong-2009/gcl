
#include <ccxx/ccxx.h>

#include <vdi/common_service.h>

#include "gcl_rttrigger.h"

using namespace std;


int main(int argc, const char*argv[])
{
    //应用初始化
    CxApplication::init(argc, argv);

    CommonService::init();

    //脚本执行
    GclLua::start();

    int iResult = CxApplication::exec();

    GclLua::stop();

    return iResult;
}

