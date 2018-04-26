/*

// ICS数据库数据请求的 json格式：
// http的URL请求：
http://10.31.0.15:8821/ics.cgi?fncode=req.sql.select.&filetype=json
fncode = req.rtdata_v101
filetype = json

                    message_command                                 message_param
远程数据库SQL数据运行	post.sql.exec.                                  type=1;session=x;param2=x;sql=
                    post.sql.exec.dbname（dbname为空时表示默认数据库）	session是自定义的会话ID或编号，sql放最后
【未必有】返回执行结果	resp.sql.exec.                                  session=x;result=
                    resp.sql.exec.dbname（dbname为空时表示默认数据库）  "session是自定义的会话ID或编号 result数据库的执行结果"


远程数据库SQL数据查询	req.sql.select.                                 session=x;param2=x;sql=
                    req.sql.select.dbname（dbname为空时表示默认数据库）	session是自定义的会话ID或编号，sql放最后
返回select后TABLE的E文件）	resp_sql_select_[dbname]_[session]_[20150828121212].json
                    "dbname为空时表示默认数据库，session是自定义的会话ID或编号
                    20150828121212：是YYYYMMDDHHMMSS，年月日时分秒"


// ICS数据库数据请求的 json格式：
{
  "sql":"select * from table",
  "url": "req.570.xx",
}

// ICS数据库数据请求返回的 json格式：
{
  "sql":"select * from table",
  "url": "req.570.xx",
  "data":
  [
    {
    "field1": "value1",
    "field2": "value2"
    },
    {
    "field1": "value1",
    "field2": "value2"
    }
  ]
}

 */

#include <fcgi_config.h>
#include <fcgiapp.h>
#include <fcgios.h>

#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTTPResponseHeader.h>
#include <cgicc/HTMLClasses.h>
#include <contrib/FCgiIO.h>

#include <ccxx/ccxx.h>

#include <psm/general_access.h>

#include <ccxx/cxdatabase_sqlite.h>
#include <ccxx/cxdatabase_odbc.h>

#include <vdi/common_service.h>

using namespace std;
using namespace cgicc;
using namespace rapidjson;

typedef struct FcgiRequest
{
    FCGX_Request * request;
    FCgiIO * cgiio;
    Cgicc * cgicc;
    int enabled;
    int index;
    volatile int ref;
} FcgiRequest;

const char * cs_command_sql_select = "SELECT * FROM ICS.T_ICS_SYS_SQLTEXT_QUERY;";
const char * cs_url = "url";
const char * cs_sql = "sql";


const int f_requestCount = 1024;
static FcgiRequest f_requests[f_requestCount];
static int f_requestDescriptorIndex = -1;


static int f_iDbHeartJumpInterval = 60000*60;
static string f_sDbHeartJumpSql;
static msepoch_t f_dtDbHeartJumpRec = 0;

void fn_initFcgiRequests()
{
    for (int i = 0; i < f_requestCount; ++i)
    {
        FcgiRequest * oFcgiRequest = f_requests+i;
        oFcgiRequest->request = NULL;
        oFcgiRequest->cgiio = NULL;
        oFcgiRequest->cgicc = NULL;
        oFcgiRequest->enabled = TRUE;
        oFcgiRequest->index = i;
        oFcgiRequest->ref = 0;
    }
}

FcgiRequest * fn_takeFcgiRequest()
{
    for (int i = 0; i < f_requestCount; ++i)
    {
        FcgiRequest * oFcigRequest = f_requests+i;
        if (oFcigRequest->ref == 0)
        {
            return oFcigRequest;
        }
    }
    return NULL;
}

void fn_holdFcgiRequest(FcgiRequest * oFcgiRequest)
{
    oFcgiRequest->ref++;
}

void fn_unholdFcgiRequest(FcgiRequest * oFcgiRequest)
{
    oFcgiRequest->ref--;
}

