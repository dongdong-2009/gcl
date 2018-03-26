#include "rtdb_service.h"
#include <vdi/measure_sharememory_all.h>
#include <script/gcl_lua_shm.h>
#include <shm/measure_sharememory.h>

using namespace std;

// T_SYS_CONFIG
static const std::string cs_sql_create_system_config_table =
    " CREATE TABLE [T_SYS_CONFIG] ("
        " [F_PID] int ,"
        " [F_TID] int,"
        " [F_Key] VARCHAR(256) NOT NULL,"
        " [F_Value] VARCHAR(4096) NOT NULL,"
        " [F_Quality] int,"
        " [F_ChangedTime] int64 NOT NULL,"
        " [F_ChangedSourceId] int,"
        " [F_ChangedReasonId] int);";

static const char c_sSqlInsert_T_SYS_CONFIG[] =
    "    INSERT INTO [T_SYS_CONFIG]( [F_Key], [F_Value], [F_ChangedTime] )"
        "    VALUES( '%s', '%s', %lld);";

// T_*_CHANGED
static const std::string cs_sql_create_monsb_changed_table =
    " CREATE TABLE [%s] ("
        " [F_MeasureId] int NOT NULL, "
        " [F_Value] int, "
        " [F_Quality] int, "
        " [F_ChangedTime] int64 NOT NULL,"
        " [F_ChangedSourceId] int, "
        " [F_ChangedReasonId] int);";

static const std::string cs_sql_create_ycadd_changed_table =
    " CREATE TABLE [%s] ("
        " [F_MeasureId] int NOT NULL, "
        " [F_Value] double, "
        " [F_Quality] int, "
        " [F_ChangedTime] int64 NOT NULL,"
        " [F_ChangedSourceId] int, "
        " [F_ChangedReasonId] int);";

static const std::string cs_sql_create_straw_changed_table =
    " CREATE TABLE [%s] ("
        " [F_MeasureId] int NOT NULL, "
        " [F_Value] VARCHAR(128), "
        " [F_Quality] int, "
        " [F_ChangedTime] int64 NOT NULL,"
        " [F_ChangedSourceId] int, "
        " [F_ChangedReasonId] int);";

static const std::string cs_sql_insert_monsb_changed =
    " INSERT INTO [%s] ( F_MeasureId, F_Value, F_Quality, F_ChangedTime, F_ChangedSourceId, F_ChangedReasonId )"
        " VALUES ( %d ,%d ,%d ,%lld ,%d ,%d )";

static const std::string cs_sql_insert_ycadd_changed =
    " INSERT INTO [%s] ( F_MeasureId, F_Value, F_Quality, F_ChangedTime, F_ChangedSourceId, F_ChangedReasonId )"
        " VALUES ( %d ,%f ,%d ,%lld ,%d ,%d )";

static const std::string cs_sql_insert_straw_changed =
    " INSERT INTO [%s] ( F_MeasureId, F_Value, F_Quality, F_ChangedTime, F_ChangedSourceId, F_ChangedReasonId )"
        " VALUES ( %d ,'%s' ,%d ,%lld ,%d ,%d )";

// * T_*_SAGE
static const std::string cs_sql_create_monsb_save_table =
    "CREATE TABLE \"T_MONSB_SAVE_LOG\" (\n"
        "  [F_ChangedIndex] int NOT NULL, \n"
        "  [F_Count] int NOT NULL, \n"
        "  [F_Quality] int NOT NULL, \n"
        "  [F_ChangedTime] int64 NOT NULL);";

static const std::string cs_sql_insert_monsb_save =
    "INSERT INTO [T_MONSB_SAVE_LOG]( [F_ChangedIndex], [F_Count], [F_Quality], [F_ChangedTime])\n"
        "VALUES( %d, %d, %d, %lld);";

