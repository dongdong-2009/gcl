/*
 * 实时数据请求，支持两种方式：cics.xml,ics.json；支持散列请求、支持数组请求；
 * cics.xml 支持散列请求：9991；数组请求是：9992；返回时都统一用：9999
<?xml version="1.0" encoding="utf-8"?>
<YGCT>
    <HEAD>
        <VERSION>1.0</VERSION>
        <SRC>1200000003</SRC>
        <DES>1200000003</DES>
        <MsgNo>9991</MsgNo>
        <MsgId>91d9e512-3695-4796-b063-306544be6f1f</MsgId>
        <MsgRef/>
        <TransDate>20151215094317</TransDate>
        <Reserve/>
    </HEAD>
    <MSG>
        <RealData9991>
            <ADDRESSES>16777300,16777218,16777219</ADDRESSES>
        </RealData9991>
        <RealData9991>
            <ADDRESS>16777217</ADDRESS>
        </RealData9991>
        <RealData9991>
            <ADDRESS>16777218</ADDRESS>
        </RealData9991>
    </MSG>
</YGCT>


 * cics.xml 支持散列请求：9991；数组请求是：9992
<?xml version="1.0" encoding="utf-8"?>
<YGCT>
    <HEAD>
        <VERSION>1.0</VERSION>
        <SRC>1200000003</SRC>
        <DES>1200000003</DES>
        <MsgNo>9992</MsgNo>
        <MsgId>91d9e512-3695-4796-b063-306544be6f1f</MsgId>
        <MsgRef/>
        <TransDate>20151215094317</TransDate>
        <Reserve/>
    </HEAD>
    <MSG>
        <RealData9992>
            <ADDRESS>16777217</ADDRESS>
            <COUNT>20</COUNT>
        </RealData9992>
        <RealData9992>
            <ADDRESS>33554432</ADDRESS>
            <COUNT>20</COUNT>
        </RealData9992>
    </MSG>
</YGCT>

 * cics.xml 返回
<?xml version="1.0" encoding="utf-8"?>
<YGCT>
    <HEAD>
        <VERSION>1.0</VERSION>
        <SRC>1200000003</SRC>
        <DES>1200000003</DES>
        <MsgNo>9999</MsgNo>
        <MsgId>91d9e512-3695-4796-b063-306544be6f1f</MsgId>
        <MsgRef/>
        <TransDate>20151215094317</TransDate>
        <Reserve/>
    </HEAD>
    <MSG>
        <RealData9999>
            <URL>YIYANG.DCS01.RLPDC02.LJL</URL>
            <ADDRESS>16777217</ADDRESS>
            <DATATIME>2015-12-1509:44:11</DATATIME>
            <VALUE>11.45</VALUE>
            <Q>1</Q>
        </RealData9999>
        <RealData9999>
            <URL>YIYANG.DCS01.RLPDC03</URL>
            <ADDRESS>16777218</ADDRESS>
            <DATATIME>2015-12-1509:44:11</DATATIME>
            <VALUE>1.24</VALUE>
            <Q>1</Q>
        </RealData9999>
    </MSG>
</YGCT>

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



// ICS实时数据请求的 json格式：支持散列请求：rtdata_v101；数组请求是：rtdata_v102；返回时都统一用：rtdata_v001
// url 是全局统一资源名（可以通用在容器对象或实体对象中）
// mid 是实时库的实时点全局唯一id
// url和mid可以只有一个，两个同时都有时以mid为准
// ics.json 散列请求
http://10.31.0.15:8821/ics.cgi?fncode=req.rtdata_v101&filetype=json

fncode = req.rtdata_v101
filetype = json

{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype": "rtdata_v101",
  "params":
  [
    {
    "url": "/fp/zyj/fgj01/rfid",
    "mid": 33556644
    },
    {
    "url": "/fp/zyj/fgj01/ypmm",
    "mid": 33556645
    }
  ]
}



// ics.json 数组请求
// 数组请求中是以url为索引时，如果url可以对应到mid就以mid为开始索引；如果url是容器时就返回容器对应数量内个数
fncode = req.rtdata_v102
filetype = json

{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype": "rtdata_v102",
  "params":
  [
    {
    "url": "/fp/zyj/fgj01/rfid",
    "mid": 33556644,
    "count": 100
    },
    {
    "url": "/fp/zyj/fgj01/ypmm",
    "mid": 33556645,
    "count": 100
    }
  ]
}


// ics.json返回时都统一用：rtdata_v001
// "v": 数值
// "q": 值的质量
// "t": 值的时间,unix时间戳（1970到目前的毫秒数，服务器的当地时间）
// 可选属性"srcid": 实时数据信息来源的源ID,
// 可选属性"srcurl": 实时数据信息来源的源url,
// 可选属性"state":状态码，无或0时表示成功，其它值看具体数据字典
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype":"rtdata_v001",
  "data":[
    {
    "url":"/fp/zyj/fgj01/rfid",
    "mid":33556644,
    "v":"ABC12345678D",
    "q":1,
    "t":1892321321,
    "srcid":1231231,
    "srcurl":"/fp/zyj/fgjapp",
    "state":0
    },
    {
    "url":"/fp/zyj/fgj01/ypmm",
    "mid":33556645,
    "v":"20160100001",
    "q":1,
    "t":1892321521
    "srcid":1231231,
    "srcurl":"/fp/zyj/fgjapp",
    "state":0
    }
  ]
}


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









// ICS 遥控请求的 json格式：yk_v101；返回时用：yk_r_v001
// url 是全局统一资源名（可以通用在容器对象或实体对象中）
// mid 是实时库的实时点全局唯一id
// v: 遥控的目标数值（如果是遥信量：1 分、2 合、0 无）（如果是遥测量、文字量就是直接数值量）
// index 遥控序号，可选预留项
// strategy 遥控策略，可选预留项
// method 遥控方法，可选预留项
// ext 遥控操作附加数据对象（kv对）（pos : 仓位号、柜号）
// url和mid可以只有一个，两个同时都有时以mid为准
// ics.json 散列请求
http://10.31.0.15:8821/ics.cgi?fncode=req.yk_v101&filetype=json

fncode = req.yk_v101
filetype = json

{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype": "yk_v101",
  "params":
  [
    {
      "url": "/fp/qyg/qyg01/yk01",
      "mid": 33556644,
      "v": 1,
      "strategy": 1,
      "method": 0,
      "ext": {
        "pos": 11
      }
    },
    {
      "url": "/fp/qyg/qyg01/yk02",
      "mid": 33556645,
      "v": 2,
      "strategy": 1,
      "method": 0,
      "ext": {
        "pos": 12
    }
  ]
}



// ICS 遥控返回用：yk_r_v001
// 可选属性"state":状态码，无或0时表示接收并转发成功（如果有转发），其它值看具体数据字典
// 可选属性"t": 值的时间,unix时间戳（1970到目前的毫秒数，服务器的当地时间）
{
  "session":"sbid=0001;xxx=adfadsf",
  "structtype":"yk_r_v001",
  "data":[
    {
      "url": "/fp/qyg/qyg01/yk01",
      "mid": 33556644,
      "state": 0,
      "t": 1892321521
    },
    {
      "url": "/fp/qyg/qyg01/yk02",
      "mid": 33556645,
      "state": 0,
      "t": 1892321521
    }
  ]
}








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

#include <ccxx/ccxx.h>

//#include <iomanip>

#include <vdi/measure_sharememory_all.h>

#include <fcgi_config.h>
#include <fcgiapp.h>
#include <fcgios.h>

#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTTPResponseHeader.h>
#include <cgicc/HTMLClasses.h>
#include <contrib/FCgiIO.h>

#include <ccxx/cxrapidjson.h>

#include <common/gcl_resource.h>

#include <psm/general_access.h>

#include <ccxx/cxdatabase_sqlite.h>
#include <ccxx/cxdatabase_odbc.h>
#include <ccxx/cxstring.h>

#include <main/gcl_rtdbs/rtdb_logfile.h>

using namespace std;
using namespace cgicc;
using namespace rapidjson;

const int ci_fileType_xml = 0;
const int ci_fileType_json = 1;

const char * f_sMeasureHeadXml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
        "\r\n<YGCT>"
        "\r\n	<HEAD>"
        "\r\n		<VERSION>1.0</VERSION>"
        "\r\n		<SRC>1200000003</SRC>"
        "\r\n		<DES>1200000003</DES>"
        "\r\n		<MsgNo>9999</MsgNo>"
        "\r\n		<MsgId>91d9e512-3695-4796-b063-306544be6f1f</MsgId>"
        "\r\n		<MsgRef/>"
        "\r\n		<TransDate>20151215094317</TransDate>"
        "\r\n		<Reserve/>"
        "\r\n	</HEAD>"
        "\r\n	<MSG>"
        ;

const char * f_sMeasureBodyXml =
        "\r\n		<RealData9999>"
        "\r\n			<URL>ics/none</URL>"
        "\r\n			<ADDRESS>%d</ADDRESS>"
        "\r\n			<DATATIME>%s</DATATIME>"
        "\r\n			<VALUE>%s</VALUE>"
        "\r\n			<Q>%d</Q>"
        "\r\n		</RealData9999>"
        ;

const char * f_sMeasureEndXml =
        "\r\n	</MSG>"
        "\r\n</YGCT>"
        ;


//{"code":"9999","measures":[{"url":"asm570a/001","mid":16777217,"v":1,"q":1,"t":1812345678},{"url":"asm570a/002","mid":16777218,"v":2,"q":1,"t":1812345679}]}
const char * f_sMeasureBodyJson =
        "{\"url\":\"%s\",\"mid\":%d,\"v\":\"%s\",\"q\":%d,\"t\":%lld,\"srcid\":%d,\"state\":%d}"
        ;

const char * f_sYkBodyJson =
        "{\"url\":\"%s\",\"mid\":%d,\"t\":%lld,\"state\":%d}"
        ;

const char * cs_command_sql_select = "SELECT * FROM ICS.T_ICS_SYS_SQLTEXT_QUERY;";
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


std::string fn_getMeasureValue(const int & value)
{
    return CxString::toString(value);
}

std::string fn_getMeasureValue(const double & value)
{
    char str[64];
//    memset(str, 0, sizeof(str));
    sprintf(str, "%1.3f", value);
    return string(str);
    //    stringstream ss;
//    ss << std::setprecision(4) << value;
//    return ss.str();
}

std::string fn_getMeasureValue(const StrawValue & value)
{
    //只返回有效字符
    return std::string(value.value);
//    return std::string(value.value, sizeof(value.value));
}


//*XML XML XML
template<typename T>
string fn_measureDataToXmlString(typename MeasureShareMemory<T>::MemoryManagerBase * const oMemoryManager, int iMid)
{
    string r;

    const typename MeasureShareMemory<T>::MeasureData * oMeasure = oMemoryManager->measureData(iMid);
    if (! oMeasure) return r;
    string sValue = fn_getMeasureValue(oMeasure->value);
    string sDatetime = CxTime::toString(oMeasure->changedTime);
    r = CxString::format(f_sMeasureBodyXml, oMeasure->measureId, sDatetime.c_str(), sValue.c_str(), oMeasure->quality);
    return r;
}

template<typename T>
string fn_measureChangedToXmlString(const typename MeasureShareMemory<T>::ChangedData * const oMeasureChanged)
{
    string r;
    if (! oMeasureChanged) return r;
    const typename MeasureShareMemory<T>::ChangedData * oChangedData = oMeasureChanged;
    string sValue = fn_getMeasureValue(oChangedData->value);
    string sDatetime = CxTime::toString(oChangedData->changedTime);
    r = CxString::format(f_sMeasureBodyXml, oChangedData->measureId, sDatetime.c_str(), sValue.c_str(), oChangedData->quality);
    return r;
}

template<typename T>
string fn_measureChangedsToXmlString(const typename MeasureShareMemory<T>::ChangedData * const oChangedBegin, const typename MeasureShareMemory<T>::ChangedData * const oChangedEnd, const std::string & sSqlInsert)
{
    string r;
    const typename MeasureShareMemory<T>::ChangedData * oChangedData = oChangedBegin;
    while (oChangedData < oChangedEnd)
    {
        r += fn_measureChangedToXmlString(oChangedData);
        oChangedData++;
    }
    return r;
}


//*JSON JSON JSON
template<typename T>
string fn_measureDataToJsonString(typename MeasureShareMemory<T>::MemoryManagerBase * oMemoryManager, int iMid)
{
    string r;

    const typename MeasureShareMemory<T>::MeasureData * oMeasure = oMemoryManager->measureData(iMid);
    if (oMeasure)
    {
        string sValue = fn_getMeasureValue(oMeasure->value);
        string sUrl;
        std::map<int, string>::const_iterator it = f_midUrls.find(iMid);
        if ( it != f_midUrls.end() ) sUrl = it->second;
//        "{\"url\":\"%s\",\"mid\":%d,\"v\":\"%s\",\"q\":%d,\"t\":%ld,\"srcid\":%d,\"state\":%d}"
        r = CxString::format(f_sMeasureBodyJson, sUrl.c_str(), oMeasure->measureId, sValue.c_str(), oMeasure->quality, oMeasure->changedTime, oMeasure->changedSourceId, 1);
    }
    else
    {
        r = CxString::format(f_sMeasureBodyJson, "", 0, "null", 0, 0, -1, -1);
    }
    return r;
}

template<typename T>
string fn_measureChangedToJsonString(const typename MeasureShareMemory<T>::ChangedData * const oMeasureChanged)
{
    string r;
    const typename MeasureShareMemory<T>::ChangedData * oChangedData = oMeasureChanged;
    string sValue = fn_getMeasureValue(oChangedData->value);
    string sUrl;
    std::map<int, string>::const_iterator it = f_midUrls.find(oChangedData->measureId);
    if ( it != f_midUrls.end() ) sUrl = it->second;
    r = CxString::format(f_sMeasureBodyJson, sUrl.c_str(), oChangedData->measureId, sValue.c_str(), oChangedData->quality, oChangedData->changedTime, oChangedData->changedSourceId, 1);
    return r;
}

template<typename T>
string fn_measureChangedsToJsonString(const typename MeasureShareMemory<T>::ChangedData * const oChangedBegin, const typename MeasureShareMemory<T>::ChangedData * const oChangedEnd, const std::string & sSqlInsert)
{
    string r;
    const typename MeasureShareMemory<T>::ChangedData * oChangedData = oChangedBegin;
    while (oChangedData < oChangedEnd)
    {
        r += fn_measureChangedToJsonString(oChangedData);
        oChangedData++;
    }
    return r;
}



// # 获取实时数据返回（以数组形式），可以XML或JSON的方式
// # iFileType =0：表示XML方式； =1：表示JSON方式
string fn_measureGetString(int iMid, int iFileType=0)
{
    string r;

    //*yx
    //遥信处理 vqt整形方式
    if (iMid>=0x01000000 && iMid<0x02000000)
    {
        if (iFileType==ci_fileType_xml)
        {
            r = fn_measureDataToXmlString<int>(MonsbShareMemory::memoryManager(), iMid);
        }
        else
        {
            r = fn_measureDataToJsonString<int>(MonsbShareMemory::memoryManager(), iMid);
        }
    }
        //*yc
        //遥测处理 vqt整形方式
    else if (iMid>=0x02000000 && iMid<0x03000000)
    {
        if (iFileType==ci_fileType_xml)
            r = fn_measureDataToXmlString<double>(YcaddShareMemory::memoryManager(), iMid);
        else
            r = fn_measureDataToJsonString<double>(YcaddShareMemory::memoryManager(), iMid);
    }
        //*yw
        //遥文处理 vqt整形方式
    else if (iMid>=0x03000000 && iMid<0x04000000)
    {
        if (iFileType==ci_fileType_xml)
            r = fn_measureDataToXmlString<StrawValue>(StrawShareMemory::memoryManager(), iMid);
        else
            r = fn_measureDataToJsonString<StrawValue>(StrawShareMemory::memoryManager(), iMid);
    }

    return r;
}

//*获取实时数据返回（以数组形式），可以XML或JSON的方式
//iFileType =0：表示XML方式； =1：表示JSON方式
string fn_measureGetArrayString(int iBeginMid, int iMidCount, int iFileType=ci_fileType_xml)
{
    string r;

    //*yx
    //遥信处理 vqt整形方式
    if (iBeginMid>=0x01000000 && iBeginMid<0x02000000)
    {
        MonsbMemoryManager * oMemoryManager = MonsbShareMemory::memoryManager();

        GM_INVALID_RETURE_(iMidCount>0 && iMidCount<oMemoryManager->measureCount(), r);
        GM_INVALID_RETURE_(iBeginMid >= oMemoryManager->getMinId(), r);
        int iEndMid = iBeginMid + iMidCount;
        GM_INVALID_RETURE_(iEndMid <= oMemoryManager->getMaxId(), r);

        if (iFileType==ci_fileType_xml)
        {
            for (int i = iBeginMid; i < iEndMid; ++i)
            {
                r += fn_measureDataToXmlString<int>(oMemoryManager, i);
            }
        }
        else
        {
            for (int i = iBeginMid; i < iEndMid; ++i)
            {
                r += fn_measureDataToJsonString<int>(oMemoryManager, i);
                if (i < iEndMid-1)
                    r.push_back(',');
            }
        }
    }
    //*yc
    //遥测处理 vqt整形方式
    else if (iBeginMid>=0x02000000 && iBeginMid<0x03000000)
    {
        YcaddMemoryManager * oMemoryManager = YcaddShareMemory::memoryManager();

        GM_INVALID_RETURE_(iMidCount>0 && iMidCount<oMemoryManager->measureCount(), r);
        GM_INVALID_RETURE_(iBeginMid >= oMemoryManager->getMinId(), r);
        int iEndMid = iBeginMid + iMidCount;
        GM_INVALID_RETURE_(iEndMid <= oMemoryManager->getMaxId(), r);

        if (iFileType==ci_fileType_xml)
        {
            for (int i = iBeginMid; i < iEndMid; ++i)
            {
                r += fn_measureDataToXmlString<double>(oMemoryManager, i);
            }
        }
        else
        {
            for (int i = iBeginMid; i < iEndMid; ++i)
            {
                r += fn_measureDataToJsonString<double>(oMemoryManager, i);
                if (i < iEndMid-1)
                    r.push_back(',');
            }
        }
    }
    //*yw
    //遥文处理 vqt整形方式
    else if (iBeginMid>=0x03000000 && iBeginMid<0x04000000)
    {
        StrawMemoryManager * oMemoryManager = StrawShareMemory::memoryManager();

        GM_INVALID_RETURE_(iMidCount>0 && iMidCount<oMemoryManager->measureCount(), r);
        GM_INVALID_RETURE_(iBeginMid >= oMemoryManager->getMinId(), r);
        int iEndMid = iBeginMid + iMidCount;
        GM_INVALID_RETURE_(iEndMid <= oMemoryManager->getMaxId(), r);

        if (iFileType==ci_fileType_xml)
        {
            for (int i = iBeginMid; i < iEndMid; ++i)
            {
                r += fn_measureDataToXmlString<StrawValue>(oMemoryManager, i);
            }
        }
        else
        {
            for (int i = iBeginMid; i < iEndMid; ++i)
            {
                r += fn_measureDataToJsonString<StrawValue>(oMemoryManager, i);
                if (i < iEndMid-1)
                    r.push_back(',');
            }
        }
    }

    return r;
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





//************************************************************************************************************
//------------------------------------------------------------------------------------------------------------
//************************************************************************************************************





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








//************************************************************************************************************
//------------------------------------------------------------------------------------------------------------
//************************************************************************************************************


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
            if (1>0)
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

            if (sFileType == "json") {
                std::string sPathField = sField + "_PATH";
                std::string sql = "select " + sField + "," + sPathField + " from " + sTableName + " where F_ID = '" + sId + "'";

                if (oDb) {
                    std::map<std::string,std::string> valueMap = oDb->queryValueToMap(sql);

                    std::string sFilePath = valueMap[sPathField];
                    std::string sFieldValue = valueMap[sField];

                    if (CxFile::save(sFilePath,sFieldValue) == false) {
                        sOut += "-2";
                    }
                    else {
                        sOut += "1";
                    }
                }
                else {
                    cxPrompt() << "load db.T_SYS_SQLTEXT_QUERY fail!!! for CxDatabaseManager::getDefaultDb() is NULL! please config the db setting!!";
                    sOut += "-1";
                }

                sOut += "}";

                outEvtFinish(oFcgiRequest, sOut);

            }else if(sFileType == "file"){
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
                }else{
                    string sFieldValue = CxRapidJson::findMemberToString(d, "data");
//                    printf("\n%s\n",sFieldValue.c_str());
                    if (CxFile::save(sFilePath,sFieldValue) == false) {
                        sOut += "-2";
                    } else {
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


//************************************************************************************************************
//------------------------------------------------------------------------------------------------------------
//************************************************************************************************************




//class CgiLocalDealThread : public CxJoinableThread
//{
//private:
//    volatile int _status;
//    CxMutex _lock;
//    std::vector<FcgiRequest *> _bufferFcgiRequest1;
//    std::vector<FcgiRequest *> _bufferFcgiRequest2;
//    std::vector<FcgiRequest *> * volatile _pushBufferFcgiRequests;
//    std::vector<FcgiRequest *> * volatile _popBufferFcgiRequests;

//    volatile int _fcgiRequestCount;
//    CxSingleWait _singleWait;

//    static std::map<int,FcgiRequest*> idMapFcgiRequest;
//    static std::map<int,std::string> idMapConfigUrl;

//public:
//    void init();

//    void pushEvtDeal(FcgiRequest * oFcgiRequest);


//private:
//    static int GeneralAccessReturn(const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach);

//    static int pushRequestInMap(FcgiRequest * oFcgiRequest);

//    static void outEvtFinish(FcgiRequest * oFcgiRequest, const string & sOut);


//protected:
//    void run();

//    void exit();
//};

//std::map<int,FcgiRequest*> CgiLocalDealThread::idMapFcgiRequest;
//std::map<int,std::string> CgiLocalDealThread::idMapConfigUrl;

//void CgiLocalDealThread::init()
//{
//    _status = 1;

//    _pushBufferFcgiRequests = & _bufferFcgiRequest1;
//    _popBufferFcgiRequests = & _bufferFcgiRequest2;
//    _fcgiRequestCount = 0;
//}

//void CgiLocalDealThread::pushEvtDeal(FcgiRequest * oFcgiRequest)
//{
//    {
//        CxMutexScope lock(_lock);
//        fn_holdFcgiRequest(oFcgiRequest);
//        _pushBufferFcgiRequests->push_back(oFcgiRequest);
//        _fcgiRequestCount++;
//    }
//    _singleWait.signal();
//}

//int CgiLocalDealThread::GeneralAccessReturn(const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach)
//{
//    std::string sRequestId;
//    std::string sResult;
//    int nRequestId;
//    int nResult;

//    std::string sOut;

//    if (sCommand.find(".convert.2file") != string::npos) {
//        std::vector<std::string> params;
//        std::vector<std::string> params1;
//        std::vector<std::string> params2;

//        params = CxString::split(sParam,";");
//        params1 = CxString::split(params[0],"=");
//        params2 = CxString::split(params[1],"=");

//        sRequestId = params1[1];
//        sResult = params2[1];

//        nRequestId = CxString::toInt32(sRequestId);
//        nResult = CxString::toInt32(sResult);

//        cxPrompt() << "nRequestId:" << nRequestId;
//        cxPrompt() << "nResult:" << nResult;

//        FcgiRequest * oFcgiRequest = idMapFcgiRequest[nRequestId];
//        std::string configUrl = idMapConfigUrl[nRequestId];

//        sOut = "\{\"data\":\"" + sResult + "\"\}";
//        outEvtFinish(oFcgiRequest, sOut);

//        idMapFcgiRequest.erase(nRequestId);
//        idMapConfigUrl.erase(nRequestId);
//    }


//    return 1;
//}

//int CgiLocalDealThread::pushRequestInMap(FcgiRequest *oFcgiRequest)
//{
//    std::map<int,FcgiRequest*>::iterator iter;
//    for( int i = 0; i <= idMapFcgiRequest.size(); i++ ){
//        iter = idMapFcgiRequest.find(i);
//        if( iter == idMapFcgiRequest.end() ){
//            idMapFcgiRequest[i] = oFcgiRequest;
//            return i;
//        }
//    }

//    return -1;
//}

//void CgiLocalDealThread::outEvtFinish(FcgiRequest * oFcgiRequest, const string & sOut)
//{
//    FCgiIO & IO = * oFcgiRequest->cgiio;
//    IO << HTTPResponseHeader("HTTP/1.1", 200, "OK")
//          .addHeader("Access-Control-Allow-Origin", "*")
//          .addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization")
//          .addHeader("Content-Type", "application/json")
//       << sOut << endl;

//    fn_finishFcgiRequest(oFcgiRequest);
//}

//void CgiLocalDealThread::run()
//{
//    _status = 1;
//    while ( _status )
//    {
//        _singleWait.wait();

//        //*deal fcgi
//        if (_fcgiRequestCount>0)
//        {
//            CxMutexScope lock(_lock);
//            std::vector<FcgiRequest *> * tmpRequests = _pushBufferFcgiRequests;
//            _pushBufferFcgiRequests = _popBufferFcgiRequests;
//            _popBufferFcgiRequests = tmpRequests;
//            _fcgiRequestCount = 0;
//        }

//        if (_popBufferFcgiRequests->size()>100)
//        {
//            for (size_t i = 0; i < _popBufferFcgiRequests->size(); ++i)
//            {
//                FcgiRequest * oFcgiRequest = _popBufferFcgiRequests->at(i);
//                fn_finishFcgiRequest(oFcgiRequest);
//            }
//            _popBufferFcgiRequests->clear();
//            cxPrompt() << "Error ! Error ! Error ! CgiSqlDealThread FcgiRequest->size()>100 , FcgiRequest->clear()!!!";
//            cxPrompt() << "Error ! Error ! Error ! CgiSqlDealThread FcgiRequest->size()>100 , FcgiRequest->clear()!!!";
//            cxPrompt() << "Error ! Error ! Error ! CgiSqlDealThread FcgiRequest->size()>100 , FcgiRequest->clear()!!!";
//        }

//        for (size_t i = 0; i < _popBufferFcgiRequests->size(); ++i)
//        {
//            FcgiRequest * oFcgiRequest = _popBufferFcgiRequests->at(i);
//            Cgicc & CGI = * oFcgiRequest->cgicc;

//            string sConfigUrl;
//            if (1>0)
//            {
//                cgicc::const_form_iterator it = CGI.getElement("config-url");
//                if (it != CGI.getElements().end())
//                {
//                    sConfigUrl = CxString::toLower(it->getValue());
//                }
//            }

//            int nRequestId = pushRequestInMap(oFcgiRequest);

//            idMapConfigUrl[nRequestId] = sConfigUrl;

//            //注册GA接收消息回调
//            GeneralAccess::registerMessageCommandDeal(GeneralAccessReturn);

//            //把配置数据写入配置文件中
//            if( nRequestId > -1 ){
//                //通过GA发送消息给专门进程处理

//                const PsmAttach *attach = GeneralAccess::defaultAttach();

//                std::string sCommand = "req.convert.2file";
//                std::string sParam = "request_id=" + CxString::toString(nRequestId) + ";config_url=" + sConfigUrl;
//                GeneralAccess::postMessage(sCommand,sParam,attach);
//            }

//        }

//        if (_popBufferFcgiRequests->size()>0)
//        {
//            _popBufferFcgiRequests->clear();
//        }
//    }
//}

//void CgiLocalDealThread::exit() {
//    _status = 0;
//    CxJoinableThread::exit();
//}


//CgiLocalDealThread * volatile f_oCgiLocalDealThread = NULL;


/*==================================================================================*/








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
                    //*url的参数command中带有convert的
