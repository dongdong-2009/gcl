
#include "db_service.h"

#include <ccxx/cxdatabase_sqlite.h>



using namespace std;


static volatile int f_gds_sf_count;
static CxMutex f_gds_sf_lock;
static std::vector<CxFileSystem::PathInfo > f_gds_sf_bufferFileInfos1;
static std::vector<CxFileSystem::PathInfo > f_gds_sf_bufferFileInfos2;
static std::vector<std::vector<std::string> > f_gds_sf_bufferFileDatas1;
static std::vector<std::vector<std::string> > f_gds_sf_bufferFileDatas2;
static std::vector<PsmAttach> f_gds_sf_bufferFileAttachs1;
static std::vector<PsmAttach> f_gds_sf_bufferFileAttachs2;
static std::vector<CxFileSystem::PathInfo > * volatile f_gds_sf_pushBufferFileInfos;
static std::vector<CxFileSystem::PathInfo > * volatile f_gds_sf_popBufferFileInfos;
static std::vector<std::vector<std::string> > * volatile f_gds_sf_pushBufferFileDatas;
static std::vector<std::vector<std::string> > * volatile f_gds_sf_popBufferFileDatas;
static std::vector<PsmAttach> * volatile f_gds_sf_pushBufferFileAttachs;
static std::vector<PsmAttach> * volatile f_gds_sf_popBufferFileAttachs;

static int f_gds_heartJumpInterval = 0;
static string f_gds_heartJumpSql;


static DbService * fn_oGdsServer()
{
    CxApplication::registStartFunction(DbService::start);
    CxApplication::registStopFunction(DbService::stop);
    GeneralAccess::registerMessageCommandDeal(DbService::dealMessage);
    GeneralAccess::registerFileWriteDeal(DbService::dealFileWrite);

    static DbService m;
    return &m;
}

static DbService * m_pTicketSvr = fn_oGdsServer();


int fn_gds_sf_pushFile(const CxFileSystem::PathInfo & pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    CxMutexScope lock(f_gds_sf_lock);
    f_gds_sf_pushBufferFileInfos->push_back(pathInfo);
    f_gds_sf_pushBufferFileDatas->push_back(fileData);
    if (oAttach)
    {
        f_gds_sf_pushBufferFileAttachs->push_back(* oAttach);
    }
    else
    {
        f_gds_sf_pushBufferFileAttachs->push_back(PsmAttach());
    }
    f_gds_sf_count++;
    return TRUE;
}

void fn_gds_sf_run()
{
    if (f_gds_sf_count>0)
    {
        CxMutexScope lock(f_gds_sf_lock);
        std::vector<CxFileSystem::PathInfo > * tmpInfos = f_gds_sf_pushBufferFileInfos;
        f_gds_sf_pushBufferFileInfos = f_gds_sf_popBufferFileInfos;
        f_gds_sf_popBufferFileInfos = tmpInfos;
        std::vector<std::vector<std::string> > * tmpDatas = f_gds_sf_pushBufferFileDatas;
        f_gds_sf_pushBufferFileDatas = f_gds_sf_popBufferFileDatas;
        f_gds_sf_popBufferFileDatas = tmpDatas;
        std::vector<PsmAttach> * tmpAttachs = f_gds_sf_pushBufferFileAttachs;
        f_gds_sf_pushBufferFileAttachs = f_gds_sf_popBufferFileAttachs;
        f_gds_sf_popBufferFileAttachs = tmpAttachs;
        f_gds_sf_count=0;
    }

    if (f_gds_sf_popBufferFileInfos->size()>0)
    {
        if (f_gds_sf_popBufferFileInfos->size() < 100)
        {
            for (size_t i = 0; i < f_gds_sf_popBufferFileInfos->size(); ++i)
            {
                const CxFileSystem::PathInfo & fileInfo = f_gds_sf_popBufferFileInfos->at(i);
                const std::vector<std::string> & fileData = f_gds_sf_popBufferFileDatas->at(i);
                const PsmAttach & fileAttach = f_gds_sf_popBufferFileAttachs->at(i);

                int iResult = GeneralAccess::postFile(fileInfo, fileData, & fileAttach);

                DbService::outGdsPrompt() << "GeneralAccess::postFileData fileName=" << fileInfo.fileName << "; fileSize=" << fileInfo.fileSize << "; result=" << iResult;
            }
        }
        else
        {
            DbService::outGdsPrompt() << "Error ! Error ! Error ! fn_gds_sf_run f_gds_sf_popBufferFileInfos->size()>100 , f_gds_sf_popBufferFileInfos->clear()!!!";
            DbService::outGdsPrompt() << "Error ! Error ! Error ! fn_gds_sf_run f_gds_sf_popBufferFileInfos->size()>100 , f_gds_sf_popBufferFileInfos->clear()!!!";
            DbService::outGdsPrompt() << "Error ! Error ! Error ! fn_gds_sf_run f_gds_sf_popBufferFileInfos->size()>100 , f_gds_sf_popBufferFileInfos->clear()!!!";
        }
        f_gds_sf_popBufferFileInfos->clear();
        f_gds_sf_popBufferFileDatas->clear();
        f_gds_sf_popBufferFileAttachs->clear();
    }
}


