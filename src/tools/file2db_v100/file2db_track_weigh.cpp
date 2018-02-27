
/*
 *
CREATE TABLE `T_LIS_BI_TRACK_WEIGHT` (
  `F_ID` varchar(36) NOT NULL COMMENT '主键',
  `F_PID` varchar(36) DEFAULT NULL COMMENT '父ID',
  `F_TRACK_NAME` varchar(64) DEFAULT NULL COMMENT '轨道名',
  `F_TRACK_FILENAME` varchar(256) DEFAULT NULL COMMENT '轨道数据文件名',
  `F_TRACK_FILESIZE` bigint(20) DEFAULT '0' COMMENT '轨道数据文件大小',
  `F_TRACK_FILEDATA` text COMMENT '重要：轨道数据文件数据内容',
  `F_COUNT` int(11) DEFAULT '0' COMMENT '顺序号的个数',
  `F_DT` bigint(20) DEFAULT '0' COMMENT 'unix时间的毫秒数',
  `F_DT_STRING` varchar(32) DEFAULT NULL,
  `F_MESSAGE` varchar(255) DEFAULT NULL,
  `F_SYN_FLAG` int(11) DEFAULT '0' COMMENT '同步标志 0 新建 +1第几次修改',
  PRIMARY KEY (`F_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8

*/

#include "file2db_track_weigh.h"


using namespace std;


//*
//*
//* regist
//*
//*
static File2dbTrackWeigh * fn_oSlsServer()
{
    CxApplication::registStartFunction(File2dbTrackWeigh::start, 20001);
    CxApplication::registStopFunction(File2dbTrackWeigh::stop);
    GeneralAccess::registerMessageCommandDeal(File2dbTrackWeigh::dealMessage);
    GeneralAccess::registerFileWriteDeal(File2dbTrackWeigh::dealFileWrite);
    static File2dbTrackWeigh m;
    return &m;
}

static File2dbTrackWeigh * m_pTicketSvr = fn_oSlsServer();


static string CS_SectionTrack = "Track";
static string f_sScanPath;
static int f_iScanInterval;
static string f_sIcs2CicsTrackPath;
static string f_sTrackName;
static string f_sCheXing;


static msepoch_t f_scanlastWriteTime = 0;
static map<string, int> f_scanFileNames;


static string fn_getTrackFileName(const string & sFileName)
{
    return f_sTrackName + "_" + sFileName;
}

#define ci_track_weight_scan_mode_filename (1)
#define ci_track_weight_scan_mode_writedtime (2)
static int fn_iScanMode()
{
    static int iScanMode = 0;
    if (iScanMode == 0)
    {
        iScanMode = CxApplication::findConfig(CS_SectionTrack, "ScanMode", int(ci_track_weight_scan_mode_filename));
    }
    return iScanMode;
}

static int fn_compare_order_pathInfo(const CxFilePathInfo& v1, const CxFilePathInfo& v2)
{
    return v2.lastWriteTime > v1.lastWriteTime;
}

static void fn_sort_pathInfo(std::vector<CxFilePathInfo> & vec1)
{
    std::sort(vec1.begin(), vec1.end(), fn_compare_order_pathInfo);
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

void File2dbTrackWeigh::start()
{
    f_sIcs2CicsTrackPath = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), "temp");
    f_sIcs2CicsTrackPath = CxFileSystem::mergeFilePath(f_sIcs2CicsTrackPath, "ics2cics_track");
    if (! CxFileSystem::isExist(f_sIcs2CicsTrackPath))
    {
        CxFileSystem::createDirMultiLevel(f_sIcs2CicsTrackPath);
    }

    f_sScanPath = CxApplication::findConfig(CS_SectionTrack, string("ScanPath"), std::string());
    if (f_sScanPath.empty())
    {
        cxPrompt() << "Track Weight's ScanPath is empty!!! Do not scan.";
        cxPrompt() << "Track Weight's ScanPath is empty!!! Do not scan.";
        cxPrompt() << "Track Weight's ScanPath is empty!!! Do not scan.";
        return;
    }
    if ( ! CxFileSystem::hasRootPath(f_sScanPath) )
    {
        f_sScanPath = CxFileSystem::cd( CxApplication::applicationDeployPath() , f_sScanPath );
    }

    f_iScanInterval = CxApplication::findConfig(CS_SectionTrack, string("ScanInterval"), 3000);
    if (f_iScanInterval < 3000) f_iScanInterval = 3000;

    f_sTrackName = CxApplication::findConfig(CS_SectionTrack, string("TrackName"), std::string("1"));

    f_sCheXing = CxApplication::findConfig(CS_SectionTrack, string("CheXing"), std::string("C62"));

    CxTimerManager::startTimer(File2dbTrackWeigh::timerDo, f_iScanInterval);

    cxPrompt() << "TrackName: " << f_sTrackName;
    cxPrompt() << "ScanPath: " << f_sScanPath;
    cxPrompt() << "ScanInterval: " << f_iScanInterval;
    cxPrompt() << "CheXing: " << f_sCheXing;
}

