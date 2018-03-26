

#include <ccxx/cxapplication.h>
#include <ccxx/cxtimer.h>
#include <vdi/measure_sharememory_all.h>
#include "rtdb_service.h"
#include <vdi/common_service.h>
#include <script/gcl_script_vxd.h>

using namespace std;


int main(int argc, const char*argv[])
{
    CxApplication::init(argc, argv);

    CommonService::init();

    MeasureShareMemoryAll::setAfterLoadCallback(RtdbService::measureShareMemoryAfterLoad);
    MeasureShareMemoryAll::setBeforeCloseCallback(RtdbService::measureShareMemoryBeforeClose);
    MeasureShareMemoryAll::open();

    //脚本执行
    GclScriptVxd::init();

    //开始数据库检查
    CxTimerManager::startTimer(CxDatabaseManager::connectCheck,60000);

    RtdbService::start();

    int iResult = CxApplication::exec();

    MeasureShareMemoryAll::close();

    GclScriptVxd::uninit();

    //停止数据库检查
    CxTimerManager::stopTimer(CxDatabaseManager::connectCheck);


    return iResult;
}