FcgiRequest * fn_acceptFcgiRequest(int iRequestDescriptorIndex)
{
    FcgiRequest * oFcgiRequest = fn_takeFcgiRequest();
    if (oFcgiRequest==NULL)
    {
        cxWarning() << "Error ! Error ! Error ! FcgiRequest Cache Full !!!";
        cout        << "Error ! Error ! Error ! FcgiRequest Cache Full !!!" << endl;
        CxApplication::raiseExit();
        return NULL;
    }
    fn_holdFcgiRequest(oFcgiRequest);
    //only data init
    oFcgiRequest->request = new FCGX_Request();
    FCGX_InitRequest(oFcgiRequest->request, iRequestDescriptorIndex, 0);
    int rAccept = FCGX_Accept_r(oFcgiRequest->request);
    if ( rAccept < 0 || oFcgiRequest->request == NULL || oFcgiRequest->request->ipcFd < 0 )
    {
        fn_unholdFcgiRequest(oFcgiRequest);
        if (oFcgiRequest->request)
        {
            cxWarning() << CxTime::currentMsepochString() << " Error ! Error ! Error ! FCGX_Accept_r fail, f_requests.index: " << oFcgiRequest->index << ", oRequest.ref: " << oFcgiRequest->ref << ", ipcFd: " << oFcgiRequest->request->ipcFd;
            cout        << CxTime::currentMsepochString() << " Error ! Error ! Error ! FCGX_Accept_r fail, f_requests.index: " << oFcgiRequest->index << ", oRequest.ref: " << oFcgiRequest->ref << ", ipcFd: " << oFcgiRequest->request->ipcFd << endl;
            FCGX_Finish_r(oFcgiRequest->request);
            delete oFcgiRequest->request;
            oFcgiRequest->request = NULL;
        }
        else
        {
            cxWarning() << CxTime::currentMsepochString() << " Error ! Error ! Error ! FCGX_Accept_r fail, f_requests.index: " << oFcgiRequest->index << ", oRequest.ref: " << oFcgiRequest->ref << ", oFcgiRequest->request: NULL";
            cout        << CxTime::currentMsepochString() << " Error ! Error ! Error ! FCGX_Accept_r fail, f_requests.index: " << oFcgiRequest->index << ", oRequest.ref: " << oFcgiRequest->ref << ", oFcgiRequest->request: NULL" << endl;
        }
        oFcgiRequest = NULL;
    }
    return oFcgiRequest;
}

void fn_finishFcgiRequest(FcgiRequest * oFcgiRequest)
{
    fn_unholdFcgiRequest(oFcgiRequest);
    if (oFcgiRequest->ref == 0)
    {
        if (oFcgiRequest->cgicc)
        {
            delete oFcgiRequest->cgicc;
            oFcgiRequest->cgicc = NULL;
        }
        if (oFcgiRequest->cgiio)
        {
            delete oFcgiRequest->cgiio;
            oFcgiRequest->cgiio = NULL;
        }
        if (oFcgiRequest->request)
        {
            FCGX_Finish_r(oFcgiRequest->request);
            delete oFcgiRequest->request;
            oFcgiRequest->request = NULL;
        }
    }
    if (oFcgiRequest->ref < 0)
    {
        oFcgiRequest->ref = 0;
    }
}



//* Thread Receive Sql
class CgiSqlDealThread : public CxJoinableThread
{
public:
    volatile bool isStarted;
    volatile bool isBusy;

private:
    vector<std::map<string, string> > _sqlQueries;
    std::map<string, int> _urlSqls;
    std::map<string, vector<int> > _typeSqls;
    std::map<string, vector<int> > _titleSqls;

    CxMutex _lock;
    std::vector<FcgiRequest *> _bufferFcgiRequest1;
    std::vector<FcgiRequest *> _bufferFcgiRequest2;
    std::vector<FcgiRequest *> * volatile _pushBufferFcgiRequests;
    std::vector<FcgiRequest *> * volatile _popBufferFcgiRequests;
    volatile int _fcgiRequestCount;

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
    volatile int _sqlCommandCount;

    CxSingleWait _singleWait;

public:
    CgiSqlDealThread()
    {
        isStarted = false;
        isBusy = false;
    }
    ~CgiSqlDealThread()
    {
    }

    void init()
    {
        _pushBufferFcgiRequests = & _bufferFcgiRequest1;
        _popBufferFcgiRequests = & _bufferFcgiRequest2;
        _fcgiRequestCount = 0;

        _pushBufferSqlCommands = & _bufferSqlCommands1;
        _popBufferSqlCommands = & _bufferSqlCommands2;
        _pushBufferSqlParams = & _bufferSqlParams1;
        _popBufferSqlParams = & _bufferSqlParams2;
        _pushBufferSqlAttachs = & _bufferSqlAttachs1;
        _popBufferSqlAttachs = & _bufferSqlAttachs2;
        _sqlCommandCount = 0;
    }

