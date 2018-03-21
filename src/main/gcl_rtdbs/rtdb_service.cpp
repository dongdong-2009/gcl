#include "rtdb_service.h"


#include <vdi/measure_sharememory_all.h>
#include <sqlext.h>
#include <script/gcl_lua_global.h>
#include <script/gcl_lua_shm.h>

using namespace std;

//静态初始化
RtdbService * fn_oDbsRealtimedata()
{
    CxApplication::registStartFunction(RtdbService::start);
    CxApplication::registStopFunction(RtdbService::stop);
    GeneralAccess::registerMessageCommandDeal(RtdbService::dealMessage);
    GeneralAccess::registerFileWriteDeal(RtdbService::dealFileWrite);

    static RtdbService m;
    return &m;
}

static RtdbService * f_oDbsRealtimedata = fn_oDbsRealtimedata();

const std::string cs_dbs_dbname = "rtdb.db";

const std::string cs_sql_create_monsb_changed_table =
        " CREATE TABLE [T_MONSB_CHANGED_LOG] ("
        " [F_MeasureId] int NOT NULL, "
        " [F_Value] int, "
        " [F_Quality] int, "
        " [F_ChangedTime] int64 NOT NULL,"
        " [F_ChangedSourceId] int, "
        " [F_ChangedReasonId] int NOT NULL);";

const std::string cs_sql_create_ycadd_changed_table =
        " CREATE TABLE [T_YCADD_CHANGED_LOG] ("
        " [F_MeasureId] int NOT NULL, "
        " [F_Value] double, "
        " [F_Quality] int, "
        " [F_ChangedTime] int64 NOT NULL,"
        " [F_ChangedSourceId] int, "
        " [F_ChangedReasonId] int NOT NULL);";

const std::string cs_sql_create_straw_changed_table =
        " CREATE TABLE [T_STRAW_CHANGED_LOG] ("
        " [F_MeasureId] int NOT NULL, "
        " [F_Value] VARCHAR(128), "
        " [F_Quality] int, "
        " [F_ChangedTime] int64 NOT NULL,"
        " [F_ChangedSourceId] int, "
        " [F_ChangedReasonId] int NOT NULL);";

const std::string cs_sql_insert_monsb_changed =
        " INSERT INTO [T_MONSB_CHANGED_LOG] ( F_MeasureId, F_Value, F_Quality, F_ChangedTime, F_ChangedSourceId, F_ChangedReasonId )"
        " VALUES ( ? ,? ,? ,? ,? ,? )";

const std::string cs_sql_insert_ycadd_changed =
        " INSERT INTO [T_YCADD_CHANGED_LOG] ( F_MeasureId, F_Value, F_Quality, F_ChangedTime, F_ChangedSourceId, F_ChangedReasonId )"
        " VALUES ( ? ,? ,? ,? ,? ,? )";

const std::string cs_sql_insert_straw_changed =
        " INSERT INTO [T_STRAW_CHANGED_LOG] ( F_MeasureId, F_Value, F_Quality, F_ChangedTime, F_ChangedSourceId, F_ChangedReasonId )"
        " VALUES ( ? ,? ,? ,? ,? ,? )";


static CxDatabase * f_oDbsDb = NULL;


static size_t f_monsbChangedIndex = GM_UINT_MAX;
static size_t f_ycaddChangedIndex = GM_UINT_MAX;
static size_t f_strawChangedIndex = GM_UINT_MAX;


static std::string fn_getMeasureValue(const int & value)
{
    return CxString::toString(value);
}

static std::string fn_getMeasureValue(const double & value)
{
    return CxString::toString(value);
}

static std::string fn_getMeasureValue(const StrawValue & value)
{
//    return std::string(value.value, sizeof(value.value));
    int datalen = strlen(value.value)>ci_straw_length?ci_straw_length:strlen(value.value);
    return std::string(value.value, datalen);
}

template<typename T>
static void fn_insertChangeds_impl(const typename MeasureShareMemory<T>::ChangedData * const oChangedBegin, const typename MeasureShareMemory<T>::ChangedData * const oChangedEnd, const std::string & sSqlInsert)
{
    assert(f_oDbsDb != NULL);

    const typename MeasureShareMemory<T>::ChangedData * oChangedData = oChangedBegin;
    std::vector<std::vector<std::string> > rows;

    while (oChangedData < oChangedEnd)
    {
        std::vector<std::string> row;
        row.push_back(CxString::toString(oChangedData->measureId));
        row.push_back(fn_getMeasureValue(oChangedData->value));
        row.push_back(CxString::toString(oChangedData->quality));
        row.push_back(CxString::toString(oChangedData->changedTime));
        row.push_back(CxString::toString(oChangedData->changedSourceId));
        row.push_back(CxString::toString(oChangedData->changedReasonId));
        rows.push_back(row);
        oChangedData++;
    }

    //
    if(rows.size()>0)
    {
        //存本地盘
        if(f_oDbsDb)
        {
            f_oDbsDb->saveTable(sSqlInsert, rows, NULL, true);
        }
        if(f_oDbsRealtimedata)f_oDbsRealtimedata->dealChangeData(rows);
    }
}


