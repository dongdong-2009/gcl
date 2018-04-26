
/*

  json文件的编辑可以使用 json 的在线编辑器，网址：
  http://www.jsoneditoronline.org/

{
  "dbname": "YGCT",
  "tables": [
    {
      "tbname": "T_CFG_YGCT_TOOL_MEASURE2DB",
      "rows": [
        {
          "mid": 16777216,
          "url": "",
          "interval": 30000
        },
        {
          "mid": 33554432,
          "url": "",
          "interval": 30000
        },
        {
          "mids": "33554433 33554434 33554435",
          "interval": 30000
        },
        {
          "mid2mid": "33554443~33554445",
          "interval": 30000
        }
      ]
    }
  ]
}


XML文件节点TAG：YGCT->MSG->RealData41117->RealData41117List
标识符	字段名称	类型	长度	备注	强制/可选
URL	域地址	String	20
ADDRESS	内部的点号	String	10		M
VALUE	实时值	Decimal	18,3		M
PINZHI	品质	Integer	1	0-差，1-好	M
DATATIME	时间	DateTime			M
RESOURCE	源地址	String	64
EXT	扩展字段	String	256		M
SBID	计量设备ID	Integer	10	对应管理对象2100	M

*/


#include "gcl_measure2db.h"
#include <vdi/measure_sharememory_all.h>
#include <ccxx/cxrapidjson.h>


#include <ccxx/cxdatabase_sqlite.h>
#include <ccxx/cxdatabase_odbc.h>


using namespace std;

const char * f_sMeasureHeadXml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
        "\r\n<YGCT>"
        "\r\n<HEAD>"
        "\r\n<VERSION>1.0</VERSION>"
        "\r\n<SRC>0500000021</SRC>"
        "\r\n<DES>0500000003</DES>"
        "\r\n<MsgNo>41117</MsgNo>"
        "\r\n<MsgId>91d9e512-3695-4796-b063-306544be6f1f</MsgId>"
        "\r\n<MsgRef/>"
        "\r\n<TransDate>20160605143005</TransDate>"
        "\r\n<Reserve/>"
        "\r\n</HEAD>"
        "\r\n<MSG>"
        "\r\n<RealData41117>"
        ;

const char * f_sMeasureBodyXml =
        "\r\n<RealData41117List>"
        "\r\n<URL>-</URL>"
        "\r\n<ADDRESS>%d</ADDRESS>"
        "\r\n<VALUE>%s</VALUE>"
        "\r\n<PINZHI>1</PINZHI>"
        "\r\n<DATATIME>%s</DATATIME>"
        "\r\n<RESOURCE>-</RESOURCE>"
        "\r\n<EXT>-</EXT>"
        "\r\n<SBID>-</SBID>"
        "\r\n</RealData41117List>"
        ;

const char * f_sMeasureEndXml =
        "\r\n</RealData41117>"
        "\r\n</MSG>"
        "\r\n</YGCT>"
        ;

//{"code":"9999","measures":[{"url":"asm570a/001","mid":16777217,"v":1,"q":1,"t":1812345678},{"url":"asm570a/002","mid":16777218,"v":2,"q":1,"t":1812345679}]}
const char * f_sMeasureBodyJson =
        "{\"url\":\"%s\",\"mid\":%d,\"v\":\"%s\",\"q\":%d,\"t\":%lld,\"srcid\":%d,\"state\":%d}"
        ;

std::string fn_getMeasureValue(const int & value)
{
    return CxString::toString(value);
}

std::string fn_getMeasureValue(const double & value)
{
    char str[32];
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
    string sDatetime = CxTime::toString(oMeasure->changedTime, '\0', '\0', '\0');
    r = CxString::format(f_sMeasureBodyXml, oMeasure->measureId, sValue.c_str(), sDatetime.c_str());
    return r;
}

//iFileType =0：表示XML方式； =1：表示JSON方式
string fn_measureGetString(int iMid)
{
    string r;

    //*yx
    //遥信处理 vqt整形方式
    if (iMid>=0x01000000 && iMid<0x02000000)
    {
        r = fn_measureDataToXmlString<int>(MonsbShareMemory::memoryManager(), iMid);
    }
    //*yc
    //遥测处理 vqt整形方式
    else if (iMid>=0x02000000 && iMid<0x03000000)
    {
        r = fn_measureDataToXmlString<double>(YcaddShareMemory::memoryManager(), iMid);
    }
    //*yw
    //遥文处理 vqt整形方式
    else if (iMid>=0x03000000 && iMid<0x04000000)
    {
        r = fn_measureDataToXmlString<StrawValue>(StrawShareMemory::memoryManager(), iMid);
    }

    return r;
}