    inline void stop() { isStarted = false; _singleWait.signal(); join(); }

    //SqlCommand is SqlUrl(url)
    void pushSqlDeal(FcgiRequest * oFcgiRequest)
    {
        {
            CxMutexScope lock(_lock);
            fn_holdFcgiRequest(oFcgiRequest);
            _pushBufferFcgiRequests->push_back(oFcgiRequest);
            _fcgiRequestCount++;
        }
        _singleWait.signal();
    }

    void pushSqlCommand(const string & sCommand, const string & sParam, const PsmAttach *oAttach)
    {
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
            _sqlCommandCount++;
        }
        _singleWait.signal();
    }

private:
    void loadSqltexts()
    {
        //*load db.T_SQLTEXT_COMMAND to _commandSqls
        CxDatabase * oDb = CxDatabaseManager::getDefaultDb();
        if (oDb)
        {
            _sqlQueries = oDb->queryToMapVector(cs_command_sql_select);

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
            cxPrompt() << "load ICS.T_ICS_SYS_SQLTEXT_QUERY to _sqlQueries.size=" << _sqlQueries.size();
        }
        else
        {
            cxPrompt() << "load ICS.T_ICS_SYS_SQLTEXT_QUERY fail!!! for CxDatabaseManager::getDefaultDb() is NULL! please config the db setting!!";
        }
    }

    void outDealFinish(FcgiRequest * oFcgiRequest, const string & sOut)
    {
        FCgiIO & IO = * oFcgiRequest->cgiio;
        IO << HTTPResponseHeader("HTTP/1.1", 200, "OK")
              .addHeader("Access-Control-Allow-Origin", "*")
              .addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization")
              .addHeader("Content-Type", "application/json")
           << sOut << endl;

        fn_finishFcgiRequest(oFcgiRequest);
    }