static const std::string cs_sql_create_ycadd_save_table =
    "CREATE TABLE \"T_YCADD_SAVE_LOG\" (\n"
        "  [F_ChangedIndex] int NOT NULL, \n"
        "  [F_Count] int NOT NULL, \n"
        "  [F_Quality] int NOT NULL, \n"
        "  [F_ChangedTime] int64 NOT NULL);";

static const std::string cs_sql_insert_ycadd_save =
    "INSERT INTO [T_YCADD_SAVE_LOG]( [F_ChangedIndex], [F_Count], [F_Quality], [F_ChangedTime])\n"
        "VALUES( %d, %d, %d, %lld);";

static const std::string cs_sql_create_straw_save_table =
    "CREATE TABLE \"T_STRAW_SAVE_LOG\" (\n"
        "  [F_ChangedIndex] int NOT NULL, \n"
        "  [F_Count] int NOT NULL, \n"
        "  [F_Quality] int NOT NULL, \n"
        "  [F_ChangedTime] int64 NOT NULL);";

static const std::string cs_sql_insert_straw_save =
    "INSERT INTO [T_straw_SAVE_LOG]( [F_ChangedIndex], [F_Count], [F_Quality], [F_ChangedTime])\n"
        "VALUES( %d, %d, %d, %lld);";

static CxDatabase *f_oDbsDb = NULL;

static const char c_sMonsbChangedIndex[]  = "MonsbChangedIndex";
static const char c_sYcaddChangedIndex[]  = "YcaddChangedIndex";
static const char c_sStrawChangedIndex[]  = "StrawChangedIndex";
static size_t f_monsbChangedIndex = GM_UINT_MAX;
static size_t f_ycaddChangedIndex = GM_UINT_MAX;
static size_t f_strawChangedIndex = GM_UINT_MAX;

static map<string, string> f_tableNames = map<string, string>();

static bool fn_isExistTable(const string &sTableName)
{
    string sCount = f_oDbsDb->queryValue(CxString::format("select count(*) from sqlite_master where type='table' and name='%s';", sTableName.c_str()));
    return CxString::toInt32(sCount) > 0;
}

static std::string fn_getMeasureValue(const int &value)
{
    return CxString::toString(value);
}

static std::string fn_getMeasureValue(const double &value)
{
    return CxString::toString(value);
}

static std::string fn_getMeasureValue(const StrawValue &value)
{
//    return std::string(value.value, sizeof(value.value));
    int datalen = strlen(value.value) > ci_straw_length ? ci_straw_length : strlen(value.value);
    return std::string(value.value, datalen);
}

static std::string
fn_getMeasureChanngedSql(int iMeasureId, const int &value, int iQuality, msepoch_t dtChangedTime, int iChangedSourceId, int iChangedReasonId)
{
    string sTableName = CxString::toHexstring(iMeasureId);
    return CxString::format(cs_sql_insert_monsb_changed.c_str(), sTableName.c_str(), iMeasureId, value, iQuality, dtChangedTime, iChangedSourceId, iChangedReasonId);
}

static std::string
fn_getMeasureChanngedSql(int iMeasureId, const double &value, int iQuality, msepoch_t dtChangedTime, int iChangedSourceId, int iChangedReasonId)
{
    string sTableName = CxString::toHexstring(iMeasureId);
    return CxString::format(cs_sql_insert_ycadd_changed.c_str(), sTableName.c_str(), iMeasureId, value, iQuality, dtChangedTime, iChangedSourceId, iChangedReasonId);
}

static std::string
fn_getMeasureChanngedSql(int iMeasureId, const StrawValue &value, int iQuality, msepoch_t dtChangedTime, int iChangedSourceId, int iChangedReasonId)
{
    string sTableName = CxString::toHexstring(iMeasureId);
    int datalen = strlen(value.value) > ci_straw_length ? ci_straw_length : strlen(value.value);
    string sValue = std::string(value.value, datalen);
    return CxString::format(cs_sql_insert_straw_changed.c_str(), sTableName.c_str(), iMeasureId, sValue.c_str(), iQuality, dtChangedTime, iChangedSourceId, iChangedReasonId);
}