//                    else if (sFnCode.find(".convert.") != string::npos)
//                    {
//                        f_oCgiLocalDealThread->pushEvtDeal(oFcgiRequest);
//                    }
                    //*url的参数command中带有savefile的
                    else if (sFnCode.find(".savefile.") != string::npos)
                    {
                        f_oCgiFileDealThread->pushEvtDeal(oFcgiRequest);
                    }
                    //**处理这类：{"session":"sbid=0001;xxx=adfadsf","structtype": "rtdata_v102","params":[{}]}
                    else
                    {
                        string sContentType = CGI.getEnvironment().getContentType();
//                        cxPrompt()<<sContentType;
                        //*ContentType是xml的请求处理
                        if (sContentType.find("xml") != string::npos)
                        {
                            string sPostData = CGI.getEnvironment().getPostData();
                            std::vector<std::map<std::string, std::string> > rows;
                            CxTinyXml::loadTable2Level(sPostData.data(), sPostData.size(), rows, "", "MSG");
                            string sOut;
                            for (size_t i = 0; i < rows.size(); ++i)
                            {
                                std::map<std::string, std::string> & row = rows.at(i);
                                int iCount = CxContainer::valueTo(row, string("COUNT"), ci_int_zero);
                                if (iCount>0)
                                {
                                    int iMid = CxContainer::valueTo(row, string("ADDRESS"), ci_int_zero);
                                    sOut += fn_measureGetArrayString(iMid, iCount);
                                }
                                else
                                {
                                    string sAddresses = CxContainer::valueTo(row, string("ADDRESSES"), string());
                                    if (sAddresses.size() > 0)
                                    {
                                        vector<string> addresses = CxString::split(sAddresses, ',');
                                        for (size_t j = 0; j < addresses.size(); ++j)
                                        {
                                            const string & sAddress = addresses.at(j);
                                            int iMid = CxString::toInt32(sAddress);
                                            sOut += fn_measureGetString(iMid);
                                        }
                                    }
                                    else
                                    {
                                        int iMid = CxContainer::valueTo(row, string("ADDRESS"), ci_int_zero);
                                        sOut += fn_measureGetString(iMid);
                                    }
                                }
                            }
                            IO << HTTPResponseHeader("HTTP/1.1", 200, "OK")
                                  .addHeader("Access-Control-Allow-Origin", "*")
                                  .addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization")
                                  .addHeader("Content-Type", "xml");
                            IO << f_sMeasureHeadXml;
                            IO << sOut;
                            IO << f_sMeasureEndXml << endl;
                        }
                        //ContentType是json的请求处理
                        else if (sContentType.find("json") != string::npos)
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
                            if (sStructType.find("rtdata_v101") != string::npos)
                            {
                                sOutBegin = CxString::format(sOutBegin.c_str(), "rtdata_v001");
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
                                        sOut += fn_measureGetString(iMid, ci_fileType_json);
//                                        cxDebug() << "debug-20160318 mid" << iMid;
//                                        cxDebug() << "debug-20160318 sOut" << sOut;
                                        if (i < vParams->Size()-1)
                                            sOut.push_back(',');
                                    }
                                }
                            }
                            //**structtype 为 rtdata_v102
                            else if (sStructType.find("rtdata_v102") != string::npos)
                            {
                                sOutBegin = CxString::format(sOutBegin.c_str(), "rtdata_v001");
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
                                        else
                                        {
                                            iMid = CxRapidJson::findMemberToInt(vMeasure, cs_mid);
                                        }
                                        int iCount = CxRapidJson::findMemberToInt(vMeasure, "count");
                                        string rOut = fn_measureGetArrayString(iMid, iCount, ci_fileType_json);
                                        if (rOut.size()>0)
                                        {
                                            sOut += rOut;
                                            if (i < vParams->Size()-1)
                                                sOut.push_back(',');
                                        }
                                    }
                                }
                            }
                            //**structtype 为 yk_v101，返回用：yk_r_v001
                            else if (sStructType.find("yk_v101") != string::npos)
                            {
                                sOutBegin = CxString::format(sOutBegin.c_str(), "yk_r_v001");
//                                cxPrompt()<<sFnCode<<sPostData;
//                                string sCmd = sFnCode+".json";
                                int iState = GeneralAccess::postMessage(sFnCode.c_str(),sPostData.c_str(),sPostData.length()) > 0;
                                msepoch_t dtNow = CxTime::currentSystemTime();
                                rapidjson::Value * vParams = CxRapidJson::findMember(d, "params");
                                if (vParams && vParams->IsArray())
                                {
                                    for (SizeType i = 0; i < vParams->Size(); ++i)
                                    {
                                        Value & vMeasure = (*vParams)[i];
                                        string sUrl = CxRapidJson::findMemberToString(vMeasure, cs_url);
                                        string sMid = CxRapidJson::findMemberToString(vMeasure, cs_mid);

                                        sOut += CxString::format(f_sYkBodyJson, sUrl.c_str(), sMid.c_str(), dtNow, iState);
//                                        cxDebug() << "debug-20160318 mid" << iMid;
//                                        cxDebug() << "debug-20160318 sOut" << sOut;
                                        if (i < vParams->Size()-1)
                                            sOut.push_back(',');
                                    }
                                }
                            }
                                // # structtype rtlog_v101
                            else if (sStructType.find("rtlog_v101") != string::npos)
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