protected:
    void run()
    {
        //CxDatabase * oDb = CxDatabaseManager::getDefaultDb();

        loadSqltexts();

        isStarted = true;
        while ( isStarted )
        {
            isBusy = false;
            _singleWait.wait();
            isBusy = true;

            //*deal fcgi
            if (_fcgiRequestCount>0)
            {
                CxMutexScope lock(_lock);
                std::vector<FcgiRequest *> * tmpRequests = _pushBufferFcgiRequests;
                _pushBufferFcgiRequests = _popBufferFcgiRequests;
                _popBufferFcgiRequests = tmpRequests;
                _fcgiRequestCount = 0;
            }

            if (_popBufferFcgiRequests->size()>100)
            {
                for (size_t i = 0; i < _popBufferFcgiRequests->size(); ++i)
                {
                    FcgiRequest * oFcgiRequest = _popBufferFcgiRequests->at(i);
                    fn_finishFcgiRequest(oFcgiRequest);
                }
                _popBufferFcgiRequests->clear();
                cxPrompt() << "Error ! Error ! Error ! CgiSqlDealThread FcgiRequest->size()>100 , FcgiRequest->clear()!!!";
                cxPrompt() << "Error ! Error ! Error ! CgiSqlDealThread FcgiRequest->size()>100 , FcgiRequest->clear()!!!";
                cxPrompt() << "Error ! Error ! Error ! CgiSqlDealThread FcgiRequest->size()>100 , FcgiRequest->clear()!!!";
            }

            for (size_t i = 0; i < _popBufferFcgiRequests->size(); ++i)
            {
                FcgiRequest * oFcgiRequest = _popBufferFcgiRequests->at(i);
                Cgicc & CGI = * oFcgiRequest->cgicc;

                string sFnCode;
                if (1>0)
                {
                    cgicc::const_form_iterator it = CGI.getElement("fncode");
                    if (it != CGI.getElements().end())
                    {
                        sFnCode = CxString::toLower(it->getValue());
                    }
                }

                //回应的内容
                string sOut;

                //要从请求的参数或数据中提取或算出 sql
                string sPostData = CGI.getEnvironment().getPostData();
                //json文件的类 的 Document
                Document d;
                d.Parse(sPostData.data());
                if (d.HasParseError())
                {
                    outDealFinish(oFcgiRequest, "{\"error\":\"can not parse the json string!\"}");
                    continue;
                }

                string sSql = CxRapidJson::findMemberToString(d, cs_sql);
                if (sSql.empty())
                {
                    string sUrl = CxRapidJson::findMemberToString(d, cs_url);
                    if (sUrl.empty())
                    {
                        outDealFinish(oFcgiRequest, "{\"error\":\"can not find sql or url in the json string!\"}");
                        continue;
                    }
                    else
                    {
                        sOut = CxString::format("{\"url\":\"%s\",\"data\":", sUrl.c_str());
                    }
                    //根据 url 找相关 SQL 语句
                    std::map<string, int>::const_iterator it = _urlSqls.find(sUrl);
                    if (it != _urlSqls.end())
                    {
                        int iSql = it->second;
                        //iSql必须是_sqlQueries的Index
                        //if (iSql>=0 && iSql<_sqlQueries.size())
                        std::map<string, string> & sqlQuery = _sqlQueries[iSql];
                        //sqlQuery内必须有sql
                        sSql = sqlQuery[cs_sql];
                    }
                    else
                    {
                        sOut += "null,\"reason\":\"can not find sql!\"}";
                        outDealFinish(oFcgiRequest, sOut);
                        continue;
                    }
                }
                else
                {
                    sOut = CxString::format("{\"sql\":\"%s\",\"data\":", sSql.c_str());
                }

                CxDatabase * oDb = CxDatabaseManager::getDefaultDb();
                if (oDb)
                {
                    bool bConnectTypeEmpty = oDb->connectType().empty();
                    bool bDataIsUtf8 = false;
                    bool bSqlIsLoad = false;
                    if (sSql.size()>0)
                    {
                        if (sFnCode.find(".exec.") != std::string::npos)
                        {
                            //根据 sql 运行
                            if ((!bConnectTypeEmpty)&&(CxEncoding::isUtf8(sSql.c_str(),sSql.length())))
                            {
                                sSql = CxEncoding::utf8ToGb2312(sSql); //加入转编码
                            }
                            int iResult = oDb->execSql(sSql);

                            if (sFnCode.find(".onlyresult.") != std::string::npos)
                            {
                                sOut = "{\"data\":";
                            }

                            sOut += CxString::toString(iResult);
                            sOut.push_back('}');
                        }
                        else
                        {
                            //根据 sql 加载 数据
                            string sData = oDb->queryToJsonString(sSql);
                            cxPrompt() << "Deal-Sql-Request : sSql : " << sSql;
                            cxPrompt() << "Deal-Sql-Request : sData : " << sData;
                            bDataIsUtf8 = CxEncoding::isUtf8(sData.c_str(),sData.length());
                            bSqlIsLoad = true;
                            if ((!bConnectTypeEmpty)&&(!bDataIsUtf8))
                            {
                                sData = CxEncoding::gb2312ToUtf8(sData); //加入转编码
                            }
                            if (sData.size()>0)
                            {
                                sOut += sData;
                                sOut.push_back('}');
                            }
                            else
                            {
                                sOut += "null,\"reason\":\"has sql but can not load data!\"}";
                            }
                        }
                    }
                    else
                    {
                        sOut += "null,\"reason\":\"sql is empty!\"}";
                    }

                    if (bSqlIsLoad)
                    {
                        cxPrompt() << "Deal-Sql-Request : sFnCode" << sFnCode
                                   << ", connectType: " << oDb->connectType()
                                   << ", bDataIsUtf8: " << bDataIsUtf8
                                   << ", sOut: " << sOut;
                    }
                }
                else
                {
                    sOut += "null,\"reason\":\"CxDatabaseManager::getDefaultDb() is NULL! please config the db setting!\"}";
                }

                outDealFinish(oFcgiRequest, sOut);
            }

            if (_popBufferFcgiRequests->size()>0)
            {
                _popBufferFcgiRequests->clear();
            }

            //*deal sql
            if (_sqlCommandCount>0)
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
                _sqlCommandCount=0;
            }

            if (_popBufferSqlCommands->size()>100)
            {
                _popBufferSqlCommands->clear();
                _popBufferSqlParams->clear();
                _popBufferSqlAttachs->clear();
                cxPrompt() << "Error ! Error ! Error ! CgiSqlDealThread SqlCommands->size()>100 , SqlCommands->clear()!!!";
                cxPrompt() << "Error ! Error ! Error ! CgiSqlDealThread SqlCommands->size()>100 , SqlCommands->clear()!!!";
                cxPrompt() << "Error ! Error ! Error ! CgiSqlDealThread SqlCommands->size()>100 , SqlCommands->clear()!!!";
            }

            for (size_t i = 0; i < _popBufferSqlCommands->size(); ++i)
            {
                string & sSqlCommand = _popBufferSqlCommands->at(i);
                string & sSqlParam = _popBufferSqlParams->at(i);
                PsmAttach & sqlAttach = _popBufferSqlAttachs->at(i);
                CxDatabase * oDb = CxDatabaseManager::getDefaultDb();
                if (! oDb) break;

                std::string sNow = CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0);
                string sParam = CxString::newString(sSqlParam);
                string sSql = CxString::tokenLeft(sParam, "sql=");

                if (sSql.empty()) break;

                if (sSqlCommand.find(".sql.exec.") != string::npos)
                {
                    msepoch_t tmStart = CxTime::currentMsepoch();
                    int iResult = oDb->execSql(sSql);
                    int iSpent = CxTime::currentMsepoch()-tmStart;
                    cxPrompt() << "execSql spent_time=" << iSpent << "; result=" << iResult << "; sql=" << sSql;
                }
                else if (sSqlCommand.find(".sql.select.") != string::npos)
                {
                    msepoch_t dtSql = CxTime::currentMsepoch();
                    std::vector<std::vector<std::string> > rows;
                    std::vector<std::string> fields;
                    int iResult = oDb->loadSql(sSql,rows,&fields);
                    if (iResult>=0)
                    {
                        f_dtDbHeartJumpRec = CxTime::currentMsepoch();
                    }
                    cxPrompt() << "loadSql sql_cost_time=" << CxTime::milliSecondDifferToNow(dtSql) << "; result=" << iResult << "; sql=" << sSql;
                }
                else
                {
                    cxPrompt() << "dealMessage sCommand Fail. command=" << sSqlCommand << " out of range!!!";
                }
            }

            if (_popBufferSqlCommands->size()>0)
            {
                _popBufferSqlCommands->clear();
                _popBufferSqlParams->clear();
                _popBufferSqlAttachs->clear();
            }
        }
        isStarted = false;
    }

};