//*
//*
//* regist
//*
//*
static Measure2db * fn_oSlsServer()
{
    CxInterinfoIn::addObserver(Measure2db::interinfoIn_line);
    CxInterinfoIn::addObserver(Measure2db::interinfoIn_cmd);

    CxApplication::registStartFunction(Measure2db::start, 20001);
    CxApplication::registStopFunction(Measure2db::stop);

    GeneralAccess::registerMessageCommandDeal(Measure2db::dealMessage);
    GeneralAccess::registerFileWriteDeal(Measure2db::dealFileWrite);
    static Measure2db m;
    return &m;
}

static Measure2db * m_pTicketSvr = fn_oSlsServer();


//*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
//--- --- publish ---
//*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***

template<typename T>
class MeasurePublishBond
{
public:
    struct TimerAllMeasure
    {
        int interval;
        vector<int> measureIds;
        msepoch_t nextTime;

        TimerAllMeasure(int iInterval) :
            interval(iInterval),
            nextTime(0)
        {
        }
    };

protected:
    int _measureType;
    vector<TimerAllMeasure> _timerAllMeasures;

public:
    MeasurePublishBond() :
        _measureType(0)
    { }

    void init(int iMeasureType)
    {
        _measureType = iMeasureType;
    }

    TimerAllMeasure * allocate(int iInterval)
    {
        TimerAllMeasure * oTimerAllMeasure = NULL;
        for (size_t i = 0; i < _timerAllMeasures.size(); ++i)
        {
            TimerAllMeasure & timerAllMeasure = _timerAllMeasures.at(i);
            if (timerAllMeasure.interval == iInterval)
            {
                oTimerAllMeasure = & timerAllMeasure;
                break;
            }
        }
        if (! oTimerAllMeasure)
        {
            _timerAllMeasures.push_back(TimerAllMeasure(iInterval));
            oTimerAllMeasure = & ( _timerAllMeasures.back() );
        }
        return oTimerAllMeasure;
    }

    void update( int iMeasureId, int iInterval ) {
        if (g_fn_getMeasureType(iMeasureId) != _measureType) return;
        //clear iMeasureId
        for (size_t i = 0; i < _timerAllMeasures.size(); ++i)
        {
            TimerAllMeasure & timerAllMeasure = _timerAllMeasures.at(i);
            CxContainer::remove(timerAllMeasure.measureIds, iMeasureId);
        }

        TimerAllMeasure * oTimerAllMeasure = allocate(iInterval);
        oTimerAllMeasure->measureIds.push_back(iMeasureId);
    }

    int update(const std::map<string, string> &row) {
        int iInterval = CxContainer::valueTo(row, string("interval"), ci_int_zero);
        if (iInterval <= ci_int_zero) return 0;
        int iMid = CxContainer::valueTo(row, string("mid"), ci_int_zero);
        string sMids = CxContainer::valueTo(row, string("mids"), string());
        string sMid2mid = CxContainer::valueTo(row, string("mid2mid"), string());
        if (iMid > 0)
        {
            update(iMid, iInterval);
        }
        else if (sMids.size()>0)
        {
            vector<string> v = CxString::split(sMids,' ');
            for (size_t i = 0; i < v.size(); ++i)
            {
                int iMid2 = CxString::toInt32(v.at(i));
                if (iMid2>0)
                {
                    update(iMid2, iInterval);
                }
            }
        }
        else if (sMid2mid.size()>0 && CxString::contain(sMid2mid, "~"))
        {
            string sMeasureBegin = CxString::token(sMid2mid, '~');
            string sMeasureEnd = sMid2mid;
            int iMeasureBegin = CxString::toInt32(sMeasureBegin);
            int iMeasureEnd = CxString::toInt32(sMeasureEnd);
            for (int iMid = iMeasureBegin; iMid <= iMeasureEnd; ++iMid)
            {
                update( iMid, iInterval );
            }
        }

        return 0;
    }