void File2dbTrackWeigh::stop()
{
}

int File2dbTrackWeigh::dealMessage(const PsmExplainer *oPsm, const string &sMessageCommand, const string &sMessageParam, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    if(sMessageCommand.find(".file2db.track_weigh") != string::npos)
    {
        return TRUE;
    }
    return FALSE;
}

int File2dbTrackWeigh::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    if (fileData.empty())
        return FALSE;
    return FALSE;
}

void File2dbTrackWeigh::timerDo(int iInterval)
{
    static int iTimes = 0;
    ++ iTimes;

    outPrompt() << "->start scan.";

    string sScanPath = f_sScanPath;
//    string sCmd = CxString::format('cd \"%s\"', sScanPath);
//    ::system( sCmd.c_str() );
    if ( sScanPath.empty() || ! CxFileSystem::isExist( sScanPath ) )
    {
        outPrompt() << "Error ! Error ! Error ! ScanPath Do Not Exist. : " << sScanPath;
        return;
    }

    int iScanMOde = fn_iScanMode();

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
    //
    if (iScanMOde == ci_track_weight_scan_mode_filename)
    {
        if (oDb->isOpen() && (f_scanFileNames.empty() || (iTimes % 10 == 0)))
        {
            f_scanFileNames.clear();
            std::vector<std::map<std::string, std::string> > fileNameSyncs = oDb->queryToMapVector("select F_TRACK_FILENAME as 'FILENAME' , F_SYN_FLAG as 'SYNC' from T_LIS_BI_TRACK_WEIGHT;");
            for (size_t i = 0; i < fileNameSyncs.size(); ++i)
            {
                const std::map<std::string, std::string> & fileNameSync = fileNameSyncs.at(i);
                string sFileName = CxContainer::value(fileNameSync, string("FILENAME"));
                if (sFileName.size()>0)
                {
                    f_scanFileNames[sFileName] = CxContainer::valueTo(fileNameSync, string("SYNC"), ci_int_minus_one);
                }
            }
        }
    }
    else if (iScanMOde == ci_track_weight_scan_mode_writedtime)
    {
        if (f_scanlastWriteTime == 0)
        {
            string sScanlastWriteTime = oDb->queryValue("select max(F_DT) as 'ScanlastWriteTime' from T_LIS_BI_TRACK_WEIGHT;");
            if (sScanlastWriteTime.size()>0)
            {
                msepoch_t dt = CxString::toInt64(sScanlastWriteTime);
                if (dt > 0)
                {
                    f_scanlastWriteTime = dt;
                }
            }
            outPrompt() << " ScanlastWriteTime : " << CxTime::toString(f_scanlastWriteTime);
        }
    }

    //*heartJump
    if (iTimes % 10 == 0)
    {
        string sHeartJumpSql = CxContainer::value(oDb->databaseAttrs(), string("HeartJumpSql"));;
        if (sHeartJumpSql.size()>0)
        {
            int iResult = oDb->execSql(sHeartJumpSql);
            outPrompt() << "Database HeartJump execute sql result=" << iResult << ";";
        }
    }

    //* scan time
    outPrompt() << "{";
    outPrompt() << "ScanBegin:" << CxTime::currentMsepochString();
    vector<CxFileSystem::PathInfo> pathInfos;
    CxFileSystem::scanDir(sScanPath, pathInfos, false);
    if (pathInfos.size()>0)
    {
        fn_sort_pathInfo(pathInfos);
        outPrompt() << "ScanFileCount:" << pathInfos.size();
        for (size_t i = 0; i < pathInfos.size(); ++i)
        {
            const CxFileSystem::PathInfo & pathInfo = pathInfos.at(i);
            if (pathInfo.pathType != CxFileSystem::PathTypeFile) continue;
            //注意
            if (! CxString::equalCase( CxFileSystem::extractFileSuffixName(pathInfo.fileName) , ".txt") ) continue;
            if (iScanMOde == ci_track_weight_scan_mode_filename)
            {
                if (! CxContainer::contain(f_scanFileNames, fn_getTrackFileName(pathInfo.fileName) ))
                {
                     doFile2db(pathInfo);
                }
            }
            else if (iScanMOde == ci_track_weight_scan_mode_writedtime)
            {
                if (pathInfo.lastWriteTime > f_scanlastWriteTime)
                {
                    doFile2db(pathInfo);
                }
            }
        }
    }
    outPrompt() << "ScanEnd:" << CxTime::currentMsepochString();
    outPrompt() << "}";
}

