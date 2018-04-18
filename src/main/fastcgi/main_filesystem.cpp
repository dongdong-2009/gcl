/*

 CREATE TABLE `T_SYS_SQLTEXT_QUERY` (
  `F_ID` int(11) NOT NULL,
  `F_URL` varchar(255) DEFAULT NULL,
  `F_SQLTEXT` varchar(4096) DEFAULT NULL,
  `F_TPYE` varchar(255) DEFAULT NULL,
  `F_NAME` varchar(255) DEFAULT NULL,
  `F_TITLE` varchar(255) DEFAULT NULL,
  `F_DESC` varchar(4096) DEFAULT NULL,
  PRIMARY KEY (`F_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


// ----------------------------------------------------------------------------------------------------------------
// 文件保存，分两个阶段进行。第一阶段：要保存的文件信息发送；第二阶段：要保存的文件内容，内容可分帧

// 文件保存的第一阶段：要保存的文件信息发送；
// http://10.31.0.15:8821/xxx.filesystem?fncode=req.fs_file_save_v101&filetype=json
// 散列请求：fs_file_save_v101
// pathType: [none, file, directory, link, io, disk, system(fifo|socket)]
// path: 系统中的路径表示
// fileName: 文件名
// fileSize: 文件大小
// fileSession: 下一次发过去的文件session
// creationTime: 创建时间
// lastWriteTime: 修改时间
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype":"fs_path_scan_v001",
  "data":[
    {
        "pathType": "file",
        "path": "/temp",
        "fileName": "a.txt"
        "fileSize": 1521,
        "fileSession: "sbid=0001;xxx=adfadsf",
        "creationTime": 1522124053962,
        "lastWriteTime": 1522124059163
    },
    {
        "pathType": "file",
        "path": "/temp",
        "fileName": "a.txt",
        "fileSize": 1521,
        "fileSession: "sbid=0001;xxx=adfadsf",
        "creationTime": 1522124053962,
        "lastWriteTime": 1522124059163
    }
  ]
}

// 返回，只为应答及校验
// pathType: [none, file, directory, link, io, disk, system(fifo|socket)]
// path: 系统中的路径表示
// fileName: 文件名
// fileSize: 文件大小
// fileSession: 下一次发过去的文件session
// creationTime: 创建时间
// lastWriteTime: 修改时间
// 可选属性"state":状态码，无或0时表示成功，其它值看具体数据字典
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype":"fs_path_scan_v001",
  "data":[
    {
        "pathType": "file",
        "path": "/temp",
        "fileName": "a.txt"
        "fileSize": 1521,
        "fileSession: "sbid=0001;xxx=adfadsf",
        "creationTime": 1522124053962,
        "lastWriteTime": 1522124059163,
        "state": 0
    },
    {
        "pathType": "file",
        "path": "/temp",
        "fileName": "a.txt",
        "fileSize": 1521,
        "fileSession: "sbid=0001;xxx=adfadsf",
        "creationTime": 1522124053962,
        "lastWriteTime": 1522124059163,
        "state": 0,
    }
  ]
}


// 文件保存的第二阶段：要保存的文件内容，内容可分帧
// http://10.31.0.15:8821/xxx.filesystem?fncode=req.fs_file_save_v102&filetype=data
// Request Headers
// Content-Type : data
// Content-Length :
// Content-Total :
// Content-Index :
// Content-Session :


// 返回 fs_file_save_v001
// Response Headers
// Content-Type :
// Content-Length :
// Content-Total :
// Content-Index :
// Content-Session :
// Content-State : 可选属性"state":状态码，无或0时表示成功，其它值看具体数据字典



// ----------------------------------------------------------------------------------------------------------------
// 文件加载
// urltype:  URL的类型有：EXE运行的发布目录，WEB的资源目录，绝对目录；【deploy|webroot|absolute】
// url 是全局统一资源名（可以通用在容器对象或实体对象中）
// http://10.31.0.15:8821/xxx.filesystem?fncode=req.fs_file_load_v101&filetype=json
// 散列请求：fs_file_load_v101
fncode = req.fs_file_load_v101
filetype = json
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype": "fs_file_load_v101",
  "params":
  [
    {
    "urltype": "deploy|webroot|absolute",
    "url": "/fp/zyj/fgj01/rfid"
    }
  ]
}

// 返回 fs_file_load_v001
// Response Headers
// Content-Type :
// Content-Length :
// Content-Total :
// Content-Index :
// Content-Session :
// Content-State : 可选属性"state":状态码，无或0时表示成功，其它值看具体数据字典



// ----------------------------------------------------------------------------------------------------------------
// 目录扫描： 目录列表只会目录内的一层，目录扫描是包括子目录
// urltype:  URL的类型有：EXE运行的发布目录，WEB的资源目录，绝对目录；【deploy|webroot|absolute】
// url 是全局统一资源名（可以通用在容器对象或实体对象中）
// http://10.31.0.15:8821/xxx.filesystem?fncode=req.fs_path_scan_v101&filetype=json
// 散列请求：fs_path_scan_v101
fncode = req.fs_path_scan_v101
filetype = json
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype": "fs_path_scan_v101",
  "params":
  [
    {
    "urltype": "deploy|webroot|absolute",
    "url": "/fp/zyj/fgj01/rfid"
    }
  ]
}

// 返回 fs_path_scan_v001
// pathType: [none, file, directory, link, io, disk, system(fifo|socket)]
// path: 系统中的路径表示
// fileName: 文件名
// fileSize: 文件大小
// creationTime: 创建时间
// lastWriteTime: 修改时间
// 可选属性"state":状态码，无或0时表示成功，其它值看具体数据字典
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype":"fs_path_scan_v001",
  "data":[
    {
        "pathType": "file",
        "path": "/temp",
        "fileName": "a.txt",
        "fileSize": 1521,
        "creationTime": 1522124053962,
        "lastWriteTime": 1522124059163,
        "state": 0
    },
    {
        "pathType": "file",
        "path": "/temp",
        "fileName": "a.txt",
        "fileSize": 1521,
        "creationTime": 1522124053962,
        "lastWriteTime": 1522124059163,
        "state": 0
    }
  ]
}



// ----------------------------------------------------------------------------------------------------------------
// 目录列表（一次只能查一个）
// urltype:  URL的类型有：EXE运行的发布目录，WEB的资源目录，绝对目录；【deploy|webroot|absolute】
// url 是全局统一资源名（可以通用在容器对象或实体对象中）
// http://10.31.0.15:8821/xxx.filesystem?fncode=req.fs_path_list_v101&filetype=json
// 散列请求：fs_path_list_v101
fncode = req.fs_path_list_v101
filetype = json
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype": "fs_path_list_v101",
  "params":
  [
    {
    "urltype": "deploy|webroot|absolute",
    "url": "/fp/zyj/fgj01/rfid"
    }
  ]
}

// 返回 fs_path_list_v001
// pathType: [none, file, directory, link, io, disk, system(fifo|socket)]
// path: 系统中的路径表示
// fileName: 文件名
// fileSize: 文件大小
// creationTime: 创建时间
// lastWriteTime: 修改时间
// 可选属性"state":状态码，无或0时表示成功，其它值看具体数据字典
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype":"fs_path_list_v001",
  "data":[
    {
        "pathType": "file",
        "path": "/temp",
        "fileName": "a.txt",
        "fileSize": 1521,
        "creationTime": 1522124053962,
        "lastWriteTime": 1522124059163,
        "state": 0
    }
  ]
}



// ----------------------------------------------------------------------------------------------------------------
// 路径信息
// urltype:  URL的类型有：EXE运行的发布目录，WEB的资源目录，绝对目录；【deploy|webroot|absolute】
// url 是全局统一资源名（可以通用在容器对象或实体对象中）
// http://10.31.0.15:8821/xxx.filesystem?fncode=req.fs_path_info_v101&filetype=json
// 散列请求：fs_path_info_v101
fncode = req.fs_path_info_v101
filetype = json
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype": "fs_path_info_v101",
  "params":
  [
    {
    "urltype": "deploy|webroot|absolute",
    "url": "/fp/zyj/fgj01/rfid"
    },
    {
    "pathtype": "deploy|webroot|absolute",
    "url": "/fp/zyj/fgj01/rfid"
    }
  ]
}

// 返回 fs_path_info_v001
// pathType: [none, file, directory, link, io, disk, system(fifo|socket)]
// path: 系统中的路径表示
// fileName: 文件名
// fileSize: 文件大小
// creationTime: 创建时间
// lastWriteTime: 修改时间
// 可选属性"state":状态码，无或0时表示成功，其它值看具体数据字典
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype":"fs_path_info_v001",
  "data":[
    {
        "pathType": "file",
        "path": "/temp",
        "fileName": "a.txt",
        "fileSize": 1521,
        "creationTime": 1522124053962,
        "lastWriteTime": 1522124059163,
        "state": 0
    },
    {
        "pathType": "file",
        "path": "/temp",
        "fileName": "a.txt",
        "fileSize": 1521,
        "creationTime": 1522124053962,
        "lastWriteTime": 1522124059163,
        "state": 0
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
//#include <iomanip>

#include <psm/general_access.h>
#include <common/gcl_resource.h>

#include <ccxx/cxdatabase_sqlite.h>
#include <ccxx/cxdatabase_odbc.h>

#include <vdi/common_service.h>

using namespace std;
using namespace cgicc;
using namespace rapidjson;

const char cs_url               [] = "url";
const char cs_urlType           [] = "urltype";
const char cs_urlType_deploy    [] = "deploy";
const char cs_urlType_webroot   [] = "webroot";
const char cs_urlType_absolute  [] = "absolute";
const char cs_webRoot           [] = "assets";

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

string fn_url2path(const string& sUrlType, const string& sUrl)
{
    if (sUrlType == cs_urlType_deploy)
    {
        return CxAppEnv::applicationDeployPathMerge(sUrl);
    }
    else if (sUrlType == cs_urlType_webroot)
    {
        return CxFileSystem::mergeFilePath(CxAppEnv::applicationDeployPathMerge(cs_webRoot), sUrl);
    }
    else if (sUrlType == cs_urlType_absolute)
    {
        return sUrl;
    }
    else
    {
        return "";
    }
}

string fn_resqFsPathInfo(const string& sUrlType, const string& sUrl)
{
//    "pathType": "%s","path": "%s","fileName": "%s","fileSize": %lld,"creationTime": %lld,"lastWriteTime": %lld,"state": %d
    static const char csPathInfo[] = "{\"pathType\": \"%s\",\"path\": \"%s\",\"fileName\": \"%s\",\"fileSize\": %lld,\"creationTime\": %lld,\"lastWriteTime\": %lld,\"state\": %d}";
    CxFilePathInfo pathInfo= CxFileSystem::getPathInfo(fn_url2path(sUrlType, sUrl));
    string r = CxString::format(csPathInfo,
                                CxFileSystem::getPathTypeString(pathInfo.pathType).c_str(),
                                CxFileSystem::convertPathStyle(CxFileSystem::normalize(pathInfo.path)).c_str(), pathInfo.fileName.c_str(),
                                pathInfo.fileSize, pathInfo.creationTime, pathInfo.lastWriteTime,
                                (int)0);
    return r;
}

vector<string> fn_resqFsPathScan(const string& sUrlType, const string& sUrl, bool bIncludeSubDir)
{
//    "pathType": "%s","path": "%s","fileName": "%s","fileSize": %lld,"creationTime": %lld,"lastWriteTime": %lld,"state": %d
    static const char csPathInfo[] = "{\"pathType\": \"%s\",\"path\": \"%s\",\"fileName\": \"%s\",\"fileSize\": %lld,\"creationTime\": %lld,\"lastWriteTime\": %lld,\"state\": %d}";
    std::vector<CxFilePathInfo> pathInfos;
    CxFileSystem::scanDir(fn_url2path(sUrlType, sUrl), pathInfos, bIncludeSubDir, true);
    vector<string> r;
    for (int j = 0; j < pathInfos.size(); ++j)
    {
        const CxFilePathInfo& pathInfo = pathInfos.at(j);
        string sPathInfo = CxString::format(csPathInfo,
                                            CxFileSystem::getPathTypeString(pathInfo.pathType).c_str(),
                                            CxFileSystem::convertPathStyle(CxFileSystem::normalize(pathInfo.path)).c_str(), pathInfo.fileName.c_str(),
                                            pathInfo.fileSize, pathInfo.creationTime, pathInfo.lastWriteTime,
                                            (int)0);
        r.push_back(sPathInfo);
    }
    return r;
}


class CgiFileDealThread : public CxJoinableThread
{
private:
    volatile int _status;
    CxMutex _lock;
    std::vector<FcgiRequest *> _bufferFcgiRequest1;
    std::vector<FcgiRequest *> _bufferFcgiRequest2;
    std::vector<FcgiRequest *> * volatile _pushBufferFcgiRequests;
    std::vector<FcgiRequest *> * volatile _popBufferFcgiRequests;

    volatile int _fcgiRequestCount;
    CxSingleWait _singleWait;

    static std::map<int,FcgiRequest*> idMapFcgiRequest;
    static std::map<int,std::string> idMapConfigUrl;

public:
    void init();

    void pushEvtDeal(FcgiRequest * oFcgiRequest);

private:
    static void outEvtFinish(FcgiRequest * oFcgiRequest, const string & sOut);


protected:
    void run();

    void exit();
};


void CgiFileDealThread::init()
{
    _status = 1;

    _pushBufferFcgiRequests = & _bufferFcgiRequest1;
    _popBufferFcgiRequests = & _bufferFcgiRequest2;
    _fcgiRequestCount = 0;
}

void CgiFileDealThread::pushEvtDeal(FcgiRequest * oFcgiRequest)
{
    {
        CxMutexScope lock(_lock);
        fn_holdFcgiRequest(oFcgiRequest);
        _pushBufferFcgiRequests->push_back(oFcgiRequest);
        _fcgiRequestCount++;
    }
    _singleWait.signal();
}


void CgiFileDealThread::outEvtFinish(FcgiRequest * oFcgiRequest, const string & sOut)
{
    FCgiIO & IO = * oFcgiRequest->cgiio;
    IO << HTTPResponseHeader("HTTP/1.1", 200, "OK")
          .addHeader("Access-Control-Allow-Origin", "*")
          .addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization")
          .addHeader("Content-Type", "application/json")
       << sOut << endl;

    fn_finishFcgiRequest(oFcgiRequest);
}

void CgiFileDealThread::run()
{
    CxDatabase * oDb = CxDatabaseManager::getDefaultDb();

    _status = 1;
    while ( _status )
    {
        _singleWait.wait();

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

            std::string sFileType;
            std::string sTableName;
            std::string sId;
            std::string sField;

            // 回应结果
            std::string sOut;
            {
                cgicc::const_form_iterator it = CGI.getElement("filetype");
                if (it != CGI.getElements().end())
                {
                    sFileType = CxString::toLower(it->getValue());
                }

                it = CGI.getElement("table");
                if (it != CGI.getElements().end())
                {
                    sTableName = CxString::toLower(it->getValue());
                }

                it = CGI.getElement("id");
                if (it != CGI.getElements().end())
                {
                    sId = CxString::toLower(it->getValue());
                }

                it = CGI.getElement("field");
                if (it != CGI.getElements().end())
                {
                    sField = it->getValue();
                }

            }

            sOut = "{\"data\":";

            if (sFileType == "json")
            {
                std::string sPathField = sField + "_PATH";
                std::string sql = "select " + sField + "," + sPathField + " from " + sTableName + " where F_ID = '" + sId + "'";

                if (oDb) {
                    std::map<std::string,std::string> valueMap = oDb->queryValueToMap(sql);

                    std::string sFilePath = valueMap[sPathField];
                    std::string sFieldValue = valueMap[sField];

                    if (! CxFile::save(sFilePath,sFieldValue))
                    {
                        sOut += "-2";
                    }
                    else
                    {
                        sOut += "1";
                    }
                }
                else
                {
                    cxPrompt() << "load db.T_SYS_SQLTEXT_QUERY fail!!! for CxDatabaseManager::getDefaultDb() is NULL! please config the db setting!!";
                    sOut += "-1";
                }

                sOut += "}";

                outEvtFinish(oFcgiRequest, sOut);

            }
            else if(sFileType == "file")
            {
                std::string sFilePath ;
                cgicc::const_form_iterator it = CGI.getElement("FilePath");
                if (it != CGI.getElements().end())
                {
                    sFilePath = it->getValue();
                }
                string sPostData = CGI.getEnvironment().getPostData();
                Document d;
                d.Parse(sPostData.data());
                if (d.HasParseError())
                {
//                    printf( "{\"error\":\"can not parse the json string!\"}\n");
//                    continue;
                    sOut += "-2";
                }
                else
                {
                    string sFieldValue = CxJson::findMemberToString(d, "data");
//                    printf("\n%s\n",sFieldValue.c_str());
                    if (! CxFile::save(sFilePath,sFieldValue))
                    {
                        sOut += "-2";
                    }
                    else
                    {
                        sOut += "1";
                    }
                }
                sOut += "}";

                outEvtFinish(oFcgiRequest, sOut);
            }
        }

        if (_popBufferFcgiRequests->size()>0)
        {
            _popBufferFcgiRequests->clear();
        }
    }
}

void CgiFileDealThread::exit() {
    _status = 0;
    CxJoinableThread::exit();
}

CgiFileDealThread * volatile f_oCgiFileDealThread = NULL;



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
                    //*url的参数command中带有savefile的
                    if (sFnCode.find(".savefile.") != string::npos)
                    {
                        f_oCgiFileDealThread->pushEvtDeal(oFcgiRequest);
                    }
                    else
                    {
                        string sContentType = CGI.getEnvironment().getContentType();
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

                            string sSession = CxJson::findMemberToString(d, "session");
                            string sStructType = CxJson::findMemberToString(d, "structtype");
                            string sOutBegin = CxString::format("{\"session\":\"%s\",\"structtype\":\"%s\",\"data\":[", sSession.c_str());
                            string sOut;
                            string sOutEnd = "]}";
                            //**structtype 为 rtdata_v101
                            cxLogDebug() << sStructType << " begin " << CxTime::currentMsepochString();

                            if (sStructType.find("fs_path_info_v101") != string::npos ||
                                sStructType.find("fs_path_list_v101") != string::npos ||
                                sStructType.find("fs_path_scan_v101") != string::npos
                                )
                            {
                                    // **structtype 为 fs_path_info_v101
                                if (sStructType.find("fs_path_info_v101") != string::npos)
                                {
                                    sOutBegin = CxString::format(sOutBegin.c_str(), "fs_path_info_v001");
                                    rapidjson::Value * vParams = CxJson::findMember(d, "params");
                                    if (vParams && vParams->IsArray())
                                    {
                                        for (SizeType i = 0; i < vParams->Size(); ++i)
                                        {
                                            Value & vMeasure = (*vParams)[i];
                                            int iMid = 0;
                                            string sUrlType = CxJson::findMemberToString(vMeasure, cs_urlType);
                                            string sUrl = CxJson::findMemberToString(vMeasure, cs_url);
                                            sOut += fn_resqFsPathInfo(sUrlType, sUrl);
                                            if (i < vParams->Size()-1)
                                                sOut.push_back(',');
                                        }
                                    }
                                }
                                    // **structtype 为 fs_path_list_v001
                                else if (sStructType.find("fs_path_list_v101") != string::npos)
                                {
                                    sOutBegin = CxString::format(sOutBegin.c_str(), "fs_path_list_v001");
                                    rapidjson::Value * vParams = CxJson::findMember(d, "params");
                                    if (vParams && vParams->IsArray())
                                    {
                                        for (SizeType i = 0; i < vParams->Size(); ++i)
                                        {
                                            Value & vMeasure = (*vParams)[i];
                                            int iMid = 0;
                                            string sUrlType = CxJson::findMemberToString(vMeasure, cs_urlType);
                                            string sUrl = CxJson::findMemberToString(vMeasure, cs_url);
                                            vector<string> sPathInfos = fn_resqFsPathScan(sUrlType, sUrl, false);
                                            sOut = CxString::join(sPathInfos, ',');
                                            continue;
                                        }
                                    }
                                }
                                    // **structtype 为 fs_path_scan_v101
                                else
                                {
                                    sOutBegin = CxString::format(sOutBegin.c_str(), "fs_path_scan_v001");
                                    rapidjson::Value * vParams = CxJson::findMember(d, "params");
                                    if (vParams && vParams->IsArray())
                                    {
                                        for (SizeType i = 0; i < vParams->Size(); ++i)
                                        {
                                            Value & vMeasure = (*vParams)[i];
                                            int iMid = 0;
                                            string sUrlType = CxJson::findMemberToString(vMeasure, cs_urlType);
                                            string sUrl = CxJson::findMemberToString(vMeasure, cs_url);
                                            vector<string> sPathInfos = fn_resqFsPathScan(sUrlType, sUrl, true);
                                            sOut = CxString::join(sPathInfos, ',');
                                            continue;
                                        }
                                    }
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
                            else if (sStructType.find("fs_file_load_v101") != string::npos)
                            {
                                // fs_file_load_v101
                                rapidjson::Value * vParams = CxJson::findMember(d, "params");
                                if (vParams && vParams->IsArray())
                                {
                                    for (SizeType i = 0; i < vParams->Size(); ++i)
                                    {
                                        Value & vMeasure = (*vParams)[i];
                                        int iMid = 0;
                                        string sUrlType = CxJson::findMemberToString(vMeasure, cs_urlType);
                                        string sUrl = CxJson::findMemberToString(vMeasure, cs_url);
                                        string sFilePath = fn_url2path(sUrlType, sUrl);
                                        sOut = CxFile::load(sFilePath);
                                        break;
                                    }
                                }
                                cxDebug() << sStructType << " ing " << CxTime::currentMsepochString();

                                IO << HTTPResponseHeader("HTTP/1.1", 200, "OK")
                                    .addHeader("Access-Control-Allow-Origin", "*")
                                    .addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization")
                                    .addHeader("Content-Type", "data")
                                    .addHeader("Content-Total", CxString::toString(sOut.size()))
                                    .addHeader("Content-Index", "0")
                                    .addHeader("Content-Length", CxString::toString(sOut.size()))
                                    .addHeader("Content-Session", "")
                                    .addHeader("Content-State", "");
                                IO << sOut;
                                IO << endl;

                                cxDebug() << sStructType << " end " << CxTime::currentMsepochString();
                            }
                            else
                            {
                                outJsonFinish(oFcgiRequest, "{\"error\":\"can not deal the structtype!\"}");
                                continue;
                            }
                        }
                        else
                        {
                            cxLog() << "!!!warning. do not know sContentType: " << sContentType;
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


void fn_timeout_dbHeartJump(int)
{
    //reconnect
    CxDatabase *oDb = CxDatabaseManager::getDefaultDb();
    if (oDb)
    {
        string sConnectSource = oDb->connectSource();
        string sConnectType = oDb->connectType();
        bool ret = CxDatabaseManager::closeDatabase(sConnectSource);
        if (ret)
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
}


int main(int argc, const char *argv[])
{
    CxApplication::init(argc, argv);

    //心跳服务初始化
    CommonService::init();

    FCGX_Init();
    f_requestDescriptorIndex = FCGX_OpenSocket(":9171", 0);
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


    if (! f_oCgiFileDealThread)
    {
        f_oCgiFileDealThread = new CgiFileDealThread();
        f_oCgiFileDealThread->init();
        f_oCgiFileDealThread->start();
        if (f_iDbHeartJumpInterval>0 && f_sDbHeartJumpSql.size()>0)
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

    if (f_oCgiFileDealThread)
    {
        f_oCgiFileDealThread->stop();
        delete f_oCgiFileDealThread;
    }

    return rExec;
}