    std::vector<std::string> reportSelf() {
        vector<string> sReports;
        sReports.push_back( CxString::format("{report: MeasurePublishBond.%s" , g_fn_getMeasureTypeName(_measureType).c_str() ) );
        //<timerAllMeasures>
        sReports.push_back( string(",timerMeasures:[") );
        for (size_t i = 0; i < _timerAllMeasures.size(); ++i)
        {
            TimerAllMeasure & timerAllMeasure = _timerAllMeasures.at(i);
            if (i==0)
                sReports.push_back(CxString::format("{interval: %d, mids:[", timerAllMeasure.interval));
            else
                sReports.push_back(CxString::format(",{interval: %d, mids:[", timerAllMeasure.interval));
            sReports.push_back(CxContainer::joinToHexstring(timerAllMeasure.measureIds,','));
            sReports.push_back("]}");
        }
        sReports.push_back( string("]") );
        //</timerAllMeasures>
        sReports.push_back( string("}") );
        return sReports;
    }

    void publishAllMeasure(msepoch_t dtNow, typename MeasureShareMemory<T>::MemoryManagerBase * oMeasureManager) {
        vector<string> sOuts;
        for (size_t i = 0; i < _timerAllMeasures.size(); ++i)
        {
            TimerAllMeasure & timerAllMeasure = _timerAllMeasures.at(i);
            if (dtNow > timerAllMeasure.nextTime && timerAllMeasure.interval > 0 && timerAllMeasure.measureIds.size()>0)
            {
                vector<int> & measureIds = timerAllMeasure.measureIds;
                string sOut;
                for (size_t i = 0; i < measureIds.size(); ++i)
                {
                    int iMeasureId = measureIds.at(i);
                    //从共享内存中读数据
                    typename MeasureShareMemory<T>::MeasureData * oMeasure = oMeasureManager->measureData(iMeasureId);
                    if (oMeasure == NULL) continue;

                    sOut += fn_measureGetString(iMeasureId);
                }
                sOuts.push_back(sOut);
                timerAllMeasure.nextTime = dtNow + timerAllMeasure.interval;
            }
        }
        for (size_t i = 0; i < sOuts.size(); ++i)
        {
            string sOut = sOuts.at(i);
            if (sOut.size()>0)
            {
                Measure2db::doXml2db(f_sMeasureHeadXml + sOut + f_sMeasureEndXml);
            }
        }
    }

};

//*YX
class MonsbPublishBond : public MeasurePublishBond<int>
{
public:
};
static MonsbPublishBond f_mMonsbPublishBond;



//*YC
class YcaddPublishBond : public MeasurePublishBond<double>
{
public:
};
static YcaddPublishBond f_mYcaddPublishBond;



//*YW
class StrawPublishBond : public MeasurePublishBond<StrawValue>
{
public:
};
static StrawPublishBond f_mStrawPublishBond;












void Measure2db::start()
{
    //让 MeasurePublishBond 知道 自己是什么 Type
    f_mMonsbPublishBond.init(MonsbShareMemory::memoryManager()->getMeasureType());
    f_mYcaddPublishBond.init(YcaddShareMemory::memoryManager()->getMeasureType());
    f_mStrawPublishBond.init(StrawShareMemory::memoryManager()->getMeasureType());

    loadMeasureConfig(CxFileSystem::mergeFilePath( CxApplication::configPath(), CxApplication::applicationTargetName() + ".extend.json" ));

    CxTimerManager::startTimer(Measure2db::timerDo, 1000);
}

void Measure2db::stop()
{
    CxTimerManager::stopTimer(Measure2db::timerDo);
}

int Measure2db::dealMessage(const PsmExplainer *oPsm, const string &sMessageCommand, const string &sMessageParam, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    if(sMessageCommand.find(".file2db.track_weigh") != string::npos)
    {
        return TRUE;
    }
    return FALSE;
}

int Measure2db::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    if (fileData.empty())
        return FALSE;
    return FALSE;
}

void Measure2db::update(const std::map<string, string> &row)
{
    if (! f_mMonsbPublishBond.update(row))
        if (! f_mYcaddPublishBond.update(row))
            f_mStrawPublishBond.update(row);
}

std::vector<string> Measure2db::reportSelf()
{
    vector<string> sReports;
    sReports.push_back( string("{PublishBond:[") );
    CxContainer::append(sReports, f_mMonsbPublishBond.reportSelf());
    sReports.push_back( string(",") );
    CxContainer::append(sReports, f_mYcaddPublishBond.reportSelf());
    sReports.push_back( string(",") );
    CxContainer::append(sReports, f_mStrawPublishBond.reportSelf());
    sReports.push_back( string("]}") );
    return sReports;
}