template<typename T>
static void
fn_insertChangeds_impl(const typename MeasureShareMemory<T>::ChangedData *const oChangedBegin, const typename MeasureShareMemory<T>::ChangedData *const oChangedEnd, const std::string &sSqlInsert)
{
    assert(f_oDbsDb != NULL);
    const typename MeasureShareMemory<T>::ChangedData *oChangedData = oChangedBegin;
    std::vector<std::string> sSqls;
    while (oChangedData < oChangedEnd)
    {
        string sSql = fn_getMeasureChanngedSql(oChangedData->measureId, oChangedData->value,
                                               oChangedData->quality, oChangedData->changedTime,
                                               oChangedData->changedSourceId, oChangedData->changedReasonId);
        sSqls.push_back(sSql);
        oChangedData++;
    }
    //
    if (sSqls.size() > 0)
    {
        //存本地盘
        if (f_oDbsDb)
        {
            int iResult = f_oDbsDb->execSqlList(sSqls);
            if (iResult <= 0)
            {
                cxLogDebug() << "fn_insertChangeds_impl-error count:" << sSqls.size();
                for (int i = 0; i < sSqls.size(); ++i)
                {
                    cxLogDebug() << "fn_insertChangeds_impl-error sql:" << sSqls[i];
                }
                cxLogDebug() << "fn_insertChangeds_impl-error end.";
            }
        }
    }
}

static void
fn_insertChangeds_monsb(const MonsbShareMemory::ChangedData *const oChangedBegin, const typename MonsbShareMemory::ChangedData *const oChangedEnd)
{
    fn_insertChangeds_impl<int>(oChangedBegin, oChangedEnd, cs_sql_insert_monsb_changed);
}

static void
fn_insertChangeds_ycadd(const YcaddShareMemory::ChangedData *const oChangedBegin, const typename YcaddShareMemory::ChangedData *const oChangedEnd)
{
    fn_insertChangeds_impl<double>(oChangedBegin, oChangedEnd, cs_sql_insert_ycadd_changed);
}

static void
fn_insertChangeds_straw(const StrawShareMemory::ChangedData *const oChangedBegin, const typename StrawShareMemory::ChangedData *const oChangedEnd)
{
    fn_insertChangeds_impl<StrawValue>(oChangedBegin, oChangedEnd, cs_sql_insert_straw_changed);
}

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

static std::string fn_getMeasureTableSql(const int &value, int iMeasureId)
{
    string sMeasureId = CxString::toHexstring(iMeasureId);
    return CxString::format(cs_sql_create_monsb_changed_table.c_str(), sMeasureId.c_str());
}

static std::string fn_getMeasureTableSql(const double &value, int iMeasureId)
{
    string sMeasureId = CxString::toHexstring(iMeasureId);
    return CxString::format(cs_sql_create_ycadd_changed_table.c_str(), sMeasureId.c_str());
}

static std::string fn_getMeasureTableSql(const StrawValue &value, int iMeasureId)
{
    string sMeasureId = CxString::toHexstring(iMeasureId);
    return CxString::format(cs_sql_create_straw_changed_table.c_str(), sMeasureId.c_str());
}

