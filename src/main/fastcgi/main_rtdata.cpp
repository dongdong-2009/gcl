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

#include <vdi/measure_sharememory_all.h>

#include <psm/general_access.h>
#include <common/gcl_resource.h>

#include <vdi/common_service.h>

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

const char cs_url      [] = "url";
const char cs_sql      [] = "sql";
const char cs_type     [] = "type";
const char cs_name     [] = "name";
const char cs_title    [] = "title";
const char cs_desc     [] = "desc";
const char cs_mid      [] = "mid";
const char cs_v        [] = "v";
const char cs_q        [] = "q";
const char cs_t        [] = "t";
const char cs_state    [] = "state";
const char cs_dtday    [] = "dtday";
const char cs_dtbegin  [] = "dtbegin";
const char cs_dtend    [] = "dtend";


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



//************************************************************************************************************
//------------------------------------------------------------------------------------------------------------
//************************************************************************************************************


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
                                        else
                                        {
                                            iMid = CxRapidJson::findMemberToInt(vMeasure, cs_mid);
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
    f_requestDescriptorIndex = FCGX_OpenSocket(":9173", 0);
    if (f_requestDescriptorIndex<0)
    {
        cxPrompt() << "Error ! Error ! Error ! Can not open socket 9000. Please Enter Any Key To Exit.";
        CxConsoleInterinfo::waitInputLine(CxApplication::processEvents, 30 * 1000);
        return 0;
    }
    //only data init
    fn_initFcgiRequests();

    MeasureShareMemoryAll::open();

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
    MeasureShareMemoryAll::close();

    return rExec;
}
