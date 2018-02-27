/*
 * 数据表结构
 *

CREATE TABLE `T_LIS_BI_TRACK_PICTURE_MAIN` (
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
  `F_F0` varchar(64) DEFAULT NULL,
  `F_F1` varchar(64) DEFAULT NULL,
  `F_F2` varchar(64) DEFAULT NULL,
  `F_F3` varchar(64) DEFAULT NULL,
  `F_F4` varchar(64) DEFAULT NULL,
  `F_F5` varchar(64) DEFAULT NULL,
  `F_F6` varchar(64) DEFAULT NULL,
  `F_F7` varchar(64) DEFAULT NULL,
  `F_F8` varchar(64) DEFAULT NULL,
  `F_F9` varchar(64) DEFAULT NULL,
  `F_SYN_FLAG` int(11) DEFAULT '0' COMMENT '同步标志 0 新建 +1第几次修改',
  PRIMARY KEY (`F_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8

CREATE TABLE `T_LIS_BI_TRACK_PICTURE_DETAIL` (
  `F_ID` varchar(36) NOT NULL COMMENT '主键',
  `F_PID` varchar(36) DEFAULT NULL COMMENT '父ID',
  `F_TRACK_FILENAME` varchar(256) DEFAULT NULL COMMENT '轨道数据文件名',
  `F_XUHAO` int(11) DEFAULT NULL COMMENT '序号',
  `F_CHEHAO` varchar(64) DEFAULT NULL COMMENT '车号',
  `F_PICTURE_PATH1` varchar(256) DEFAULT NULL COMMENT '摄像头1车身照片URL存储路径',
  `F_PICTURE_PATH2` varchar(256) DEFAULT NULL COMMENT '摄像头2车身照片URL存储路径',
  `F_PICTURE_PATH3` varchar(256) DEFAULT NULL COMMENT '摄像头3车身照片URL存储路径',
  `F_DT` bigint(20) DEFAULT '0' COMMENT 'unix时间的毫秒数,存储当前数据时的时间',
  `F_DT_STRING` varchar(32) DEFAULT NULL COMMENT 'unix时间的毫秒数,存储当前数据时的时间',
  `F_MESSAGE` varchar(255) DEFAULT NULL,
  `F_F0` varchar(64) DEFAULT NULL,
  `F_F1` varchar(64) DEFAULT NULL,
  `F_F2` varchar(64) DEFAULT NULL,
  `F_F3` varchar(64) DEFAULT NULL,
  `F_F4` varchar(64) DEFAULT NULL,
  `F_F5` varchar(64) DEFAULT NULL,
  `F_F6` varchar(64) DEFAULT NULL,
  `F_F7` varchar(64) DEFAULT NULL,
  `F_F8` varchar(64) DEFAULT NULL,
  `F_F9` varchar(64) DEFAULT NULL,
  `F_SYN_FLAG` int(11) DEFAULT '0' COMMENT '同步标志 0 新建 +1第几次修改',
  PRIMARY KEY (`F_ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8

 *
 */


#include "file2db_track_picture.h"


using namespace std;


static File2dbTrackPicture * fn_oSlsServer()
{
    CxApplication::registStartFunction(File2dbTrackPicture::start, 20001);
    CxApplication::registStopFunction(File2dbTrackPicture::stop);
    GeneralAccess::registerMessageCommandDeal(File2dbTrackPicture::dealMessage);
    GeneralAccess::registerFileWriteDeal(File2dbTrackPicture::dealFileWrite);
    static File2dbTrackPicture m;
    return &m;
}

static File2dbTrackPicture * m_pTicketSvr = fn_oSlsServer();


static string CS_SectionTakePictrue = "TakePictrue";
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
        iScanMode = CxApplication::findConfig(CS_SectionTakePictrue, "ScanMode", int(ci_track_weight_scan_mode_filename));
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

