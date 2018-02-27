#include "straw_sharememory.h"


using namespace std;


static StrawShareMemory::MemoryManager f_strawShareMemory;

StrawShareMemory::MemoryManager * StrawShareMemory::memoryManager()
{
    return & f_strawShareMemory;
}


int StrawShareMemory::MemoryManager::loadConfigImpl(MeasureShareMemory<StrawValue>::ConfigData *oConfigBegin, const MeasureShareMemory<StrawValue>::ConfigData *oConfigEnd)
{
    int iMinId = g_fn_getMinMeasureId(getMeasureTypeImpl());
    int iMaxId = g_fn_getMaxMeasureId(getMeasureTypeImpl());
    int iMeasureId = iMinId;
    MeasureShareMemory<StrawValue>::ConfigData * oConfig = oConfigBegin;
    while ((oConfig < oConfigEnd) && (iMeasureId < iMaxId))
    {
        oConfig->measureId = iMeasureId;
        oConfig->equalStrategyId = 0;
        oConfig ++;
        iMeasureId ++;
    }
    return TRUE;
}
