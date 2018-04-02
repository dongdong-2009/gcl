#ifndef RTDB_LOGFILE_H
#define RTDB_LOGFILE_H

#include <ccxx/ccxx.h>
#include <vdi/measure_sharememory_all.h>

class MeasureChangedFile
{
public:
    FILE* file;
    int newCount;

};

class RtdbLogFile
{
public:
    static int
    init();

    static int
    start();

    static int
    stop();

    static std::string
    getMeasureValue(const int& value);

    static std::string
    getMeasureValue(const double& value);

    static std::string
    getMeasureValue(const StrawValue& value);

    static std::string
    getMeasureLogPath();

    static int
    checkMeaureChangedFiles();

    static std::string
    getMeasureChanngedText(const int& value,
                           int iQuality,
                           msepoch_t dtChangedTime,
                           int iChangedSourceId,
                           int iChangedReasonId);

    static std::string
    getMeasureChanngedText(const double& value,
                           int iQuality,
                           msepoch_t dtChangedTime,
                           int iChangedSourceId,
                           int iChangedReasonId);

    static std::string
    getMeasureChanngedText(const StrawValue& value,
                           int iQuality,
                           msepoch_t dtChangedTime,
                           int iChangedSourceId,
                           int iChangedReasonId);

    template<typename T>
    static void
    insertChangeds_impl(const typename MeasureShareMemory<T>::ChangedData* const oChangedBegin,
                        const typename MeasureShareMemory<T>::ChangedData* const oChangedEnd)
    {
        const typename MeasureShareMemory<T>::ChangedData* oChangedData = oChangedBegin;
        std::vector<std::string> sSqls;
        FILE* oFile = NULL;
        std::string sText;
        size_t iWrote;
        while (oChangedData < oChangedEnd)
        {
            std::string sFileName = CxString::toHexstring(oChangedData->measureId) + ".txt";
            std::string sFilePath = CxFileSystem::mergeFilePath(getMeasureLogPath(), sFileName);
            FILE* oFile = fopen(sFilePath.data(), "ab+");
            if (oFile)
            {
                sText = getMeasureChanngedText(oChangedData->value,
                                               oChangedData->quality, oChangedData->changedTime,
                                               oChangedData->changedSourceId, oChangedData->changedReasonId);
                iWrote = fwrite(const_cast<char*>(sText.data()), 1, sText.size(), oFile);
                if (iWrote <= 0)
                {
                    cxLogDebug() << "insertChangeds_impl error - sText: " << sText;
                }
                fclose(oFile);
            }
            else
            {
                cxLogDebug() << "fopen [ " << sFilePath << " ] error: " << CxFileSystem::getFileLastError();
            }
            oChangedData++;
        }
    }

    static void
    insertChangeds_monsb(const MonsbShareMemory::ChangedData* const oChangedBegin,
                         const typename MonsbShareMemory::ChangedData* const oChangedEnd)
    {
        insertChangeds_impl<int>(oChangedBegin, oChangedEnd);
    }

    static void
    insertChangeds_ycadd(const YcaddShareMemory::ChangedData* const oChangedBegin,
                         const typename YcaddShareMemory::ChangedData* const oChangedEnd)
    {
        insertChangeds_impl<double>(oChangedBegin, oChangedEnd);
    }

    static void
    insertChangeds_straw(const StrawShareMemory::ChangedData* const oChangedBegin,
                         const typename StrawShareMemory::ChangedData* const oChangedEnd)
    {
        insertChangeds_impl<StrawValue>(oChangedBegin, oChangedEnd);
    }

    static size_t
    loadMeasureChangedIndex(const std::string& sFileName);

    static size_t
    loadMonsbChangedIndex();

    static size_t
    loadYcaddChangedIndex();

    static size_t
    loadStrawChangedIndex();

    static int
    saveMeasureChangedIndex(size_t iOldIndex, size_t iNewIndex, FILE* oFile);

    static int
    saveMonsbChangedIndex(size_t iOldIndex, size_t iNewIndex);

    static int
    saveYcaddChangedIndex(size_t iOldIndex, size_t iNewIndex);

    static int
    saveStrawChangedIndex(size_t iOldIndex, size_t iNewIndex);

    static std::string
    loadMeasureChanged(int iMeasureId, const std::string & sDirName);

    static std::string
    loadMeasureChanged(int iMeasureId, msepoch_t dtBegin, msepoch_t dtEnd);

};

#endif // RTDB_LOGFILE_H