void Measure2db::timerDo(int iInterval)
{
    static int iTimes = 0;
    iTimes++;

    //*prepare data
    CxDatabase * oDb = CxDatabaseManager::getDefaultDb();
    if (! oDb)
    {
        outPrompt() << "Error ! Error ! Error ! Do Not Config DataBase. ";
        return;
    }
    //
    if (! oDb->isOpen())
    {
        oDb->openDatabase();
    }

    outPrompt() << "->start scan.";

    //定时 1000 毫秒就 check publish（如果到了发布时间 nextTime 就发布）
    if (MonsbShareMemory::memoryManager()->isValid())
    {
        f_mMonsbPublishBond.publishAllMeasure(CxTime::currentSystemTime(), MonsbShareMemory::memoryManager());
    }
    if (YcaddShareMemory::memoryManager()->isValid())
    {
        f_mYcaddPublishBond.publishAllMeasure(CxTime::currentSystemTime(), YcaddShareMemory::memoryManager());
    }
    if (StrawShareMemory::memoryManager()->isValid())
    {
        f_mStrawPublishBond.publishAllMeasure(CxTime::currentSystemTime(), StrawShareMemory::memoryManager());
    }

    //*heartJump 10 * 1000 毫秒
    if (iTimes % 10 == 0)
    {
        string sHeartJumpSql = CxContainer::value(oDb->databaseAttrs(), string("HeartJumpSql"));;
        if (sHeartJumpSql.size()>0)
        {
            int iResult = oDb->execSql(sHeartJumpSql);
            outPrompt() << "Database HeartJump execute sql result=" << iResult << ";";
        }
    }
}

void Measure2db::doXml2db(const string &sXml41117)
{
    CxDatabase * oDb = CxDatabase::getDefaultDb();
    if (! ( oDb && oDb->isOpen() ) )
    {
        cxPrompt()<<"File2dbTrackWeigh Fail : DataBase is null || not open!!!";
        return;
    }

    int iWrited41170 = 0;
    //*2T_CMM_BI_MC
    //string sInsertValues = string.Format("'{0}', null, 'MC', 'CICS系统', 'v4.0', '{1}', 'DB', '{2}', '1', '{3}', 'ICS.MC_M4.0', '{4}', null, '0', null, null, '0'",
    //Guid.NewGuid().ToString(), sMsgNo, sNow, xml, sNow);
    //            _sqlTableInfo = bOk ? "SELECT * FROM %s LIMIT 1;" : "SELECT TOP 1 * FROM %s;";
    if(sXml41117.size()>0)
    {
        string sTableName = "T_CMM_BI_MC";
        vector<string> sColumnNames;
        sColumnNames.push_back(string("F_ID"            ));     //sRow.push_back(string(CxString::toString(CxTime::currentMsepoch())));
        sColumnNames.push_back(string("F_PID"           ));     //sRow.push_back(string());
        sColumnNames.push_back(string("F_SRC"    ));            //sRow.push_back(string("MC"));
        sColumnNames.push_back(string("F_DEST"));               //sRow.push_back(string("CICS系统"));
        sColumnNames.push_back(string("F_VER"));                //sRow.push_back(string("v4.0"));
        sColumnNames.push_back(string("F_MSG"));                //sRow.push_back(string("msgno_xx"));
        sColumnNames.push_back(string("F_CLASS"         ));     //sRow.push_back(string("DB"));
        sColumnNames.push_back(string("F_DT"            ));     //sRow.push_back(CxTime::currentSepochString());
        sColumnNames.push_back(string("F_TYPE"     ));          //sRow.push_back(string("msgno_xx"));
        sColumnNames.push_back(string("F_V"      ));            //sRow.push_back("xml");
        sColumnNames.push_back(string("F_USER_CRT"      ));     //sRow.push_back("ICS.MC_M4.0");
        sColumnNames.push_back(string("F_DT_CRT"      ));       //sRow.push_back(CxTime::currentSepochString());
//        sColumnNames.push_back(string("F_USER_MOD"      ));     //sRow.push_back(string("null"));
//        sColumnNames.push_back(string("F_DT_MOD"      ));       //sRow.push_back(string("0"));
//        sColumnNames.push_back(string("F_RES0"      ));         //sRow.push_back(string("null"));
//        sColumnNames.push_back(string("F_RES1"      ));         //sRow.push_back(string("null"));
        sColumnNames.push_back(string("F_SYN_FLAG"      ));     //sRow.push_back(string("0"));

        vector<vector<string> > sRows;
        string sMsgNo;

        //*insert Table[T_CMM_BI_MC:Weight
        vector<string> sRow41117;;
        sMsgNo = "41117";
        sRow41117.push_back(string(CxString::toString(CxTime::currentMsepoch())));
        sRow41117.push_back(string());
        sRow41117.push_back(string("MC"));
        sRow41117.push_back(string("CICS系统"));
        sRow41117.push_back(string("v4.0"));
        sRow41117.push_back(sMsgNo);
        sRow41117.push_back(string("AMQ")); // AMQ : active mq send; DB : db send;
        sRow41117.push_back(CxTime::currentSepochString());
        sRow41117.push_back(string("1")); // 1：表示向外发；0：表示接收；
        sRow41117.push_back(sXml41117);
        sRow41117.push_back(string("ICS.MC_M4.0"));
        sRow41117.push_back(CxTime::currentSepochString());
//          sRow.push_back(string("null"));
//          sRow.push_back(string("0"));
//          sRow.push_back(string("null"));
//          sRow.push_back(string("null"));
        sRow41117.push_back(string("0"));

        sRows.clear();
        sRows.push_back(sRow41117);
        iWrited41170 = oDb->saveTable(sTableName, sColumnNames, sRows);
        cxPrompt()<<"DataBase Insert T_CMM_BI_MC(41117) result=" << iWrited41170;

        string sTempPath = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), "temp");
        sTempPath = CxFileSystem::mergeFilePath(sTempPath, CxApplication::applicationTargetName());
        if (! CxFileSystem::isExist(sTempPath)) CxFileSystem::createDirMultiLevel(sTempPath);
        CxFile::save(CxString::format(CxFileSystem::mergeFilePath( sTempPath, CxTime::currentMsepochString(0,0,0)+"_%d.xml").c_str(), iWrited41170), sXml41117);
    }

}

