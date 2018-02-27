#include "ycadd_sharememory.h"

#include <common/gcl_resource.h>

using namespace std;


static YcaddShareMemory::MemoryManager f_ycaddShareMemory;

YcaddShareMemory::MemoryManager * YcaddShareMemory::memoryManager()
{
    return & f_ycaddShareMemory;
}

int YcaddShareMemory::MemoryManager::loadConfigImpl(MeasureShareMemory<double>::ConfigData * oConfigBegin, const MeasureShareMemory<double>::ConfigData * oConfigEnd)
{
    int iMinId = g_fn_getMinMeasureId(getMeasureTypeImpl());
    int iMaxId = g_fn_getMaxMeasureId(getMeasureTypeImpl());
    int iMeasureId = iMinId;
    MeasureShareMemory<double>::ConfigData * oConfig = oConfigBegin;
    while ((oConfig < oConfigEnd) && (iMeasureId < iMaxId))
    {
        oConfig->measureId = iMeasureId;
        oConfig->equalStrategyId = 1;
        oConfig ++;
        iMeasureId ++;
    }
    iMaxId = iMeasureId;

    string sFilePath = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), "measure/ycadd_strategy.xml");
    CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
    if (oProfile)
    {
        const std::map<std::string, std::map<std::string, std::string> > & sRows = oProfile->getSectionEntryValues();

        for (map<string, map<string, string> >::const_iterator itRows = sRows.begin(); itRows != sRows.end(); ++itRows)
        {
            const string & sSection = itRows->first;
            if (sSection == CxDescribeStrings::self())
                continue;

            const map<string, string> & sRow = itRows->second;

            iMeasureId = CxContainer::valueTo(sRow, CS_EntryMeasureId, ci_int_zero);
            int iEqualStrategyId = CxContainer::valueTo(sRow, CS_EntryEqualStrategy, ci_int_zero);
            double dDeadValue = CxContainer::valueTo(sRow, CS_EntryDeadValue, cd_double_zero);
            if (iMeasureId>=iMinId && iMeasureId<iMaxId && iEqualStrategyId>0)
            {
                oConfig = oConfigBegin + (iMeasureId-iMinId);
                oConfig->equalStrategyId = iEqualStrategyId;
                oConfig->value1 = dDeadValue;
                cxDebug() << "config iMeasureId " << iMeasureId << " ; equalStrategyId : " << oConfig->equalStrategyId << dDeadValue << cxEndLine;
            }
        }
        CxSkverManager::endUse(oProfile);
    }
    return TRUE;
}