static CgiSqlDealThread * f_oCgiSqlDealThread = NULL;



class FastcgiDealThread : public CxJoinableThread
{
public:
    FastcgiDealThread()
    {
        _isStarted = false;
    }
    ~FastcgiDealThread()
    {
    }

    inline void stop()
    {
        _isStarted = false;
        if (f_requestDescriptorIndex >= 0)
        {
            OS_Close(f_requestDescriptorIndex, true);
        }
        join();
    }

protected:
    void outHtml(FCgiIO & IO, Cgicc & CGI, const std::string & sTitle, const std::string & sInfo)
    {
        IO << HTTPHTMLHeader() << HTMLDoctype( HTMLDoctype::eStrict ) << endl
           << html().set( "lang", "en" ).set( "dir", "ltr" ) << endl;

        // Set up the page's header and title.
        IO << head() << endl
           << title() << "gcl_fastcgi return info" << title() << endl
           << head() << endl;

        // Start the HTML body
        IO << body() << endl;

        // Print out a message
        IO << h1("gcl_fastcgi info: ") << endl
           << br() << endl
           << "title: " << sTitle << br() << endl
           << "info: " << sInfo << br() << endl;

        IO  << "Form Elements:" << br() << endl;

        for(const_form_iterator i = CGI.getElements().begin();
            i != CGI.getElements().end(); ++i )
            IO << i->getName() << " = " << i->getValue() << br() << endl;

        // Close the document
        IO << body() << html();
    }

