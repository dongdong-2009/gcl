/*

// GCL实时点的历史实时数据请求的 json格式：支持散列请求：rtlog_v101；数组请求是：rtlog_v102；返回时都统一用：rtlog_v001
// url 是全局统一资源名（可以通用在容器对象或实体对象中）
// mid 是实时库的实时点全局唯一id
// url和mid可以只有一个，两个同时都有时以mid为准
// http://10.31.0.15:8821/ics.cgi?fncode=req.rtlog_v101&filetype=json

// 散列请求：rtlog_v101
fncode = req.rtlog_v101
filetype = json

{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype": "rtlog_v101",
  "params":
  [
    {
    "url": "/fp/zyj/fgj01/rfid",
    "mid": 33556644,
    "dtday": "20180329",
    "dtbegin": 31343242341,
    "dtend": 23413241234
    },
    {
    "url": "/fp/zyj/fgj01/ypmm",
    "mid": 33556645,
    "dtday": "20180329",
    "dtbegin": 31343242341,
    "dtend": 23413241234
    }
  ]
}


// ics.json返回时都统一用：rtlog_v001
// "v": 数值；数组形式
// "q": 值的质量；数组形式
// "t": 值的时间,unix时间戳（1970到目前的毫秒数，服务器的当地时间）；数组形式
// "s": 实时数据信息来源的源ID,ChangedSourceId；数组形式
// "u": 实时数据信息来源的源url,ChangedSourceId；数组形式
// "r": ChangedReasonId；数组形式
// 可选属性"state":状态码，无或0时表示成功，其它值看具体数据字典
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype":"rtdata_v001",
  "data":[
    {
    "url":"/fp/zyj/fgj01/rfid",
    "mid":33556644,
    "dtday": "20180329",
    "dtbegin": 31343242341,
    "dtend": 23413241234,
    "log": "#logfile.text",
    "state":0
    },
    {
    "url":"/fp/zyj/fgj01/ypmm",
    "mid":33556645,
    "dtday": "20180329",
    "dtbegin": 31343242341,
    "dtend": 23413241234,
    "log": "#logfile.text",
    "state":0
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
#include <common/gcl_resource.h>

#include <vdi/common_service.h>

#include <main/gcl_rtdbs/rtdb_logfile.h>

using namespace std;
using namespace cgicc;
using namespace rapidjson;

const int ci_fileType_xml = 0;
const int ci_fileType_json = 1;

const char * cs_url = "url";
const char * cs_sql = "sql";
const char * cs_type = "type";
const char * cs_name = "name";
const char * cs_title = "title";
const char * cs_desc = "desc";
const char * cs_mid = "mid";
const char * cs_v = "v";
const char * cs_q = "q";
const char * cs_t = "t";
const char * cs_state = "state";
const char * cs_dtday = "dtday";
const char * cs_dtbegin = "dtbegin";
const char * cs_dtend = "dtend";


static std::map<std::string, int> f_urlMids;
static std::map<int, std::string> f_midUrls;

typedef struct FcgiRequest
{
    FCGX_Request * request;
    FCgiIO * cgiio;
    Cgicc * cgicc;
    int enabled;
    int index;
    volatile int ref;
} FcgiRequest;

const int f_requestCount = 1024;
static FcgiRequest f_requests[f_requestCount];
static int f_requestDescriptorIndex = -1;

static int f_iDbHeartJumpInterval = 60000*60;
static string f_sDbHeartJumpSql;
static msepoch_t f_dtDbHeartJumpRec = 0;


//{"measures":[{"url":"/fp/zyj/fgj01/rfid","mid":16777217},{"url":"/fp/zyj/fgj01/ypmm","mid":16777218}]}
//*can common
void fn_loadUrlMids2(string sFilePath, std::map<std::string, int> & urlMids, std::map<int, std::string> & midUrls)
{
    vector<string> sLines;
    CxFile::load(sFilePath, sLines);
    for (size_t i = 0; i < sLines.size(); ++i)
    {
        const string & sLine = sLines.at(i);
        size_t posMeasureId = string::npos;
        string sMeasureId = CxString::unquote(sLine, 0, "MeasureId=", ",", &posMeasureId);
        int iMid = CxString::toInt32(sMeasureId);
        if (iMid>0)
        {
            string sUrl = CxString::unquote(sLine, posMeasureId, "Url=", ",", &posMeasureId);
            if (sUrl.size()>0)
            {
                urlMids[sUrl] = iMid;
                midUrls[iMid] = sUrl;
            }
        }
    }
}

void fn_loadUrlMids()
{
    string sMeasurePath = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), CS_DirNameMeasure);
    fn_loadUrlMids2(CxFileSystem::mergeFilePath(sMeasurePath, CS_FileNameMonsbMeasure), f_urlMids, f_midUrls);
    fn_loadUrlMids2(CxFileSystem::mergeFilePath(sMeasurePath, CS_FileNameYcaddMeasure), f_urlMids, f_midUrls);
    fn_loadUrlMids2(CxFileSystem::mergeFilePath(sMeasurePath, CS_FileNameStrawMeasure), f_urlMids, f_midUrls);
    cxPrompt() << "load urlMids=" << f_urlMids.size() << "; midUrls=" << f_midUrls.size();
}

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


// # 获取实时数据返回（以数组形式），可以XML或JSON的方式
// # iFileType =0：表示XML方式； =1：表示JSON方式
string fn_rtlogGetString(const string & sUrl, int iMid, string sDtDay, int iFileType=0)
{
    string r;

    if (iFileType==ci_fileType_xml)
    {
    }
    else
    {
        string sBegin = CxString::format("{\"url\":\"%s\",\"mid\":%d,\"dtday\":\"%s\",\"log\":\"", sUrl.c_str(), iMid, sDtDay.c_str());
        string sBody = RtdbLogFile::loadMeasureChanged(iMid, sDtDay);
        string sEnd = "\",\"state\":1}";
        r = sBegin + sBody + sEnd;
    }

    return r;
}
string fn_rtlogGetString(const string & sUrl, int iMid, msepoch_t dtBeing, msepoch_t dtEnd, int iFileType=0)
{
    string r;

    if (iFileType==ci_fileType_xml)
    {
    }
    else
    {
        string sBegin = CxString::format("{\"url\":\"%s\",\"mid\":%d,\"dtbegin\":%lld,\"dtend\":%lld,\"log\":\"", sUrl.c_str(), iMid, dtBeing, dtEnd);
        string sBody = RtdbLogFile::loadMeasureChanged(iMid, dtBeing, dtEnd);
        string sEnd = "\",\"state\":1}";
        r = sBegin + sBody + sEnd;
    }

    return r;
}



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
                    if (1>0)
                    {
                        cgicc::const_form_iterator it = CGI.getElement("fncode");
                        if (it != CGI.getElements().end())
                        {
                            sFnCode = CxString::toLower(it->getValue());
                        }
//                        cxPrompt()<<sFnCode;
                    }
                    {
                        string sContentType = CGI.getEnvironment().getContentType();
//                        cxPrompt()<<sContentType;
                        //ContentType是json的请求处理
                        if (sContentType.find("json") != string::npos)
                        {
                            string sPostData = CGI.getEnvironment().getPostData();
                            Document d;
                            d.Parse(sPostData.data());
                            if (d.HasParseError())
                            {
                                outJsonFinish(oFcgiRequest, "{\"error\":\"can not parse the json string!\"}");
                                continue;
                            }

                            string sSession = CxRapidJson::findMemberToString(d, "session");
                            string sStructType = CxRapidJson::findMemberToString(d, "structtype");
                            string sOutBegin = CxString::format("{\"session\":\"%s\",\"structtype\":\"%s\",\"data\":[", sSession.c_str());
                            string sOut;
                            string sOutEnd = "]}";
                            //**structtype 为 rtdata_v101
                            cxDebug() << sStructType << " begin " << CxTime::currentMsepochString();
                                // # structtype rtlog_v101
                            if (sStructType.find("rtlog_v101") != string::npos)
                            {
                                sOutBegin = CxString::format(sOutBegin.c_str(), "rtlog_v001");
                                rapidjson::Value * vParams = CxRapidJson::findMember(d, "params");
                                if (vParams && vParams->IsArray())
                                {
                                    for (SizeType i = 0; i < vParams->Size(); ++i)
                                    {
                                        Value & vMeasure = (*vParams)[i];
                                        int iMid = 0;
                                        string sUrl = CxRapidJson::findMemberToString(vMeasure, cs_url);
                                        if (sUrl.size()>0)
                                        {
                                            std::map<string, int>::const_iterator it = f_urlMids.find(sUrl);
                                            if (it != f_urlMids.end())
                                            {
                                                iMid = it->second;
                                            }
                                            else
                                            {
                                                iMid = CxRapidJson::findMemberToInt(vMeasure, cs_mid);
                                            }
                                        }
                                        string sDtDay = CxRapidJson::findMemberToString(vMeasure, cs_dtday);
                                        if (sDtDay.size() > 0)
                                        {
                                            sOut += fn_rtlogGetString(sUrl, iMid, sDtDay, ci_fileType_json);
                                        }
                                        else
                                        {
                                            msepoch_t dtBegin = CxTime::currentDayEnd() - GM_MSEPOCH_ONE_DAY + 2;
                                            string sDtBegin = CxRapidJson::findMemberToString(vMeasure, cs_dtbegin);
                                            if (sDtBegin.size()>0)
                                            {
                                                dtBegin = CxString::toInt64(sDtBegin);
                                            }
                                            msepoch_t dtEnd = CxTime::currentDayEnd();
                                            string sDtEnd = CxRapidJson::findMemberToString(vMeasure, cs_dtend);
                                            if (sDtEnd.size()>0)
                                            {
                                                dtEnd = CxString::toInt64(sDtEnd);
                                            }
                                            sOut += fn_rtlogGetString(sUrl, iMid, dtBegin, dtEnd, ci_fileType_json);
                                        }
//                                        cxDebug() << "debug-20160318 mid" << iMid;
//                                        cxDebug() << "debug-20160318 sOut" << sOut;
                                        if (i < vParams->Size()-1)
                                            sOut.push_back(',');
                                    }
                                }
                            }
                            else
                            {
                                outJsonFinish(oFcgiRequest, "{\"error\":\"can not deal the structtype!\"}");
                                continue;
                            }

                            cxDebug() << sStructType << " ing " << CxTime::currentMsepochString();

                            IO << HTTPResponseHeader("HTTP/1.1", 200, "OK")
                                  .addHeader("Access-Control-Allow-Origin", "*")
                                  .addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization")
                                  .addHeader("Content-Type", "application/json");
                            IO << sOutBegin;
                            IO << sOut;
                            IO << sOutEnd << endl;

                            cxDebug() << sStructType << " end " << CxTime::currentMsepochString();
                        }
                        else
                        {
                            outHtml(IO, CGI, "error", "can not deal content-type" + sContentType);
                        }
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


int main(int argc, const char *argv[])
{
    CxApplication::init(argc, argv);

    //心跳服务初始化
    CommonService::init();

    FCGX_Init();
    f_requestDescriptorIndex = FCGX_OpenSocket(":9174", 0);
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

    fn_loadUrlMids();

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

    int rExec = CxApplication::exec();

    if (f_oFastcgiDealThread)
    {
        f_oFastcgiDealThread->stop();
        delete f_oFastcgiDealThread;
    }

    return rExec;
}
