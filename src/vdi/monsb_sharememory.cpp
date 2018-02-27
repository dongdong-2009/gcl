#include "monsb_sharememory.h"


using namespace std;


static MonsbShareMemory::MemoryManager f_monsbShareMemory;

MonsbShareMemory::MemoryManager * MonsbShareMemory::memoryManager()
{
    return & f_monsbShareMemory;
}

int MonsbShareMemory::MemoryManager::loadConfigImpl(MeasureShareMemory<int>::ConfigData * oConfigBegin, const MeasureShareMemory<int>::ConfigData * oConfigEnd)
{
    int iMinId = g_fn_getMinMeasureId(getMeasureTypeImpl());
    int iMaxId = g_fn_getMaxMeasureId(getMeasureTypeImpl());
    int iMeasureId = iMinId;
    MeasureShareMemory<int>::ConfigData * oConfig = oConfigBegin;
    while ((oConfig < oConfigEnd) && (iMeasureId < iMaxId))
    {
        oConfig->measureId = iMeasureId;
        oConfig->equalStrategyId = 0;
        oConfig ++;
        iMeasureId ++;
    }
    return TRUE;
}
