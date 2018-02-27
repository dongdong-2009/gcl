
#include <ccxx/cxapplication.h>
#include "ntp_server.h"
#include <vdi/common_service.h>

using namespace std;

int main(int argc, const char*argv[])
{
    CxApplication::init(argc, argv);

    //心跳服务初始化
    CommonService::init();


    //加载 发布器，发布器的初始化需要 MeasureShareMemory::memoryManager() 中量对象的个数

//    GclLua::start();

    int iResult = CxApplication::exec();

    //先停时钟 -> 停发布器（包括发布器的线程） --> 关共享内存

//    GclLua::stop();

    return iResult;
}

