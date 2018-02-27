#include "db_sync.h"


#include <sqlext.h>


using namespace std;


//静态初始化
DbSync * fn_oDbsRealtimedata()
{
    CxApplication::registStartFunction(DbSync::start);
    CxApplication::registStopFunction(DbSync::stop);
    GeneralAccess::registerMessageCommandDeal(DbSync::dealMessage);
    GeneralAccess::registerFileWriteDeal(DbSync::dealFileWrite);

    static DbSync m;
    return &m;
}


static DbSync * f_oDbsRealtimedata = fn_oDbsRealtimedata();

const std::string cs_dbs_dbname = "rtdb.db";

const std::string cs_sql_create_sync_config =
    " CREATE TABLE [T_DB_SYNC_CONFIG] ("
    " [F_ID] INTEGER PRIMARY KEY   AUTOINCREMENT,"
    " [F_SOURCE_TABLE_NAME] VARCHAR(64),"
    " [F_SOURCE_PRIMARY_KEY] VARCHAR(64),"
    " [F_SOURCE_KEYS] VARCHAR(1024),"
    " [F_TARGET_TABLE_NAME] VARCHAR(64),"
    " [F_TARGET_PRIMARY_KEY] VARCHAR(64),"
    " [F_TARGET_KEYS] VARCHAR(1024),"
    " [F_SYNC_INTERVAL] INT );";

const std::string cs_sql_create_sync_log =
    " CREATE TABLE [T_DB_SYNC_%s] ("
    " [F_ID] INTEGER PRIMARY KEY AUTOINCREMENT,"
    " [F_PRIMARY_KEY] VARCHAR(64),"
    " [F_ACTION_TIME] INT64,"
    " [F_ACTION] INT );";


const std::string cs_sql_insert_sync_log =
        " INSERT INTO [%s] ( F_PRIMARY_KEY, F_ACTION_TIME, F_ACTION )"
        " VALUES ( ? ,? ,? )";


CxDatabase * f_oDbSyncLog = NULL;
CxDatabase * f_oDbSource = NULL;
CxDatabase * f_oDbTarget = NULL;


size_t f_monsbChangedIndex = GM_UINT_MAX;
size_t f_ycaddChangedIndex = GM_UINT_MAX;
size_t f_strawChangedIndex = GM_UINT_MAX;


class TableSync : ICxTimerNotify
{
public:
    int F_ID;
    string F_SOURCE_TABLE_NAME;
    string F_SOURCE_PRIMARY_KEY;
    string F_SOURCE_KEYS;
    vector<string> F_SOURCE_KEYS_;
    string F_TARGET_TABLE_NAME;
    string F_TARGET_PRIMARY_KEY;
    string F_TARGET_KEYS;
    vector<string> F_TARGET_KEYS_;
    int F_SYNC_INTERVAL;

    CxTimer _timer;

public:
    void setSOURCE_KEYS(const string & value)
    {
        F_SOURCE_KEYS = value;
        F_SOURCE_KEYS_ = CxString::split(value, ' ');
    }

    void setTARGET_KEYS(const string & value)
    {
        F_TARGET_KEYS = value;
        F_TARGET_KEYS_ = CxString::split(value, ' ');
    }

public:
    TableSync() {}

    void timer_timeOut(const CxTimer* oTimer)
    {
        std::vector<std::vector<std::string> > rows;
        if (f_oDbSource->loadSql(sSql, rows)>0)
        {
            for (size_t i = 0; i < rows.size(); ++i)
            {
                std::vector<std::string> & row = rows.at(i);
                for (size_t j = 0; j < row.size(); ++j)
                {

                }
            }
        }
    }

private:


};


template<typename T>
void fn_insertChangeds_impl(const typename MeasureShareMemory<T>::ChangedData * const oChangedBegin, const typename MeasureShareMemory<T>::ChangedData * const oChangedEnd, const std::string & sSqlInsert)
{
    assert(f_oDbSyncLog != NULL);

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
    f_oDbSyncLog->saveTable(sSqlInsert, rows, NULL, true);
}