//* Thread Receive Sql
class GdsSqlRunThread : public CxJoinableThread
{
private:
    volatile int _status;
    volatile int _count;
    CxMutex _lock;
    std::vector<std::string> _bufferSqlCommands1;
    std::vector<std::string> _bufferSqlCommands2;
    std::vector<std::string> _bufferSqlParams1;
    std::vector<std::string> _bufferSqlParams2;
    std::vector<PsmAttach> _bufferSqlAttachs1;
    std::vector<PsmAttach> _bufferSqlAttachs2;
    std::vector<std::string> * volatile _pushBufferSqlCommands;
    std::vector<std::string> * volatile _popBufferSqlCommands;
    std::vector<std::string> * volatile _pushBufferSqlParams;
    std::vector<std::string> * volatile _popBufferSqlParams;
    std::vector<PsmAttach> * volatile _pushBufferSqlAttachs;
    std::vector<PsmAttach> * volatile _popBufferSqlAttachs;

public:
    void init()
    {
        _status = 1;
        _count = 0;
        _pushBufferSqlCommands = & _bufferSqlCommands1;
        _popBufferSqlCommands = & _bufferSqlCommands2;
        _pushBufferSqlParams = & _bufferSqlParams1;
        _popBufferSqlParams = & _bufferSqlParams2;
        _pushBufferSqlAttachs = & _bufferSqlAttachs1;
        _popBufferSqlAttachs = & _bufferSqlAttachs2;
    }

    inline void stop() { _status = 0; join(); }

    int pushSqlCommand(const string & sCommand, const string & sParam, const PsmAttach *oAttach)
    {
        CxMutexScope lock(_lock);
        _pushBufferSqlCommands->push_back(CxString::newString(sCommand));
        _pushBufferSqlParams->push_back(CxString::newString(sParam));
        if (oAttach)
        {
            _pushBufferSqlAttachs->push_back(* oAttach);
        }
        else
        {
            _pushBufferSqlAttachs->push_back(PsmAttach());
        }
        _count++;
        return TRUE;
    }

 protected:
    void run() {
        _status = 1;
        while ( _status )
        {
            if (_count>0)
            {
                CxMutexScope lock(_lock);
                std::vector<string> * tmpSqlCommands = _pushBufferSqlCommands;
                _pushBufferSqlCommands = _popBufferSqlCommands;
                _popBufferSqlCommands = tmpSqlCommands;
                std::vector<std::string> * tmpSqlParams = _pushBufferSqlParams;
                _pushBufferSqlParams = _popBufferSqlParams;
                _popBufferSqlParams = tmpSqlParams;
                std::vector<PsmAttach> * tmpAttachs = _pushBufferSqlAttachs;
                _pushBufferSqlAttachs = _popBufferSqlAttachs;
                _popBufferSqlAttachs = tmpAttachs;
                _count=0;
            }

            if (_popBufferSqlCommands->size()>100)
            {
                _popBufferSqlCommands->clear();
                _popBufferSqlParams->clear();
                _popBufferSqlAttachs->clear();
                DbService::outGdsPrompt() << "Error ! Error ! Error ! GdsSqlRunThread SqlCommands->size()>100 , SqlCommands->clear()!!!";
                DbService::outGdsPrompt() << "Error ! Error ! Error ! GdsSqlRunThread SqlCommands->size()>100 , SqlCommands->clear()!!!";
                DbService::outGdsPrompt() << "Error ! Error ! Error ! GdsSqlRunThread SqlCommands->size()>100 , SqlCommands->clear()!!!";
            }

            for (size_t i = 0; i < _popBufferSqlCommands->size(); ++i)
            {
                string & sSqlCommand = _popBufferSqlCommands->at(i);
                string & sSqlParam = _popBufferSqlParams->at(i);
                PsmAttach & sqlAttach = _popBufferSqlAttachs->at(i);

                std::string sNow = CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0);
                string sParam = CxString::newString(sSqlParam);
                string sSql = CxString::tokenLeft(sParam, "sql=");

                if (sSqlCommand.find("post.sql.exec.") != string::npos)
                {
                    msepoch_t tmStart = CxTime::currentMsepoch();
                    int iResult = CxDatabase::getDefaultDb()->execSql(sSql);
                    int iSpent = CxTime::currentMsepoch()-tmStart;
                    DbService::outGdsPrompt() << "execSql spent_time=" << iSpent << "; result=" << iResult << "; sql=" << sSql;
                }
                else if (sSqlCommand.find("post.sql.select.") != string::npos)
                {
                    msepoch_t dtSql = CxTime::currentMsepoch();
                    std::vector<std::vector<std::string> > rows;
                    std::vector<std::string> fields;
                    int iResult = CxDatabase::getDefaultDb()->loadSql(sSql,rows,&fields);
                    msepoch_t dtEfile = CxTime::currentMsepoch();
                    if (iResult>=0)
                    {
                        std::map<string, string> sParams = CxString::splitToMap(sParam, '=', ';');
                        string sSession = CxContainer::value(sParams, string("session"));
                        string sDbname = CxString::unquote(sSqlCommand, "post.sql.select.", ".");
                        string sNow = CxTime::currentMsepochString(0, 0, 0);

                        CxFileSystem::PathInfo sendPathInfo;
                        sendPathInfo.fileName = CxString::format("resp_sql_select_%s_%s.efl", sSession.c_str(), sNow.c_str());

                        string sClassName = sSession.empty() ? "ygct" : sSession;
                        string sInstanceName = sClassName;
                        std::vector<string> sendFileData = CxEfile::toEfileBuffer(sClassName, sInstanceName, fields, rows);
                        sendPathInfo.fileSize = CxString::sizeOf(sendFileData);
                        PsmAttach sendAttach;
                        sendAttach.sourceId = sqlAttach.targetId;
                        sendAttach.targetId = sqlAttach.sourceId;

                        fn_gds_sf_pushFile(sendPathInfo, sendFileData, &sendAttach);
                    }
                    DbService::outGdsPrompt() << "loadSql sql_cost_time=" << dtEfile-dtSql << "; efile_cost_time" << CxTime::milliSecondDifferToNow(dtEfile) << "; result=" << iResult << "; sql=" << sSql;
                }
                else
                {
                    DbService::outGdsPrompt() << "dealMessage sCommand Fail. command=" << sSqlCommand << " out of range!!!";
                }
            }
            _popBufferSqlCommands->clear();
            _popBufferSqlParams->clear();
            _popBufferSqlAttachs->clear();

            CxThread::sleep(30);
        }

    }

    void exit() {
        _status = 0;
        CxJoinableThread::exit();
    }

};

