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

 */

#include <ccxx/ccxx.h>

#include <iomanip>

#include <vdi/measure_sharememory_all.h>

#include <fcgi_config.h>
#include <fcgiapp.h>

#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTTPResponseHeader.h>
#include <cgicc/HTMLClasses.h>

#include <contrib/FCgiIO.h>

using namespace std;
using namespace cgicc;

const char * f_sHead =
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

const char * f_sBody =
        "\r\n		<RealData9999>"
        "\r\n			<URL>YIYANG.DCS01.RLPDC03</URL>"
        "\r\n			<ADDRESS>%d</ADDRESS>"
        "\r\n			<DATATIME>%s</DATATIME>"
        "\r\n			<VALUE>%s</VALUE>"
        "\r\n			<Q>%d</Q>"
        "\r\n		</RealData9999>"
        ;

const char * f_sEnd =
        "\r\n	</MSG>"
        "\r\n</YGCT>"
        ;

std::string fn_getMeasureValue(const int & value)
{
    return CxString::toString(value);
}

std::string fn_getMeasureValue(const double & value)
{
        char str[32];
        sprintf(str, "%21.4lf", value);
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

template<typename T>
string fn_measureDataToString(const typename MeasureShareMemory<T>::MeasureData * const oMeasureData)
{
    string r;
    const typename MeasureShareMemory<T>::MeasureData * oMeasure = oMeasureData;
    string sValue = fn_getMeasureValue(oMeasure->value);
    string sDatetime = CxTime::toString(oMeasure->changedTime);
    r = CxString::format(f_sBody, oMeasure->measureId, sDatetime.c_str(), sValue.c_str(), oMeasure->quality);
    return r;
}

template<typename T>
string fn_measureChangedToString(const typename MeasureShareMemory<T>::ChangedData * const oMeasureChanged)
{
    string r;
    const typename MeasureShareMemory<T>::ChangedData * oChangedData = oMeasureChanged;
    string sValue = fn_getMeasureValue(oChangedData->value);
    string sDatetime = CxTime::toString(oChangedData->changedTime);
    r = CxString::format(f_sBody, oChangedData->measureId, sDatetime.c_str(), sValue.c_str(), oChangedData->quality);
    return r;
}

template<typename T>
string fn_measureChangedsToString(const typename MeasureShareMemory<T>::ChangedData * const oChangedBegin, const typename MeasureShareMemory<T>::ChangedData * const oChangedEnd, const std::string & sSqlInsert)
{
    string r;
    const typename MeasureShareMemory<T>::ChangedData * oChangedData = oChangedBegin;
    while (oChangedData < oChangedEnd)
    {
        r += fn_measureChangedToString(oChangedData);
        oChangedData++;
    }
    return r;
}


string fn_measureGetArrayString(int iBeginMid, int iMidCount)
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

        for (int i = iBeginMid; i < iEndMid; ++i)
        {
            MonsbShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(i);
            if (! oMeasureData) continue;
            r += fn_measureDataToString<int>(oMeasureData);
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

        for (int i = iBeginMid; i < iEndMid; ++i)
        {
            YcaddShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(i);
            if (! oMeasureData) continue;
            r += fn_measureDataToString<double>(oMeasureData);
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

        for (int i = iBeginMid; i < iEndMid; ++i)
        {
            StrawShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(i);
            if (! oMeasureData) continue;
            r += fn_measureDataToString<StrawValue>(oMeasureData);
        }
    }

    return r;
}

string fn_measureGetString(int iMid)
{
    string r;

    //*yx
    //遥信处理 vqt整形方式
    if (iMid>=0x01000000 && iMid<0x02000000)
    {
        MonsbMemoryManager * oMemoryManager = MonsbShareMemory::memoryManager();
        MonsbShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(iMid);
        if (! oMeasureData) return r;
        r = fn_measureDataToString<int>(oMeasureData);
    }
    //*yc
    //遥测处理 vqt整形方式
    else if (iMid>=0x02000000 && iMid<0x03000000)
    {
        YcaddMemoryManager * oMemoryManager = YcaddShareMemory::memoryManager();
        YcaddShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(iMid);
        if (! oMeasureData) return r;
        r = fn_measureDataToString<double>(oMeasureData);
    }
    //*yw
    //遥文处理 vqt整形方式
    else if (iMid>=0x03000000 && iMid<0x04000000)
    {
        StrawMemoryManager * oMemoryManager = StrawShareMemory::memoryManager();
        StrawShareMemory::MeasureData * oMeasureData = oMemoryManager->measureData(iMid);
        if (! oMeasureData) return r;
        r = fn_measureDataToString<StrawValue>(oMeasureData);
    }

    return r;
}


class FastcgiDealThread : public CxJoinableThread
{
public:
    static void startFastcgiDeal();

    static void stopFastcgiDeal();

    FastcgiDealThread()
    {
        _isStarted = false;
    }
    ~FastcgiDealThread()
    {
    }

protected:
    void out(FCgiIO & IO, Cgicc & CGI, const std::string & sTitle, const std::string & sInfo)
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

    void run()
    {
        FCGX_Init();
        int socket = FCGX_OpenSocket(":9000", 0);
        char* remote_addr;

        FCGX_Request request;
        if (FCGX_InitRequest(&request, socket, 0) != 0)
        {
            cxWarn() << "FCGX_InitRequest fail. socket=" << socket;
            return;
        }

        while (_isStarted)
        {
            FCGX_Accept_r(&request);
            try
            {
                FCgiIO IO(request);
                Cgicc CGI(&IO);

                remote_addr = FCGX_GetParam("REMOTE_ADDR", request.envp);

                string sRequestMethod = CxString::toLower(CGI.getEnvironment().getRequestMethod());
                if (sRequestMethod.find("options") != string::npos)
                {
                    IO << HTTPResponseHeader("HTTP/1.1", 0, "OPTIONS Authorization")
                          .addHeader("Access-Control-Allow-Origin", "*")
                          .addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS")
                          .addHeader("Access-Control-Allow-Headers", "POWERED-BY-AID,Content-Type")
                          .addHeader("Access-Control-Max-Age", "30")
                       << endl;
                    //                    cout << HTTPResponseHeader("HTTP/1.1", 401, "Unauthorized")
                    //                     .addHeader("WWW-Authenticate", "NTLM")
                    //                     .addHeader("WWW-Authenticate",  "Basic realm=\"cgicc\"");

                    //                    response.addHeader("Access-Control-Allow-Origin", "*");
                    //                    response.addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

                    //                    response.setMimeType("application/json");
                }
                else
                {
                    string sContentType = CGI.getEnvironment().getContentType();
                    if (sContentType.find("xml") != string::npos)
                    {
                        string sPostData = CGI.getEnvironment().getPostData();
                        std::vector<std::map<std::string, std::string> > rows;
                        CxXml::loadTable2Level(sPostData.data(), sPostData.size(), rows, "", "MSG");
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
                              .addHeader("Content-Type", "xml")
                           << f_sHead << sOut << f_sEnd << endl;
//                           << HTTPContentHeader("xml") << f_sHead << sOut << f_sEnd << endl;
                        //                    cout << HTTPResponseHeader("HTTP/1.1", 401, "Unauthorized")
                        //                     .addHeader("WWW-Authenticate", "NTLM")
                        //                     .addHeader("WWW-Authenticate",  "Basic realm=\"cgicc\"");

                        //                    response.addHeader("Access-Control-Allow-Origin", "*");
                        //                    response.addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

                        //                    response.setMimeType("application/json");

                    }
                    else if (sContentType.find("json") != string::npos)
                    {
                        out(IO, CGI, "planting", "now do not support content-type" + sContentType);
                    }
                    else
                    {
                        out(IO, CGI, "error", "can not deal content-type" + sContentType);
                    }
                }

    //            for(const_form_iterator i = CGI.getElements().begin(); i != CGI.getElements().end(); ++i )
    //            {
    //                IO << i->getName() << " = " << i->getValue() << br() << endl;
    //            }

    ///*做测试、实验用*/
    //            std::string s;
    //            CxFile::load("c:/1.xml", s);
    //            IO << HTTPContentHeader("xml") << s << endl;
    ///*needtodo: 这句只多了一个 endl，但前端解释出问题。要检查。IO << HTTPContentHeader("xml") << endl << s << endl;*/
    //            FCGX_PutS("Content-Type: xml\n\n", request.out);
    //            //        FCGX_PutS("Content-Type: text/xml\n\n", request.out);
    //            FCGX_PutStr(s.data(), s.size(), request.out);
    //            FCGX_PutS("\n", request.out);

    //test1
    /*
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
            catch(const exception&)
            {
                // handle error condition
            }

            FCGX_Finish_r(&request);
        }
    }

private:
    volatile bool _isStarted;

    inline void waitExit() { join(); }

};

FastcgiDealThread * fn_threadTimerInit()
{
    CxApplication::registStartFunction(FastcgiDealThread::startFastcgiDeal);
    CxApplication::registStopFunction(FastcgiDealThread::stopFastcgiDeal);
    static FastcgiDealThread m;
    return & m;
}
static FastcgiDealThread * f_oThreadTimer = fn_threadTimerInit();

void FastcgiDealThread::startFastcgiDeal()
{
    if (f_oThreadTimer && ! f_oThreadTimer->_isStarted)
    {
#ifdef GM_OS_WIN
        //THREAD_PRIORITY_TIME_CRITICAL
        int iPriority = THREAD_PRIORITY_HIGHEST;
#else
        //SCHED_OTHER,SCHED_FIFO,SCHED_RR
        int iPriority = 2;
#endif
        f_oThreadTimer->_isStarted = true;
        f_oThreadTimer->start(iPriority);
    }
}

void FastcgiDealThread::stopFastcgiDeal()
{
    if (f_oThreadTimer && f_oThreadTimer->_isStarted)
    {
        f_oThreadTimer->_isStarted = false;
        f_oThreadTimer->waitExit();

        MeasureShareMemoryAll::close();
    }
}

int
main(int argc,
     const char *argv[],
     char **/*envp*/)
{
    CxApplication::init(argc, argv);

    MeasureShareMemoryAll::open();

    return CxApplication::exec();
}


int main1(void)
{
    FCGX_Init();
    int socket = FCGX_OpenSocket(":9000", 0);
    char* remote_addr;

    FCGX_Request request;
    if (FCGX_InitRequest(&request, socket, 0) != 0)
        return 1;

    //    CxTime::currentMsepoch();
    //    std::string s;
    //    CxFile::load("c:/1.jpg", s);

    while (1) {
        FCGX_Accept_r(&request);
        remote_addr = FCGX_GetParam("REMOTE_ADDR", request.envp);

        //        FCGX_PutS("Content-type: text/plain\n\n", request.out);
        //        FCGX_PutS(remote_addr, request.out);
        //        FCGX_PutS("\n", request.out);

        //        FCGX_PutS("Content-Type: image/jpeg\n\n", request.out);

        std::string s;
        //    CxFile::load("c:/1.jpg", s);
        CxFile::load("c:/1.xml", s);

        FCGX_PutS("Content-Type: xml\n\n", request.out);
        //        FCGX_PutS("Content-Type: text/xml\n\n", request.out);
        FCGX_PutStr(s.data(), s.size(), request.out);
        FCGX_PutS("\n", request.out);

        FCGX_Finish_r(&request);
    }
}