bool DbSync::initDatabase()
{
    f_oDbSyncLog = CxDatabaseManager::getDbByIndex(1);
    f_oDbSource = CxDatabaseManager::getDbByIndex(2);
    f_oDbTarget = CxDatabaseManager::getDbByIndex(3);

    GM_ASSER_OBJECT(f_oDbSyncLog);
    GM_ASSER_OBJECT(f_oDbSource);
    GM_ASSER_OBJECT(f_oDbTarget);

    if(f_oDbSyncLog->openDatabase())
    {

        " CREATE TABLE [T_DB_SYNC_CONFIG] ("
        " [F_ID] INTEGER PRIMARY KEY   AUTOINCREMENT,"
        " [F_SOURCE_TABLE_NAME] VARCHAR(64),"
        " [F_SOURCE_PRIMARY_KEY] VARCHAR(64),"
        " [F_SOURCE_KEYS] VARCHAR(1024),"
        " [F_TARGET_TABLE_NAME] VARCHAR(64),"
        " [F_TARGET_PRIMARY_KEY] VARCHAR(64),"
        " [F_TARGET_KEYS] VARCHAR(1024),"
        " [F_SYNC_INTERVAL] INT );";

        _sqlQueries = oDb->queryToMapVector("select * from T_DB_SYNC_CONFIG;");

        _urlSqls.clear();
        for (size_t i = 0; i < _sqlQueries.size(); ++i)
        {
            const std::map<string, string> & sqlQuery = _sqlQueries.at(i);
            std::map<string, string>::const_iterator it = sqlQuery.find(cs_url);
            if (it != sqlQuery.end())
            {
                _urlSqls[it->second] = i;
            }
        }
        cxPrompt() << "load db.T_SYS_SQLTEXT_QUERY to _sqlQueries.size=" << _sqlQueries.size();


        std::vector<std::vector<std::string> > rows;
        if (f_oDbSyncLog->loadTable(sTableName, rows)>0)
        {
            for (size_t i = 0; i < rows.size(); ++i)
            {
                std::vector<std::string> & row = rows.at(i);
                for (size_t j = 0; j < row.size(); ++j)
                {

                }
            }
        }

        string sCount;
        sCount = f_oDbSyncLog->queryValue("select count(*) from sqlite_master where type='table' and name='T_MONSB_CHANGED_LOG';");
        if (CxString::toInt32(sCount) <= 0)
        {
            cxPromptCheck(f_oDbSyncLog->execSql(cs_sql_create_monsb_changed_table), return false);
        }
        sCount = f_oDbSyncLog->queryValue("select count(*) from sqlite_master where type='table' and name='T_YCADD_CHANGED_LOG';");
        if (CxString::toInt32(sCount) <= 0)
        {
            cxPromptCheck(f_oDbSyncLog->execSql(cs_sql_create_ycadd_changed_table), return false);
        }
        sCount = f_oDbSyncLog->queryValue("select count(*) from sqlite_master where type='table' and name='T_STRAW_CHANGED_LOG';");
        if (CxString::toInt32(sCount) <= 0)
        {
            cxPromptCheck(f_oDbSyncLog->execSql(cs_sql_create_straw_changed_table), return false);
        }
        return true;
    }else{
        return false;
    }

}

void DbSync::start()
{
    CxTimerManager::startTimer(DbSync::timerTimeout, 100);
}

void DbSync::stop()
{
    CxTimerManager::stopTimer(DbSync::timerTimeout);
}

void DbSync::timerTimeout(int iInterval)
{
    static int iTimes = 0;

    int iCase;
    iCase = iTimes % 5;
    if (iCase == 0)
    {
        //加载数据库，加载不成功退出
        if(! f_oDbSyncLog)
        {
            if (! DbSync::reopenDatabase())
            {
                cxPrompt() << "open database " << cs_dbs_dbname << " fail . application will exit now !!!";
                CxApplication::exit();
            }
        }
    }
    else if (iCase == 1)
    {
        if (MonsbShareMemory::memoryManager()->isValid())
        {
            f_monsbChangedIndex = MonsbShareMemory::memoryManager()->checkChangeds(f_monsbChangedIndex, fn_insertChangeds_monsb);
        }
    }
    else if (iCase == 2)
    {
        if (YcaddShareMemory::memoryManager()->isValid())
        {
            f_ycaddChangedIndex = YcaddShareMemory::memoryManager()->checkChangeds(f_ycaddChangedIndex, fn_insertChangeds_ycadd);
        }
    }
    else if (iCase == 3)
    {
        if (StrawShareMemory::memoryManager()->isValid())
        {
            f_strawChangedIndex = StrawShareMemory::memoryManager()->checkChangeds(f_strawChangedIndex, fn_insertChangeds_straw);
        }
    }

    iTimes++;
}

int DbSync::dealMessage(const PsmExplainer *oPsm, const string &sCommand, const string &sParam, const PsmAttach *oAttach)
{
    return FALSE;
}

int DbSync::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    return FALSE;
}