void File2dbTrackPicture::start()
{
    f_sIcs2CicsTrackPath = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), "temp");
    f_sIcs2CicsTrackPath = CxFileSystem::mergeFilePath(f_sIcs2CicsTrackPath, "ics2cics_track");
    if (! CxFileSystem::isExist(f_sIcs2CicsTrackPath))
    {
        CxFileSystem::createDirMultiLevel(f_sIcs2CicsTrackPath);
    }

    f_sScanPath = CxApplication::findConfig(CS_SectionTakePictrue, string("ScanPath"), std::string());
    if (f_sScanPath.empty())
    {
        cxPrompt() << "Track Pictrue's ScanPath is empty!!! Do not scan.";
        cxPrompt() << "Track Pictrue's ScanPath is empty!!! Do not scan.";
        cxPrompt() << "Track Pictrue's ScanPath is empty!!! Do not scan.";
        return;
    }
    if ( ! CxFileSystem::hasRootPath(f_sScanPath) )
    {
        f_sScanPath = CxFileSystem::cd( CxApplication::applicationDeployPath() , f_sScanPath );
    }

    f_iScanInterval = CxApplication::findConfig(CS_SectionTakePictrue, string("ScanInterval"), 3000);
    if (f_iScanInterval < 3000) f_iScanInterval = 3000;

    f_sTrackName = CxApplication::findConfig(CS_SectionTakePictrue, string("TrackName"), std::string("1"));

    f_sCheXing = CxApplication::findConfig(CS_SectionTakePictrue, string("CheXing"), std::string("C62"));

    CxTimerManager::startTimer(File2dbTrackPicture::timerDo, f_iScanInterval);

    cxPrompt() << "TrackName: " << f_sTrackName;
    cxPrompt() << "ScanPath: " << f_sScanPath;
    cxPrompt() << "ScanInterval: " << f_iScanInterval;
}

void File2dbTrackPicture::stop()
{
    CxTimerManager::stopTimer(File2dbTrackPicture::timerDo, f_iScanInterval);
}

int File2dbTrackPicture::dealMessage(const PsmExplainer *oPsm, const string &sMessageCommand, const string &sMessageParam, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    if(sMessageCommand.find(".file2db.track_weigh") != string::npos)
    {
        return TRUE;
    }
    return FALSE;
}

int File2dbTrackPicture::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    if (fileData.empty())
        return FALSE;
    return FALSE;
}