GdsSqlRunThread * volatile f_oGdsSqlRunThread = NULL;






void DbService::start()
{
    f_gds_sf_count = 0;
    f_gds_sf_pushBufferFileInfos = & f_gds_sf_bufferFileInfos1;
    f_gds_sf_popBufferFileInfos = & f_gds_sf_bufferFileInfos2;
    f_gds_sf_pushBufferFileDatas = & f_gds_sf_bufferFileDatas1;
    f_gds_sf_popBufferFileDatas = & f_gds_sf_bufferFileDatas2;
    f_gds_sf_pushBufferFileAttachs = & f_gds_sf_bufferFileAttachs1;
    f_gds_sf_popBufferFileAttachs = & f_gds_sf_bufferFileAttachs2;

    if (! f_oGdsSqlRunThread)
    {
        f_oGdsSqlRunThread = new GdsSqlRunThread();
        f_oGdsSqlRunThread->init();
        f_oGdsSqlRunThread->start();
    }

    f_gds_heartJumpInterval = CxApplication::findConfig(CS_SectionProgramConfig, string("DbsHeartJumpInterval"), f_gds_heartJumpInterval);
    f_gds_heartJumpSql = CxApplication::findConfig(CS_SectionProgramConfig, string("DbsHeartJumpSql"), f_gds_heartJumpSql);
    if (f_gds_heartJumpInterval<=0)
    {
        f_gds_heartJumpInterval = CxApplication::findConfig(CS_SectionProgramConfig, string("DdsHeartJumpInterval"), f_gds_heartJumpInterval);
    }
    if (f_gds_heartJumpSql.size()==0)
    {
        f_gds_heartJumpSql = CxApplication::findConfig(CS_SectionProgramConfig, string("DdsHeartJumpSql"), f_gds_heartJumpSql);
    }
    if (f_gds_heartJumpInterval>=3000 && f_gds_heartJumpSql.size()>0)
    {
        CxTimerManager::startTimer(DbService::timerHeartJump, f_gds_heartJumpInterval);
    }

    CxTimerManager::startTimer(DbService::timerRunGds, 30);
}

void DbService::stop()
{
    if (f_oGdsSqlRunThread)
    {
        f_oGdsSqlRunThread->stop();
        f_oGdsSqlRunThread = NULL;
    }
}

void DbService::timerRunGds(int iInterval)
{
    fn_gds_sf_run();
}

void DbService::timerHeartJump(int iInterval)
{
    static PsmAttach attach;
    static string sCommand = "post.sql.exec.heartjump";
    static string sParams = "session=0;sql="+f_gds_heartJumpSql+";";
    f_oGdsSqlRunThread->pushSqlCommand(sCommand, sParams, &attach);
}

int DbService::dealMessage(const PsmExplainer *oPsm, const string &sCommand, const string &sParam, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);
    GM_ASSER_OBJECT(f_oGdsSqlRunThread);

    //sql的处理
    if(sCommand.find("post.sql.") != string::npos)
    {
        f_oGdsSqlRunThread->pushSqlCommand(sCommand, sParam, oAttach);
        return true;
    }
    return false;
}

int DbService::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    return TRUE;
}