    void outJsonFinish(FcgiRequest * oFcgiRequest, const string & sOut)
    {
        FCgiIO & IO = * oFcgiRequest->cgiio;
        IO << HTTPResponseHeader("HTTP/1.1", 200, "OK")
              .addHeader("Access-Control-Allow-Origin", "*")
              .addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization")
              .addHeader("Content-Type", "application/json")
           << sOut << endl;

        fn_finishFcgiRequest(oFcgiRequest);
    }

    void run()
    {
        _isStarted = true;
        while (_isStarted)
        {
            //阻塞操作 : accept
            FcgiRequest * oFcgiRequest = fn_acceptFcgiRequest(f_requestDescriptorIndex);
            if (oFcgiRequest==NULL)
            {
                //needtodo
                continue;
            }
            FCGX_Request * oRequest = oFcgiRequest->request;
            oFcgiRequest->cgiio = new FCgiIO(*oRequest);
            oFcgiRequest->cgicc = new Cgicc(oFcgiRequest->cgiio);
            FCgiIO & IO = * oFcgiRequest->cgiio;
            Cgicc & CGI = * oFcgiRequest->cgicc;
            try
            {
                char * remote_addr = FCGX_GetParam("REMOTE_ADDR", oRequest->envp);
//                cxDebug() << remote_addr;
                string sRequestMethod = CxString::toLower(CGI.getEnvironment().getRequestMethod());
                //*跨域的支持
                if (sRequestMethod.find("options") != string::npos)
                {
                    IO << HTTPResponseHeader("HTTP/1.1", 0, "OPTIONS Authorization")
                          .addHeader("Access-Control-Allow-Origin", "*")
                          .addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS")
                          .addHeader("Access-Control-Allow-Headers", "POWERED-BY-AID,Content-Type")
                          .addHeader("Access-Control-Max-Age", "30")
                       << endl;
/*
                    cout << HTTPResponseHeader("HTTP/1.1", 401, "Unauthorized")
                            .addHeader("WWW-Authenticate", "NTLM")
                            .addHeader("WWW-Authenticate",  "Basic realm=\"cgicc\"");
                    response.addHeader("Access-Control-Allow-Origin", "*");
                    response.addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
                    response.setMimeType("application/json");
*/
                }
                else
                {
                    //http://125.89.65.238:8821/ics/x.cig?fncode=req.sql
                    string sFnCode;
                    {
                        cgicc::const_form_iterator it = CGI.getElement("fncode");
                        if (it != CGI.getElements().end())
                        {
                            sFnCode = CxString::toLower(it->getValue());
                        }
//                        cxPrompt()<<sFnCode;
                    }
                    //*url的参数command中带有sql的
                    if (sFnCode.find(".sql.") != string::npos)
                    {
                        if (! f_oCgiSqlDealThread->isBusy)
                        {
                            f_oCgiSqlDealThread->pushSqlDeal(oFcgiRequest);
                        }
                        else
                        {
                            outJsonFinish(oFcgiRequest, "{\"error\":\"db is busy!\"}");
                        }
                    }
                    else
                    {
                        cxLog() << "!!!warning. do not know sFnCode: " << sFnCode;
                    }
                }

                /*
                for(const_form_iterator i = CGI.getElements().begin(); i != CGI.getElements().end(); ++i )
                {
                    IO << i->getName() << " = " << i->getValue() << br() << endl;
                }

    //做测试、实验用
                std::string s;
                CxFile::load("c:/1.xml", s);
                IO << HTTPContentHeader("xml") << s << endl;
    //needtodo: 这句只多了一个 endl，但前端解释出问题。要检查。IO << HTTPContentHeader("xml") << endl << s << endl;
                FCGX_PutS("Content-Type: xml\n\n", request.out);
                //        FCGX_PutS("Content-Type: text/xml\n\n", request.out);
                FCGX_PutStr(s.data(), s.size(), request.out);
                FCGX_PutS("\n", request.out);
                */

                /*
                //test1
                unsigned count = 0;
                // Output the HTTP headers for an HTML document, and the HTML 4.0 DTD info
                IO << HTTPHTMLHeader() << HTMLDoctype( HTMLDoctype::eStrict ) << endl
                   << html().set( "lang", "en" ).set( "dir", "ltr" ) << endl;

                // Set up the page's header and title.
                IO << head() << endl
                   << title() << "GNU cgicc v" << CGI.getVersion() << title() << endl
                   << head() << endl;

                // Start the HTML body
                IO << body() << endl;

                // Print out a message
                IO << h1("Cgicc/FastCGI Test") << endl
                   << "PID: " << getpid() << br() << endl
                   << "count: " << count++ << br() << endl;

                IO  << "Form Elements:" << br() << endl;

                for(const_form_iterator i = CGI.getElements().begin();
                    i != CGI.getElements().end(); ++i )
                    IO << i->getName() << " = " << i->getValue() << br() << endl;

                // Close the document
                IO << body() << html();
                */
            }
            catch(...)
            {
                // handle error condition
                fn_finishFcgiRequest(oFcgiRequest);
            }

            fn_finishFcgiRequest(oFcgiRequest);
        }
        _isStarted = false;
    }

public:
    volatile bool _isStarted;


};