void File2dbTrackPicture::timerDo(int iInterval)
{
    static int iTimes = 0;
    static int iHeartJumpErrorTimes = 0;
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
    //数据库心跳错误就关闭数据库
    if (iHeartJumpErrorTimes>3)
    {
        oDb->closeDatabase();
        iHeartJumpErrorTimes = 0;
        outPrompt() << "Error ! Error ! Error ! Db HeartJumpErrorTimes>3 Close DataBase. ";
        return;
    }
    else if (! oDb->isOpen())
    {
        oDb->openDatabase();
        outPrompt() << "DB Open DataBase. ";
    }

    if (! oDb->isOpen())
    {
        outPrompt() << "Error ! Error ! Error ! Db is not open. ";
        return;
    }

    //
    if (iScanMOde == ci_track_weight_scan_mode_filename)
    {
        if (f_scanFileNames.empty() && oDb->isOpen())
        {
            std::vector<std::map<std::string, std::string> > fileNameSyncs = oDb->queryToMapVector("select F_TRACK_FILENAME as 'FILENAME' , F_SYN_FLAG as 'SYNC' from T_LIS_BI_TRACK_PICTURE_MAIN;");
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
            string sScanlastWriteTime = oDb->queryValue("select max(F_DT) as 'ScanlastWriteTime' from T_LIS_BI_TRACK_PICTURE_MAIN;");
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
            if (iResult < 0) iHeartJumpErrorTimes++;
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
            if (! CxString::equalCase( CxFileSystem::extractFileSuffixName(pathInfo.fileName) , ".csv") ) continue;
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

void File2dbTrackPicture::doFile2db(const CxFileSystem::PathInfo &pathInfo)
{
    string sTrackFileName = fn_getTrackFileName(pathInfo.fileName);
    string sTrackFilePrefixName = CxFileSystem::extractFilePrefixName(sTrackFileName);

    CxDatabase * oDb = CxDatabase::getDefaultDb();
    if (! ( oDb && oDb->isOpen() ) )
    {
        cxPrompt()<<"doFile2db Fail : DataBase is null || not open!!!";
        return;
    }

    //1 先把读到的文本文件放一份到数据库上
    //2 把文本文件转成 xml 放 bi_mc
    string sFileData = CxEncoding::gb2312ToUtf8( CxString::trim( CxFile::load(pathInfo.filePath()) ) );

    std::vector<std::string> sFileRows = CxString::split(sFileData);

    string sMessage = sFileData.size()>0 ? string() : "fail: load file contentsize eq 0;";

    string sId = sTrackFilePrefixName.size()<19 ? sTrackFilePrefixName + "-" + CxString::toString(CxTime::currentMsepoch()) : CxString::toString(CxTime::currentMsepoch());
    int rDetailCount = 0;
    if (sFileData.size()>0)
    {
        rDetailCount = doFileData2Db(sFileRows, pathInfo, sId, sMessage);
    }
    else
    {
        cxPrompt()<<"doFile2db fileData size==0;";
    }

    string sFileValidData;
    if (rDetailCount>0)
    {
        sFileRows.resize(rDetailCount+3);
        sFileValidData = CxString::join(sFileRows);
    }
    else
    {
        sFileValidData = sFileData;
    }
    if (sFileValidData.size()>16384) sFileValidData = string();

    string sLine1 = sFileRows.size()>0 ? sFileRows.at(0) : string();
    vector<string> sFields = CxString::split(sLine1, ',');
    string sF0 = sFields.size()>0 ? sFields.at(0) : string();
    string sF1 = sFields.size()>1 ? sFields.at(1) : string();
    string sF2 = sFields.size()>2 ? sFields.at(2) : string();
    string sF3 = sFields.size()>3 ? sFields.at(3) : string();
    string sF4 = sFields.size()>4 ? sFields.at(4) : string();
    string sF5 = sFields.size()>5 ? sFields.at(5) : string();
    string sF6 = sFields.size()>6 ? sFields.at(6) : string();
    string sF7 = sFields.size()>7 ? sFields.at(7) : string();
    string sF8 = sFields.size()>8 ? sFields.at(8) : string();
    string sF9 = sFields.size()>9 ? sFields.at(9) : string();

    //*2T_LIS_BI_TRACK_PICTURE_MAIN
    string sTableName = "T_LIS_BI_TRACK_PICTURE_MAIN";
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
    sColumnNames.push_back(string("F_F0"       ));
    sColumnNames.push_back(string("F_F1"       ));
    sColumnNames.push_back(string("F_F2"       ));
    sColumnNames.push_back(string("F_F3"       ));
    sColumnNames.push_back(string("F_F4"       ));
    sColumnNames.push_back(string("F_F5"       ));
    sColumnNames.push_back(string("F_F6"       ));
    sColumnNames.push_back(string("F_F7"       ));
    sColumnNames.push_back(string("F_F8"       ));
    sColumnNames.push_back(string("F_F9"       ));
    sColumnNames.push_back(string("F_SYN_FLAG"      ));
    vector<vector<string> > sRows;
    vector<string> sRow;
    sRow.push_back(sId);
    sRow.push_back(string());
    sRow.push_back(f_sTrackName);
    sRow.push_back(sTrackFileName);
    sRow.push_back(CxString::toString(pathInfo.fileSize));
    sRow.push_back(sFileValidData);
    sRow.push_back(CxString::toString(rDetailCount));
    sRow.push_back(CxString::toString(pathInfo.lastWriteTime));
    sRow.push_back(CxTime::toString(pathInfo.lastWriteTime));
    sRow.push_back(sMessage);
    sRow.push_back(sF0);
    sRow.push_back(sF1);
    sRow.push_back(sF2);
    sRow.push_back(sF3);
    sRow.push_back(sF4);
    sRow.push_back(sF5);
    sRow.push_back(sF6);
    sRow.push_back(sF7);
    sRow.push_back(sF8);
    sRow.push_back(sF9);
    sRow.push_back(CxString::toString(int(0)));

    sRows.clear();
    sRows.push_back(sRow);
    int iWritedLog = oDb->saveTable(sTableName, sColumnNames, sRows);
//        CxFile::save("f:\\0.txt", sFileData);
    cxPrompt()<<"DataBase Insert T_LIS_BI_TRACK_PICTURE_MAIN result=" << iWritedLog;
    if (iWritedLog>0)
    {
        f_scanFileNames[sTrackFileName] = iWritedLog;
        if (pathInfo.lastWriteTime > f_scanlastWriteTime) f_scanlastWriteTime = pathInfo.lastWriteTime;
    }
}

int File2dbTrackPicture::doFileData2Db(const std::vector<std::string> & sFileRows, const CxFileSystem::PathInfo & pathInfo, const string &sPid, std::string & sOutMessage)
{
    CxDatabase * oDb = CxDatabase::getDefaultDb();
    if (! ( oDb && oDb->isOpen() ) )
    {
        cxPrompt()<<"doFileData2Db Fail : DataBase is null || not open!!!";
        return 0;
    }

    int r = 0;

//    CxString::format("%06d",i)

    for (size_t i = 1; i < sFileRows.size(); ++i)
    {
        const string & sFileRow = CxString::trim(sFileRows.at(i));
        if (sFileRow.empty()) continue;
        vector<string> sFields = CxString::split(sFileRow, ',');
        string sID             = sPid + "-" + CxString::toString(i);
        string sPID            = sPid;
        string sTRACK_FILENAME = fn_getTrackFileName(pathInfo.fileName);
        string sXUHAO          = sFields.size()>0 ? sFields.at(0) : string();
        string sCHEHAO         = sFields.size()>1 ? sFields.at(1) : string();
        string sPICTURE_PATH1  = sFields.size()>2 ? sFields.at(2) : string();
        string sPICTURE_PATH2  = sFields.size()>3 ? sFields.at(3) : string();
        string sPICTURE_PATH3  = sFields.size()>4 ? sFields.at(4) : string();
        string sDT             = "";
        string sDT_STRING      = sFields.size()>5 ? sFields.at(5) : string();
        string sMESSAGE        = "";
        string sF0             = sFields.size()>6 ? sFields.at(6) : string();
        string sF1             = sFields.size()>7 ? sFields.at(7) : string();
        string sF2             = sFields.size()>8 ? sFields.at(8) : string();
        string sF3             = sFields.size()>9 ? sFields.at(9) : string();
        string sF4             = sFields.size()>10 ? sFields.at(10) : string();
        string sF5             = sFields.size()>11 ? sFields.at(11) : string();
        string sF6             = sFields.size()>12 ? sFields.at(12) : string();
        string sF7             = sFields.size()>13 ? sFields.at(13) : string();
        string sF8             = sFields.size()>14 ? sFields.at(14) : string();
        string sF9             = sFields.size()>15 ? sFields.at(15) : string();
        string sSYN_FLAG       = "0";

        sXUHAO = CxString::trim(sXUHAO);
        sCHEHAO = CxString::trim(sCHEHAO);
        if (sXUHAO.empty() || sCHEHAO.empty())
        {
            continue;
        }

        sDT_STRING = sDT_STRING.size() == 8  ? CxTime::toStringDay(sepoch_t(pathInfo.lastWriteTime / 1000)) + " " + sDT_STRING : sDT_STRING;
        sepoch_t dtDT;
        if (CxTime::fromStringSepoch(sDT_STRING, dtDT))
        {
            msepoch_t dtDT2 = dtDT * 1000;
            sDT = CxString::toString(dtDT2);
        }

        string sTableName = "T_LIS_BI_TRACK_PICTURE_DETAIL";
        vector<string> sColumnNames;
        sColumnNames.push_back(string("F_ID"));
        sColumnNames.push_back(string("F_PID"));
        sColumnNames.push_back(string("F_TRACK_FILENAME"));
        sColumnNames.push_back(string("F_XUHAO"));
        sColumnNames.push_back(string("F_CHEHAO"));
        sColumnNames.push_back(string("F_PICTURE_PATH1"));
        sColumnNames.push_back(string("F_PICTURE_PATH2"));
        sColumnNames.push_back(string("F_PICTURE_PATH3"));
        sColumnNames.push_back(string("F_DT"));
        sColumnNames.push_back(string("F_DT_STRING"));
        sColumnNames.push_back(string("F_MESSAGE"));
        sColumnNames.push_back(string("F_F0"));
        sColumnNames.push_back(string("F_F1"));
        sColumnNames.push_back(string("F_F2"));
        sColumnNames.push_back(string("F_F3"));
        sColumnNames.push_back(string("F_F4"));
        sColumnNames.push_back(string("F_F5"));
        sColumnNames.push_back(string("F_F6"));
        sColumnNames.push_back(string("F_F7"));
        sColumnNames.push_back(string("F_F8"));
        sColumnNames.push_back(string("F_F9"));
        sColumnNames.push_back(string("F_SYN_FLAG"));
        vector<vector<string> > sRows;
        vector<string> sRow;
        sRow.push_back(sID            );
        sRow.push_back(sPID           );
        sRow.push_back(sTRACK_FILENAME);
        sRow.push_back(sXUHAO         );
        sRow.push_back(sCHEHAO        );
        sRow.push_back(sPICTURE_PATH1 );
        sRow.push_back(sPICTURE_PATH2 );
        sRow.push_back(sPICTURE_PATH3 );
        sRow.push_back(sDT            );
        sRow.push_back(sDT_STRING     );
        sRow.push_back(sMESSAGE       );
        sRow.push_back(sF0            );
        sRow.push_back(sF1            );
        sRow.push_back(sF2            );
        sRow.push_back(sF3            );
        sRow.push_back(sF4            );
        sRow.push_back(sF5            );
        sRow.push_back(sF6            );
        sRow.push_back(sF7            );
        sRow.push_back(sF8            );
        sRow.push_back(sF9            );
        sRow.push_back(sSYN_FLAG      );

        sRows.clear();
        sRows.push_back(sRow);
        int iSaveTable = oDb->saveTable(sTableName, sColumnNames, sRows);
        sOutMessage += CxString::format(" r%d:%d,", i, iSaveTable);
        if (iSaveTable>0) r++;
    //        CxFile::save("f:\\0.txt", sFileData);
        cxPrompt() << "DataBase Insert T_LIS_BI_TRACK_PICTURE_DETAIL [F_ID,RESULT]: " << sID << "," << iSaveTable;
    }

    return r;
}

int File2dbTrackPicture::doFileData2Mc(const string &sXmlBody, const CxFileSystem::PathInfo &pathInfo, const string &sPid, string &sOutMessage)
{
    CxDatabase * oDb = CxDatabase::getDefaultDb();
    if (! ( oDb && oDb->isOpen() ) )
    {
        cxPrompt()<<"doFileData2Mc Fail : DataBase is null || not open!!!";
        return 0;
    }

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

//    sOut41108 += CxString::format(f_s41108XmlBody, sID.c_str(), sCHEHAO.c_str(), sGUIDAO.c_str(), sGUIDAO.c_str(), iSXH, sZL.c_str(), sGHSJ.c_str(), sCHEXING.c_str());

    const char * f_s41108XmlEnd =
    "\r\n        </Gdhjl41108>"
    "\r\n    </MSG>"
    "\r\n</YGCT>"
            ;

    string sTransDate = CxTime::toStringSepoch(CxTime::currentSepoch(), 0, 0, 0);
    string sOutXml41108 = CxString::format( f_s41108XmlHead , sTransDate.c_str()) + sXmlBody + f_s41108XmlEnd;

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
    sRow41180.push_back(sPid);
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
    int iWrited41180 = oDb->saveTable(sTableName, sColumnNames, sRows);
    cxPrompt()<<"DataBase Insert T_CMM_BI_MC(41180) result=" << iWrited41180;

    sOutMessage += CxString::format(" rMC:%d,", iWrited41180);

    CxFile::save(CxString::format(CxFileSystem::mergeFilePath(f_sIcs2CicsTrackPath, pathInfo.fileName+"_%d.xml").c_str(), iWrited41180), sOutXml41108);

    int r = iWrited41180 > 0 ? 1 : 0;

    return r;
}