/***
 m2db -update ssid=1;name=xxx;ttid=2

*/
int Measure2db::interinfoIn_line(const string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

int Measure2db::interinfoIn_cmd(const string &sCommand, const std::map<string, string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    if (sCommand == "m2db" || sCommand == "measure2db")
    {
        if (CxContainer::contain(sParams, std::string("update")))
        {
            std::string sUpdateValue = CxContainer::value(sParams, std::string("update"));
            string sFilePath = CxApplication::configExtendFilePath();
            if (sUpdateValue.size() > 0)
            {
                if (CxFileSystem::isExist(sUpdateValue))
                {
                    sFilePath = sUpdateValue;
                }
                else
                {
                    Measure2db::update(sParams);

                    cxPrompt() << "m2db update success : update by contents - " << sParams;

                    return CxInterinfo::InDeal_Ed;
                }
            }
            if (CxFileSystem::isExist(sFilePath))
            {
                loadMeasureConfig(sFilePath);
                return CxInterinfo::InDeal_Ed;
            }
            else
            {
                cxPrompt() << "measure2db update fail : update by filePath - " << sFilePath;
            }
        }
        if (CxContainer::contain(sParams, std::string("report")))
        {
            string sReportFilePath = CxFileSystem::mergeFilePath( CxApplication::applicationDeployPath(), "report" );
            sReportFilePath = CxFileSystem::mergeFilePath( sReportFilePath, CxApplication::applicationTargetName() + ".report.txt" );
            vector<string> sReports;

            CxContainer::append(sReports, Measure2db::reportSelf());

            if (CxFile::save(sReportFilePath, sReports)>0)
            {
                cxPrompt() << "measure2db report success , and save to file " << sReportFilePath;
            }
            else
            {
                cxPrompt() << "measure2db report fail!!!" << sReportFilePath;
            }
            return CxInterinfo::InDeal_Ed;
        }
        if (CxContainer::contain(sParams, std::string("exit")) && CxContainer::contain(sParams, std::string("test135246")))
        {
            CxApplication::exit();
            return CxInterinfo::InDeal_Ed;
        }
        return CxInterinfo::InDeal_Ed;
    }
    return 0;
}

void Measure2db::loadMeasureConfig(const std::string & sFilePath)
{
    cxDebug() << "measure2db load config: " << sFilePath;
    if (CxFileSystem::isExist(sFilePath))
    {
        vector<map<string, string> > rows;
        if ( CxRapidJson::loadTable(sFilePath, rows) > 0 )
        {
            for (size_t i = 0; i < rows.size(); ++i)
            {
                const std::map<std::string, std::string> & row = rows.at(i);
                f_mMonsbPublishBond.update(row);
                f_mYcaddPublishBond.update(row);
                f_mStrawPublishBond.update(row);
            }
        }
        else
        {
            cxDebug() << "measure2db load config fail.";
        }
        cxDebug() << "measure2db load config finish.";
    }
    else
    {
        cxDebug() << "file : " << sFilePath << " don't exist!!";
    }
}