/*
 *

CREATE TABLE `T_LIS_BI_TRACK_WEIGHT` (
  `F_ID` varchar(36) NOT NULL COMMENT '主键',
  `F_PID` varchar(36) DEFAULT NULL COMMENT '父ID',
  `F_TRACK_NAME` varchar(64) DEFAULT NULL COMMENT '轨道名',
  `F_TRACK_FILENAME` varchar(256) DEFAULT NULL COMMENT '轨道数据文件名',
  `F_TRACK_FILESIZE` bigint(20) DEFAULT '0' COMMENT '轨道数据文件大小',
  `F_TRACK_FILEDATA` text COMMENT '重要：轨道数据文件数据内容',
  `F_COUNT` int(11) DEFAULT '0' COMMENT '顺序号的个数',
  `F_DT` bigint(20) DEFAULT '0' COMMENT 'unix时间的毫秒数',
  `F_DT_STRING` varchar(32) DEFAULT NULL,
  `F_MESSAGE` varchar(255) DEFAULT NULL,
  `F_SYN_FLAG` int(11) DEFAULT '0' COMMENT '同步标志 0 新建 +1第几次修改',
  PRIMARY KEY (`F_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
SELECT * FROM YGCT.T_LIS_BI_TRACK_WEIGHT;

 *
*/
void File2dbTrackWeigh::doFile2db(const CxFileSystem::PathInfo &pathInfo)
{
    string sTrackFileName = fn_getTrackFileName(pathInfo.fileName);

    CxDatabase * oDb = CxDatabase::getDefaultDb();
    if (! ( oDb && oDb->isOpen() ) )
    {
        cxPrompt()<<"File2dbTrackWeigh Fail : DataBase is null || not open!!!";
        return;
    }

    //1 先把读到的文本文件放一份到数据库上
    //2 把文本文件转成 xml 放 bi_mc
    string sFileData = CxEncoding::gb2312ToUtf8( CxString::trim( CxFile::load(pathInfo.filePath()) ) );

    std::vector<std::string> sFileRows = CxString::split(sFileData);

    string sOutXml41108;
    int iWrited41180 = 0;

    int iWritedLog = 0;

    string sMessage = sFileData.size()>0 ? string() : "fail: load file contentsize eq 0;";

    //*2T_CMM_BI_MC
    //string sInsertValues = string.Format("'{0}', null, 'MC', 'CICS系统', 'v4.0', '{1}', 'DB', '{2}', '1', '{3}', 'ICS.MC_M4.0', '{4}', null, '0', null, null, '0'",
    //Guid.NewGuid().ToString(), sMsgNo, sNow, xml, sNow);
    //            _sqlTableInfo = bOk ? "SELECT * FROM %s LIMIT 1;" : "SELECT TOP 1 * FROM %s;";
    int rConvert = FALSE;
    if (sFileData.size()>0)
    {
        rConvert = doConvert2Xml_chengDeChengSheng(sFileRows, pathInfo, sOutXml41108, sMessage);
        if(rConvert==TRUE)
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
            vector<string> sRow41180;;
            sMsgNo = "41108";
            sRow41180.push_back(string(CxString::toString(CxTime::currentMsepoch())));
            sRow41180.push_back(string());
            sRow41180.push_back(string("MC"));
            sRow41180.push_back(string("CICS系统"));
            sRow41180.push_back(string("v4.0"));
            sRow41180.push_back(sMsgNo);
            sRow41180.push_back(string("AMQ")); // AMQ : active mq send; DB : db send;
            sRow41180.push_back(CxTime::currentSepochString());
            sRow41180.push_back(string("1")); // 1：表示向外发；0：表示接收；
            sRow41180.push_back(sOutXml41108);
            sRow41180.push_back(string("ICS.MC_M4.0"));
            sRow41180.push_back(CxTime::currentSepochString());
    //          sRow.push_back(string("null"));
    //          sRow.push_back(string("0"));
    //          sRow.push_back(string("null"));
    //          sRow.push_back(string("null"));
            sRow41180.push_back(string("0"));

            sRows.clear();
            sRows.push_back(sRow41180);
            iWrited41180 = oDb->saveTable(sTableName, sColumnNames, sRows);
            sMessage += "Chehao2T_CMM_BI_MC=" + CxString::toString(iWrited41180);
            cxPrompt()<<"DataBase Insert T_CMM_BI_MC(41180), TrackFileName: " << sTrackFileName << ", result:" << iWrited41180;

            CxFile::save(CxString::format(CxFileSystem::mergeFilePath(f_sIcs2CicsTrackPath, sTrackFileName+"_%d.xml").c_str(), iWrited41180), sOutXml41108);
        }
        else
        {
            cxPrompt()<<"Convert2Xml_chengDeChengSheng Fail, TrackFileName: " << sTrackFileName << ", Message: " << sMessage;
        }
    }
    else
    {
        cxPrompt()<<"Convert2Xml_chengDeChengSheng Fail, TrackFileName: " << sTrackFileName << ", fileData-size: 0";
    }


    //*2T_LIS_BI_TRACK_WEIGHT
    if (rConvert==TRUE)
    {
        int iSync = iWrited41180 > 0 ? 1 : 0;

        string sTableName = "T_LIS_BI_TRACK_WEIGHT";
        vector<string> sColumnNames;
        sColumnNames.push_back(string("F_ID"            ));
        sColumnNames.push_back(string("F_PID"           ));
        sColumnNames.push_back(string("F_TRACK_NAME"    ));
        sColumnNames.push_back(string("F_TRACK_FILENAME"));
        sColumnNames.push_back(string("F_TRACK_FILESIZE"));
        sColumnNames.push_back(string("F_TRACK_FILEDATA"));
        sColumnNames.push_back(string("F_COUNT"         ));
        sColumnNames.push_back(string("F_DT"            ));
        sColumnNames.push_back(string("F_DT_STRING"     ));
        sColumnNames.push_back(string("F_MESSAGE"       ));
        sColumnNames.push_back(string("F_SYN_FLAG"      ));
        vector<vector<string> > sRows;
        vector<string> sRow;
        sRow.push_back(CxString::toString(CxTime::currentMsepoch()));
        sRow.push_back(string());
        sRow.push_back(f_sTrackName);
        sRow.push_back(sTrackFileName);
        sRow.push_back(CxString::toString(pathInfo.fileSize));
        if (sFileData.size()<8192) sRow.push_back(sFileData); else sRow.push_back(string());
        sRow.push_back(CxString::toString(sFileRows.size()));
        sRow.push_back(CxString::toString(pathInfo.lastWriteTime));
        sRow.push_back(CxTime::toString(pathInfo.lastWriteTime));
        sRow.push_back(sMessage);
        sRow.push_back(CxString::toString(iSync));

        sRows.clear();
        sRows.push_back(sRow);
        iWritedLog = oDb->saveTable(sTableName, sColumnNames, sRows);
//        CxFile::save("f:\\0.txt", sFileData);
        sMessage += "Chehao2T_CMM_BI_MC=" + CxString::toString(iWritedLog);
        cxPrompt()<<"DataBase Insert T_LIS_BI_TRACK_WEIGHT, TrackFileName: " << sTrackFileName << ", result:" << iWritedLog;
        if (iWritedLog>0)
        {
            f_scanFileNames[sTrackFileName] = iSync;
            if (pathInfo.lastWriteTime > f_scanlastWriteTime) f_scanlastWriteTime = pathInfo.lastWriteTime;
        }
    }
}