static FastcgiDealThread * f_oFastcgiDealThread = NULL;

void fn_timeout_dbHeartJump(int)
{
    //reconnect
    CxDatabase *oDb = CxDatabaseManager::getDefaultDb();
    if (oDb)
    {
        string sConnectSource = oDb->connectSource();
        string sConnectType = oDb->connectType();
        bool ret = CxDatabaseManager::closeDatabase(sConnectSource);
        if (ret == true)
        {
            CxDatabase *new_oDb = CxDatabaseManager::getDefaultDb();
            if (new_oDb)
            {
                std::map<string, string> sConnectParams = CxString::splitToMap("", CxGlobal::middleChar, CxGlobal::splitChar);
                ret = new_oDb->openDatabase(sConnectSource, sConnectType, true, &sConnectParams);
                cxDebug()<<"reconnect_db : ret="<<ret;
            }
        }
    }

    if (f_dtDbHeartJumpRec>0 && CxTime::milliSecondDifferToNow(f_dtDbHeartJumpRec)>f_iDbHeartJumpInterval*3)
    {
    }
    else
    {
        if (f_oCgiSqlDealThread)
        {
            f_oCgiSqlDealThread->pushSqlCommand(".sql.select.", f_sDbHeartJumpSql, NULL);
        }
    }
}

int main(int argc, const char *argv[])
{
    CxApplication::init(argc, argv);

    //心跳服务初始化
    CommonService::init();

    FCGX_Init();
    f_requestDescriptorIndex = FCGX_OpenSocket(":9172", 0);
    if (f_requestDescriptorIndex<0)
    {
        cxPrompt() << "Error ! Error ! Error ! Can not open socket 9000. Please Enter Any Key To Exit.";
        CxConsoleInterinfo::waitInputLine(CxApplication::processEvents, 30 * 1000);
        return 0;
    }
    //only data init
    fn_initFcgiRequests();

    CxDatabase * oDb = CxDatabaseManager::getDefaultDb();
    if (oDb) oDb->openDatabase();

    if (! f_oFastcgiDealThread)
    {
        f_oFastcgiDealThread = new FastcgiDealThread();
#ifdef GM_OS_WIN
        //THREAD_PRIORITY_TIME_CRITICAL
        int iPriority = THREAD_PRIORITY_HIGHEST;
#else
        //SCHED_OTHER,SCHED_FIFO,SCHED_RR
        int iPriority = 2;
#endif
        f_oFastcgiDealThread->start(iPriority);
    }

    if (! f_oCgiSqlDealThread)
    {
        f_oCgiSqlDealThread = new CgiSqlDealThread();
        f_oCgiSqlDealThread->init();
        f_oCgiSqlDealThread->start();
        if (f_iDbHeartJumpInterval>0 || f_sDbHeartJumpSql.size()>0)
        {
             CxTimerManager::startTimer(fn_timeout_dbHeartJump, f_iDbHeartJumpInterval);
             cxPrompt() << "startTimer dbHeartJump:interval=" << f_iDbHeartJumpInterval;
        }
    }

    int rExec = CxApplication::exec();

    if (f_oFastcgiDealThread)
    {
        f_oFastcgiDealThread->stop();
        delete f_oFastcgiDealThread;
    }

    if (f_oCgiSqlDealThread)
    {
        f_oCgiSqlDealThread->stop();
        delete f_oCgiSqlDealThread;
    }


    return rExec;
}
