
#include <ccxx/cxapplication.h>

#include <vdi/measure_sharememory_all.h>
#include <common/gcl_config.h>

#include "gcl_lua_dispatch.h"
#include <vdi/common_service.h>

using namespace std;

int main(int argc, const char*argv[])
{
    CxApplication::init(argc, argv);

    GclConfig::load();

    CommonService::init();

    MeasureShareMemoryAll::setAfterLoadCallback(GclLuaDispatch::measureShareMemoryAfterLoad);
    MeasureShareMemoryAll::setBeforeCloseCallback(GclLuaDispatch::measureShareMemoryBeforeClose);
    MeasureShareMemoryAll::open();

    int iResult = CxApplication::exec();

    MeasureShareMemoryAll::close();

    return iResult;
}