int File2dbTrackWeigh::doConvert2Xml_chengDeChengSheng(const std::vector<std::string> & sFileRows, const CxFileSystem::PathInfo & pathInfo, std::string & sOut41108, std::string & sOutMessage)
{
    int r = TRUE;

    sOutMessage = "sucess;";

    const char * f_s41108XmlHead =
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
    "\r\n<YGCT>"
    "\r\n    <HEAD>"
    "\r\n        <VERSION>1.0</VERSION>"
    "\r\n        <SRC>0500000021</SRC>"
    "\r\n        <DES>0500000003</DES>"
    "\r\n        <MsgNo>41108</MsgNo>"
    "\r\n        <MsgId>543</MsgId>"
    "\r\n        <MsgRef/>"
    "\r\n        <TransDate>%s</TransDate>"
    "\r\n        <Reserve/>"
    "\r\n    </HEAD>"
    "\r\n    <MSG>"
    "\r\n        <Gdhjl41108>"
            ;


    const char * f_s41108XmlBody =
    "\r\n<Gdhjl41108List>"
    "\r\n    <ID>%s</ID>"
    "\r\n    <CHEHAO>%s</CHEHAO>"
    "\r\n    <SHEBEI>%s</SHEBEI>"
    "\r\n    <GUIDAO>%s</GUIDAO>"
    "\r\n    <SXH>%d</SXH>"
    "\r\n    <ZL>%s</ZL>"
    "\r\n    <GHSJ>%s</GHSJ>"
    "\r\n    <GHLX>00000001</GHLX>"
    "\r\n    <CHEXING>%s</CHEXING>"
    "\r\n</Gdhjl41108List>"
            ;


    const char * f_s41108XmlEnd =
    "\r\n        </Gdhjl41108>"
    "\r\n    </MSG>"
    "\r\n</YGCT>"
            ;
    string sTransDate = CxTime::toStringSepoch(CxTime::currentSepoch(), 0, 0, 0);
    string sID = CxTime::toString(CxTime::currentMsepoch(), '\0', '\0', '\0') + "%03d"; //*唯一ID
    string sCHEHAO = ""; //*车号
    string sGUIDAO = f_sTrackName; //轨道
    int iSXH; //*顺序号
    string sZL; //*重量
    double dZL;
    string sGHSJ = CxFileSystem::extractFilePrefixName(pathInfo.fileName); string sCurYear = sTransDate.substr(0, 4); // 过衡时间
    if (sGHSJ.size() == 8) //旧版文件名 07102132
    {
        sGHSJ += ":00"; sGHSJ.insert(6,  ":"); sGHSJ.insert(4,  " "); sGHSJ.insert(2,  "-"); sGHSJ.insert(0, sCurYear+"-");
    }
    else if (sGHSJ.size() == 12) //新版文件名 201605311839
    {
        sGHSJ += ":00"; sGHSJ.insert(10,  ":"); sGHSJ.insert(8,  " "); sGHSJ.insert(6,  "-"); sGHSJ.insert(4, "-");
    }
    else
    {
        sGHSJ = CxTime::toStringSepoch(sepoch_t(pathInfo.lastWriteTime / 1000), '-', ' ', ':'); // 过衡时间
    }
    string sCHEXING = f_sCheXing;
//    CxString::format("%06d",i)

    for (size_t i = 0; i < sFileRows.size(); ++i)
    {

//新的
//"   1","abcdefgh12345",88950,2.3,2
//"   2","abcdefgh12345",88080,2.6,2

//旧的
//"88064",5.7,"双阳矿",62,85.868,22.680,63.188,0.000,1.188," ","07","10","10:19","1","1"
        const string & sFileRow = CxString::trim(sFileRows.at(i));
        if (sFileRow.empty()) continue;
        vector<string> sRow = CxString::split(sFileRow, ',', true);
        if (sRow.size()<3)
        {
            sOutMessage = "fail: item count invalid!;";
            r = false;
            break;
        }

        sID = CxString::format(sID.c_str(), i+1);

        if (sRow.size() > 9)
        {
            sCHEHAO = CxString::trim( CxString::trim( sRow.at(0) ), '\"' );
            iSXH = i+1;
            sZL = CxString::trim( CxString::trim( sRow.at(4) ), '\"' );
            //旧版车号为空不传数据
            if (sCHEHAO.empty())
            {
                sOutMessage = "fail: item value[CheHao] invalid!;";
                r = -2;
                break;
            }
        }
        else
        {
            sCHEHAO = CxString::trim( CxString::trim( sRow.at(1) ), '\"' );
            iSXH = CxString::toInt32( CxString::trim( CxString::trim( sRow.at(0) ), '\"' ) );
            sZL = CxString::trim( CxString::trim( sRow.at(2) ), '\"' );
            dZL = CxString::toDouble(sZL); if (dZL>0) sZL = fn_getMeasureValue(dZL / 1000);
        }

        if (iSXH < 0 || iSXH > 100)
        {
            sOutMessage = "fail: item value[ShunXuHao] invalid!;";
            r = -1;
            break;
        }

        sOut41108 += CxString::format(f_s41108XmlBody, sID.c_str(), sCHEHAO.c_str(), sGUIDAO.c_str(), sGUIDAO.c_str(), iSXH, sZL.c_str(), sGHSJ.c_str(), sCHEXING.c_str());
    }

    sOut41108 = CxString::format( f_s41108XmlHead , sTransDate.c_str()) + sOut41108 + f_s41108XmlEnd;

    return r;
}