template<typename T>
static int fn_measureLogTablesInit(typename MeasureShareMemory<T>::MemoryManagerBase *oMemoryManager)
{
    if (f_oDbsDb == NULL)
    {
        return -1;
    }
    //MemoryManager
    size_t *oMeasuresIndex;
    size_t *oMeasuresIndexEnd;
    oMeasuresIndex = oMemoryManager->measuresIndexBegin();
    oMeasuresIndexEnd = oMemoryManager->measuresIndexEnd();
    typename MeasureShareMemory<T>::MeasureData *oMeasureDataBegin = oMemoryManager->measuresDataBegin();
    typename MeasureShareMemory<T>::MeasureData measureData;
    //SendData
    int iCount = oMeasuresIndexEnd - oMeasuresIndex;
    if (iCount != oMemoryManager->measureCount())
    {
        cxDebug() << "fn_measureLogTablesInit error !!! internal error : iCount != oMemoryManager->measureCount()";
        return -1;
    }
    if (iCount <= 0 || iCount > 1024 * 100)
    {
        cxDebug() << "fn_measureLogTablesInit error !!! iCount[<1024*100] error : iCount: " << iCount;
        return -1;
    }
    int rCount = 0;
    int iIndex = 0;
    while (oMeasuresIndex < oMeasuresIndexEnd)
    {
        measureData = *(oMeasureDataBegin + (*oMeasuresIndex));
//        //MeasureId
//        measureData.measureId;
//        //值
//        measureData.value;
//        //品质
//        measureData.quality;
//        //修改时间
//        measureData.changedTime;
        string sTableName = CxString::toHexstring(measureData.measureId);
        if (!CxContainer::contain(f_tableNames, sTableName))
        {
            if (f_oDbsDb->execSql(fn_getMeasureTableSql(measureData.value, measureData.measureId)))
            {
                cxPrompt() << "Create Table [" << sTableName << "] Success!";
                ++rCount;
            }
            else
            {
                cxPrompt() << "Create Table [" << sTableName << "] Fail!";
                return -1;
            }
        }
        oMeasuresIndex++;
        iIndex++;
    }
    return rCount;
}

void RtdbService::initDb()
{
    f_oDbsDb = CxDatabase::getDefaultDb();
    if (f_oDbsDb == NULL)
    {
        cxPrompt()
            << "Error Error Error!!! Can Not Open DefaultDb. DataBaseSource Config Invalid. Application will exit now !!!";
        CxApplication::exit();
        return;
    }
    if (!f_oDbsDb->openDatabase())
    {
        cxPrompt() << "Error Error Error!!! Can Not Open DefaultDb By DataBaseSource: " << f_oDbsDb->connectSource()
                   << " fail . application will exit now !!!";
        CxApplication::exit();
        return;
    }
    if (!fn_isExistTable("T_SYS_CONFIG"))
    {
        if (f_oDbsDb->execSql(cs_sql_create_system_config_table) <= 0)
        {
            cxPrompt() << "Error Error Error!!! Can Not Create Table 'T_SYS_CONFIG'!"
                       << " application will exit now !!!";
            CxApplication::exit();
            return;
        }
    }
    if (!fn_isExistTable("T_MONSB_SAVE_LOG"))
    {
        if (f_oDbsDb->execSql(cs_sql_create_monsb_save_table) <= 0)
        {
            cxPrompt() << "Error Error Error!!! Can Not Create Table 'T_MONSB_SAVE_LOG'!"
                       << " application will exit now !!!";
            CxApplication::exit();
            return;
        }
    }
    if (!fn_isExistTable("T_YCADD_SAVE_LOG"))
    {
        if (f_oDbsDb->execSql(cs_sql_create_ycadd_save_table) <= 0)
        {
            cxPrompt() << "Error Error Error!!! Can Not Create Table 'T_YCADD_SAVE_LOG'!"
                       << " application will exit now !!!";
            CxApplication::exit();
            return;
        }
    }
    if (!fn_isExistTable("T_STRAW_SAVE_LOG"))
    {
        if (f_oDbsDb->execSql(cs_sql_create_straw_save_table) <= 0)
        {
            cxPrompt() << "Error Error Error!!! Can Not Create Table 'T_STRAW_SAVE_LOG'!"
                       << " application will exit now !!!";
            CxApplication::exit();
            return;
        }
    }
    {
        string sSql;
        string sValue;
        sSql = "SELECT F_ChangedIndex FROM T_MONSB_SAVE_LOG ORDER BY F_ChangedTime DESC LIMIT 1;";
        sValue = f_oDbsDb->queryValue(sSql);
        f_monsbChangedIndex = sValue.empty() ? GM_UINT_MAX : CxString::toInt32(sValue);
        sSql = "SELECT F_ChangedIndex FROM T_YCADD_SAVE_LOG ORDER BY F_ChangedTime DESC LIMIT 1;";
        sValue = f_oDbsDb->queryValue(sSql);
        f_ycaddChangedIndex = sValue.empty() ? GM_UINT_MAX : CxString::toInt32(sValue);
        sSql = "SELECT F_ChangedIndex FROM T_STRAW_SAVE_LOG ORDER BY F_ChangedTime DESC LIMIT 1;";
        sValue = f_oDbsDb->queryValue(sSql);
        f_strawChangedIndex = sValue.empty() ? GM_UINT_MAX : CxString::toInt32(sValue);
    }
    f_tableNames = f_oDbsDb->loadVerticalTableObject("select name , sql from sqlite_master;");
    // init measureLogTables
    if (fn_measureLogTablesInit<int>(MonsbShareMemory::memoryManager()) < 0)
    {
        cxPrompt() << "Error Error Error!!! fn_measureLogTablesInit<int>() fail . application will exit now !!!";
        CxApplication::exit();
        return;
    }
    // init measureLogTables
    if (fn_measureLogTablesInit<double>(YcaddShareMemory::memoryManager()) < 0)
    {
        cxPrompt() << "Error Error Error!!! fn_measureLogTablesInit<double>() fail . application will exit now !!!";
        CxApplication::exit();
        return;
    }
    // init measureLogTables
    if (!fn_measureLogTablesInit<StrawValue>(StrawShareMemory::memoryManager()) < 0)
    {
        cxPrompt() << "Error Error Error!!! fn_measureLogTablesInit<StrawValue>() fail . application will exit now !!!";
        CxApplication::exit();
        return;
    }
}

