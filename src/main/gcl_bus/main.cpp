#include <ccxx/cxapplication.h>
#include <vdi/common_service.h>
#include <vdi/measure_sharememory_all.h>
#include <vdi/fileinfo_sharememory.h>
#include "bus_measure_publish.h"
#include "bus_measure_control.h"
#include "bus_data_deal.h"

int main(int argc, const char*argv[])
{
    if(!CxApplication::singleInstance(argc, argv))
    {
        return -1;
    }
    //应用初始化
    CxApplication::init(argc, argv);
    //心跳服务初始化
    CommonService::init();

    MeasureShareMemoryAll::open(true);
    FileInfoShareMemory::open(true);

    //加载 发布器，发布器的初始化需要 MeasureShareMemory::memoryManager() 中量对象的个数
    BusMeasurePublisher::start();
    BusMeasureControl::start();

    CxTimerManager::startTimer(BusMeasurePublisher::timerCheckPublish, 30);
    CxTimerManager::startTimer(BusMeasureControl::timerCheckPublish, 50);
    CxTimerManager::startTimer(BusDataDeal::timerDeal, 1000);

    //GclLua::start();

    int iResult = CxApplication::exec();

    //先停时钟 -> 停发布器（包括发布器的线程） --> 关共享内存
    CxTimerManager::stopTimer(BusDataDeal::timerDeal);
    CxTimerManager::stopTimer(BusMeasureControl::timerCheckPublish);
    CxTimerManager::stopTimer(BusMeasurePublisher::timerCheckPublish);

    BusMeasureControl::stop();
    BusMeasurePublisher::stop();

    FileInfoShareMemory::close();
    MeasureShareMemoryAll::close();

    //GclLua::stop();

    return iResult;
}
