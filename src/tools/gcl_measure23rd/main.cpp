#include <QApplication>
#include <ccxx/ccxx.h>
#include <script/gcl_script.h>
#include <vdi/measure_sharememory_all.h>
#include <vdi/common_service.h>
#include "gcl_measure23rd.h"

using namespace std;

int main(int argc, const char*argv[])
{
    QApplication a(argc, (char **)argv);

    //应用初始化
    CxApplication::init(argc, argv);

    CommonService::init();

    MeasureShareMemoryAll::setAfterLoadCallback(Measure23rd::measureShareMemoryAfterLoad);
    MeasureShareMemoryAll::setBeforeCloseCallback(Measure23rd::measureShareMemoryBeforeClose);
    MeasureShareMemoryAll::open();

    //脚本执行
    GclLua::start();

    int iResult = CxApplication::exec();

    //
    GclLua::stop();

    MeasureShareMemoryAll::close();

    return iResult;
}