void RtdbService::start()
{
    CxApplication::pushProcessCallBack(RtdbService::doStart);
}

void RtdbService::stop()
{
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
                size_t iMonsbChangedIndex = MonsbShareMemory::memoryManager()->checkChangeds(f_monsbChangedIndex, fn_insertChangeds_monsb);
                if (iMonsbChangedIndex != f_monsbChangedIndex)
                {
                    if (f_oDbsDb)
                    {
                        f_oDbsDb->execSql(CxString::format(cs_sql_insert_monsb_save.c_str(), f_monsbChangedIndex, f_monsbChangedIndex-iMonsbChangedIndex, 0, CxTime::currentMsepoch()));
                    }
                    f_monsbChangedIndex = iMonsbChangedIndex;
                }
            }
            break;
        case 2:
            if (YcaddShareMemory::memoryManager()->isValid())
            {
                size_t iYcaddChangedIndex = YcaddShareMemory::memoryManager()->checkChangeds(f_ycaddChangedIndex, fn_insertChangeds_ycadd);
                if (iYcaddChangedIndex != f_ycaddChangedIndex)
                {
                    if (f_oDbsDb)
                    {
                        f_oDbsDb->execSql(CxString::format(cs_sql_insert_ycadd_save.c_str(), f_ycaddChangedIndex, f_ycaddChangedIndex-iYcaddChangedIndex, 0, CxTime::currentMsepoch()));
                    }
                    f_ycaddChangedIndex = iYcaddChangedIndex;
                }
            }
            break;
        case 3:
            if (StrawShareMemory::memoryManager()->isValid())
            {
                size_t iStrawChangedIndex = StrawShareMemory::memoryManager()->checkChangeds(f_strawChangedIndex, fn_insertChangeds_straw);
                if (iStrawChangedIndex != f_strawChangedIndex)
                {
                    if (f_oDbsDb)
                    {
                        f_oDbsDb->execSql(CxString::format(cs_sql_insert_straw_save.c_str(), f_strawChangedIndex, f_strawChangedIndex-iStrawChangedIndex, 0, CxTime::currentMsepoch()));
                    }
                    f_strawChangedIndex = iStrawChangedIndex;
                }
            }
            break;
        case 4:

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

    RtdbService::initDb();
}

