#include "rtdb_service.h"

using namespace std;

static size_t f_monsbChangedIndex = GM_UINT_MAX;
static size_t f_ycaddChangedIndex = GM_UINT_MAX;
static size_t f_strawChangedIndex = GM_UINT_MAX;

void RtdbService::measureShareMemoryAfterLoad(int iMeasureType)
{
    switch (iMeasureType)
    {
        case MeasureType_Monsb:
            f_monsbChangedIndex = MonsbShareMemory::memoryManager()->changedCount() - 1;
            break;
        case MeasureType_Ycadd:
            f_ycaddChangedIndex = YcaddShareMemory::memoryManager()->changedCount() - 1;
            break;
        case MeasureType_Straw:
            f_strawChangedIndex = StrawShareMemory::memoryManager()->changedCount() - 1;
            break;
        default:
            break;
    }
}

void RtdbService::measureShareMemoryBeforeClose(int iMeasureType)
{
}

void RtdbService::start()
{
    CxApplication::pushProcessCallBack(RtdbService::doStart);
}

void RtdbService::stop()
{
    RtdbLogFile::stop();
    CxTimerManager::stopTimer(RtdbService::timerTimeout);
}

void RtdbService::timerTimeout(int iInterval)
{
    static int iTimes = 0;

    int iCase;
    iCase = iTimes % 5;
    switch (iCase)
    {
        case 0:
            break;
        case 1:
            if (MonsbShareMemory::memoryManager()->isValid())
            {
                size_t iMonsbChangedIndex = MonsbShareMemory::memoryManager()->checkChangeds(f_monsbChangedIndex, RtdbLogFile::insertChangeds_monsb);
                if (iMonsbChangedIndex != f_monsbChangedIndex)
                {
                    RtdbLogFile::saveMonsbChangedIndex(f_monsbChangedIndex, iMonsbChangedIndex);
                    f_monsbChangedIndex = iMonsbChangedIndex;
                }
            }
            break;
        case 2:
            if (YcaddShareMemory::memoryManager()->isValid())
            {
                size_t iYcaddChangedIndex = YcaddShareMemory::memoryManager()->checkChangeds(f_ycaddChangedIndex, RtdbLogFile::insertChangeds_ycadd);
                if (iYcaddChangedIndex != f_ycaddChangedIndex)
                {
                    RtdbLogFile::saveMonsbChangedIndex(f_ycaddChangedIndex, iYcaddChangedIndex);
                    f_ycaddChangedIndex = iYcaddChangedIndex;
                }
            }
            break;
        case 3:
            if (StrawShareMemory::memoryManager()->isValid())
            {
                size_t iStrawChangedIndex = StrawShareMemory::memoryManager()->checkChangeds(f_strawChangedIndex, RtdbLogFile::insertChangeds_straw);
                if (iStrawChangedIndex != f_strawChangedIndex)
                {
                    RtdbLogFile::saveMonsbChangedIndex(f_strawChangedIndex, iStrawChangedIndex);
                    f_strawChangedIndex = iStrawChangedIndex;
                }
            }
            break;
        case 4:
            RtdbLogFile::checkMeaureChangedFiles();
            break;
        default:
            break;
    }

    iTimes++;
}

int
RtdbService::dealMessage(const PsmExplainer *oPsm, const string &sCommand, const string &sParam, const PsmAttach *oAttach)
{
    return FALSE;
}

int
RtdbService::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    return FALSE;
}

void RtdbService::doStart(int, int, const void *, int, void *, void *)
{
    CxApplication::registStopFunction(RtdbService::stop);
    GeneralAccess::registerMessageCommandDeal(RtdbService::dealMessage);
    GeneralAccess::registerFileWriteDeal(RtdbService::dealFileWrite);
    CxTimerManager::startTimer(RtdbService::timerTimeout, 200);

    {
        f_monsbChangedIndex = RtdbLogFile::loadMonsbChangedIndex();
        f_ycaddChangedIndex = RtdbLogFile::loadYcaddChangedIndex();
        f_strawChangedIndex = RtdbLogFile::loadStrawChangedIndex();
    }
    RtdbLogFile::start();
}