static void fn_insertChangeds_monsb(const MonsbShareMemory::ChangedData * const oChangedBegin, const typename MonsbShareMemory::ChangedData * const oChangedEnd)
{
    fn_insertChangeds_impl<int>(oChangedBegin, oChangedEnd, cs_sql_insert_monsb_changed);
}

static void fn_insertChangeds_ycadd(const YcaddShareMemory::ChangedData * const oChangedBegin, const typename YcaddShareMemory::ChangedData * const oChangedEnd)
{
    fn_insertChangeds_impl<double>(oChangedBegin, oChangedEnd, cs_sql_insert_ycadd_changed);
}

static void fn_insertChangeds_straw(const StrawShareMemory::ChangedData * const oChangedBegin, const typename StrawShareMemory::ChangedData * const oChangedEnd)
{
    fn_insertChangeds_impl<StrawValue>(oChangedBegin, oChangedEnd, cs_sql_insert_straw_changed);
}

void RtdbService::measureShareMemoryAfterLoad(int iMeasureType)
{
    switch (iMeasureType)
    {
    case MeasureType_Monsb:
        f_monsbChangedIndex = MonsbShareMemory::memoryManager()->changedCount()-1;
        break;
    case MeasureType_Ycadd:
        f_ycaddChangedIndex = YcaddShareMemory::memoryManager()->changedCount()-1;
        break;
    case MeasureType_Straw:
        f_strawChangedIndex = StrawShareMemory::memoryManager()->changedCount()-1;
        break;
    default:
        break;
    }
}

void RtdbService::measureShareMemoryBeforeClose(int iMeasureType)
{
}

bool RtdbService::openDatabase()
{
    f_oDbsDb = CxDatabase::getDefaultDb();
    if(f_oDbsDb==NULL)
    {
        return false;
    }

    if(f_oDbsDb->openDatabase())
    {
        string sCount;
        sCount = f_oDbsDb->queryValue("select count(*) from sqlite_master where type='table' and name='T_MONSB_CHANGED_LOG';");
        if (CxString::toInt32(sCount) <= 0)
        {
            cxPromptCheck(f_oDbsDb->execSql(cs_sql_create_monsb_changed_table), return false);
        }
        sCount = f_oDbsDb->queryValue("select count(*) from sqlite_master where type='table' and name='T_YCADD_CHANGED_LOG';");
        if (CxString::toInt32(sCount) <= 0)
        {
            cxPromptCheck(f_oDbsDb->execSql(cs_sql_create_ycadd_changed_table), return false);
        }
        sCount = f_oDbsDb->queryValue("select count(*) from sqlite_master where type='table' and name='T_STRAW_CHANGED_LOG';");
        if (CxString::toInt32(sCount) <= 0)
        {
            cxPromptCheck(f_oDbsDb->execSql(cs_sql_create_straw_changed_table),return false);
        }
        return true;
    }else{
        return false;
    }

}

void RtdbService::start()
{
    CxTimerManager::startTimer(RtdbService::timerTimeout, 100);
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
    switch(iCase)
    {
    case 0:
        //加载数据库，加载不成功退出
        if(!f_oDbsDb)
        {
            if (!RtdbService::openDatabase())
            {
                cxPrompt() << "open database " << cs_dbs_dbname << " fail . application will exit now !!!";
                CxApplication::exit();
            }
        }
        break;
    case 1:
        if (MonsbShareMemory::memoryManager()->isValid())
        {
            f_monsbChangedIndex = MonsbShareMemory::memoryManager()->checkChangeds(f_monsbChangedIndex, fn_insertChangeds_monsb);
        }
        break;
    case 2:
        if (YcaddShareMemory::memoryManager()->isValid())
        {
            f_ycaddChangedIndex = YcaddShareMemory::memoryManager()->checkChangeds(f_ycaddChangedIndex, fn_insertChangeds_ycadd);
        }
        break;
    case 3:
        if (StrawShareMemory::memoryManager()->isValid())
        {
            f_strawChangedIndex = StrawShareMemory::memoryManager()->checkChangeds(f_strawChangedIndex, fn_insertChangeds_straw);
        }
        break;
    case 4:

        break;
    default:
        break;
    }


    iTimes++;
}

int RtdbService::dealMessage(const PsmExplainer *oPsm, const string &sCommand, const string &sParam, const PsmAttach *oAttach)
{
    return FALSE;
}

int RtdbService::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    return FALSE;
}

int RtdbService::dealChangeData(std::vector<std::vector<string> > &rows)
{
    return GclLuaShm::dealChangeData(rows);
}