void fn_dbHeartjumpLoad()
{
    //*load dbHeartJump
    f_iDbHeartJumpInterval = CxApplication::findConfig(CS_SectionProgramConfig, string("DbsHeartJumpInterval"), f_iDbHeartJumpInterval);
    f_sDbHeartJumpSql = CxApplication::findConfig(CS_SectionProgramConfig, string("DbsHeartJumpSql"), f_sDbHeartJumpSql);
    if (f_sDbHeartJumpSql.size()>0)
    {
        f_sDbHeartJumpSql = "sql=" + f_sDbHeartJumpSql;
    }
    f_dtDbHeartJumpRec = 0;
}

void fn_timeout_dbHeartJump(int iInterval)
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

#include <vdi/common_service.h>

int main(int argc, const char *argv[])
{
    CxApplication::init(argc, argv);

    //心跳服务初始化
    CommonService::init();

    FCGX_Init();
    f_requestDescriptorIndex = FCGX_OpenSocket(":9000", 0);
    if (f_requestDescriptorIndex<0)
    {
        cxPrompt() << "Error ! Error ! Error ! Can not open socket 9000. Please Enter Any Key To Exit.";
        CxConsoleInterinfo::waitInputLine(CxApplication::processEvents, 30 * 1000);
        return 0;
    }
    //only data init
    fn_initFcgiRequests();

    MeasureShareMemoryAll::open();

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

    if (f_oCgiSqlDealThread)
    {
        f_oCgiSqlDealThread->stop();
        delete f_oCgiSqlDealThread;
    }

    if (f_oCgiFileDealThread)
    {
        f_oCgiFileDealThread->stop();
        delete f_oCgiFileDealThread;
    }

    MeasureShareMemoryAll::close();

    return rExec;
}
