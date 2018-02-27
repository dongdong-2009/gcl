#include "gcl_config.h"


#include <ccxx/cxapplication.h>
#include <ccxx/cxfile.h>
#include <ccxx/cxcontainer.h>

#include "gcl_resource.h"


using namespace std;

static bool f_bGclConfigLoad = false;

GclConfig * fn_oGclConfig()
{
    CxApplication::registInitFunction(GclConfig::start);
    CxApplication::registStopFunction(GclConfig::stop);
    static GclConfig m;
    return & m;
}

static GclConfig * f_oGclConfig = fn_oGclConfig();



static std::string f_sApplicationPath   ;
static std::string f_sDatabasePath      ;
static std::string f_sMeasurePath       ;
static std::string f_sMeasureCommonPath ;
static std::string f_sTempPath          ;
static std::string f_sLogPath           ;
static std::string f_sLogCommonPath     ;
static std::string f_sReportPath        ;
static std::string f_sTerminalPath      ;
static std::string f_sDescribePath      ;
static std::string f_sBusinessPath      ;
static std::string f_sMonsbMeasurePath  ;
static std::string f_sYcaddMeasurePath  ;
static std::string f_sStrawMeasurePath  ;
static std::string f_sCalcMeasurePath  ;
static std::string f_sMonsbOperatesPath ;
static std::string f_sScriptPath        ;
static std::string f_sScriptCommonPath  ;
static std::string f_sUiPath            ;
static std::string f_sDataPath          ;
static std::string f_sDataCommonPath    ;
static std::string f_sModelPath         ;
static std::string f_sModelCommonPath   ;
static std::string f_sReportDefaultFilePath;
static std::string f_sConfigPath;


static std::string f_sMqIp              = "127.0.0.1";

static std::string f_sDSN_ICS           = "orcl";      //数据库
static std::string f_sUser_ICS          = "gcl";
static std::string f_sPassWord_ICS      = "gcl";

static int         f_nDBtype            = 1;           //数据库类型
static uint64      f_lShareMemSize              = 0;   //共享内存大小

static bool f_bIsAutoStart              = false   ;    //
static bool f_bIsTesting                = false   ;    //是否写日志
static bool f_bIsWriteLog               = false   ;
static bool f_bIsStartWatchdog          = false   ;
static bool f_bIsLoadMonsbOperate       = false   ;

static std::string f_defaultDbName;

static bool f_bIsChannelState           = false;
static int  f_nChannelBaseAddr          = 0;
static int  f_nMqPort                   = 5559;
static bool f_bIsByteInverse            = true;


const int CGChannelBase = 0x1000000;


void GclConfig::start()
{  
    load();
}

void GclConfig::stop()
{
}

void GclConfig::load()
{
    if (f_bGclConfigLoad)
        return;

    f_sApplicationPath   = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryApplicationPath  , deployPath       ()) );
    f_sDatabasePath      = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryDatabasePath     , databasePath     ()) );
    f_sMeasurePath       = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryMeasurePath      , measurePath      ()) );
    f_sTempPath          = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryTempPath         , tempPath         ()) );
    f_sLogPath           = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryLogPath          , logPath          ()) );
    f_sReportPath        = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryReportPath          , reportPath       ()) );
    f_sTerminalPath      = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryTerminalPath     , terminalPath     ()) );
    f_sDescribePath      = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryDescribePath     , describePath     ()) );
    f_sBusinessPath      = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryBusinessPath     , businessPath     ()) );
    f_sMonsbMeasurePath  = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryMonsbMeasurePath , monsbMeasurePath ()) );
    f_sYcaddMeasurePath  = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryYcaddMeasurePath , ycaddMeasurePath ()) );
    f_sStrawMeasurePath  = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryStrawMeasurePath , strawMeasurePath ()) );
    f_sMonsbOperatesPath = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryMonsbOperatesPath, monsbOperatesPath()) );
    f_sScriptPath        = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryScriptPath       , scriptPath       ()) );
    f_sUiPath            = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryUiPath           , uiPath           ()) );
    f_sDataPath       = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryDataPath      , dataPath      ()) );
    f_sModelPath       = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryModelPath      , modelPath      ()) );
    f_sConfigPath       = CxFileSystem::fullFilePath( CxApplication::applicationDeployPath(), CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryConfigPath      , configPath      ()) );


    if (! CxFileSystem::isExist(f_sApplicationPath)) CxFileSystem::createDirMultiLevel(f_sApplicationPath);
    if (! CxFileSystem::isExist(f_sDatabasePath)) CxFileSystem::createDirMultiLevel(f_sDatabasePath);
    if (! CxFileSystem::isExist(f_sMeasurePath)) CxFileSystem::createDirMultiLevel(f_sMeasurePath);
    if (! CxFileSystem::isExist(f_sTempPath)) CxFileSystem::createDirMultiLevel(f_sTempPath);
    if (! CxFileSystem::isExist(f_sLogPath)) CxFileSystem::createDirMultiLevel(f_sLogPath);
    if (! CxFileSystem::isExist(f_sReportPath)) CxFileSystem::createDirMultiLevel(f_sReportPath);
    if (! CxFileSystem::isExist(f_sTerminalPath)) CxFileSystem::createDirMultiLevel(f_sTerminalPath);
    if (! CxFileSystem::isExist(f_sDescribePath)) CxFileSystem::createDirMultiLevel(f_sDescribePath);
    if (! CxFileSystem::isExist(f_sBusinessPath)) CxFileSystem::createDirMultiLevel(f_sBusinessPath);
    if (! CxFileSystem::isExist(f_sScriptPath)) CxFileSystem::createDirMultiLevel(f_sScriptPath);
    if (! CxFileSystem::isExist(f_sUiPath)) CxFileSystem::createDirMultiLevel(f_sUiPath);
    if (! CxFileSystem::isExist(f_sDataPath)) CxFileSystem::createDirMultiLevel(f_sDataPath);
    if (! CxFileSystem::isExist(f_sModelPath)) CxFileSystem::createDirMultiLevel(f_sModelPath);
    if (! CxFileSystem::isExist(f_sConfigPath)) CxFileSystem::createDirMultiLevel(f_sConfigPath);

    f_sMeasureCommonPath = CxFileSystem::parentPath(f_sMeasurePath);
    f_sLogCommonPath     = CxFileSystem::parentPath(f_sLogPath);
    f_sScriptCommonPath  = CxFileSystem::parentPath(f_sScriptPath);
    f_sDataCommonPath  = CxFileSystem::parentPath(f_sDataPath);
    f_sModelCommonPath  = CxFileSystem::parentPath(f_sModelPath);

    f_sMqIp              = CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryMQIP, f_sMqIp);
    f_sDSN_ICS           = CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryDSN_ICS, f_sDSN_ICS);
    f_sUser_ICS          = CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryUSER_ICS, f_sUser_ICS);
    f_sPassWord_ICS      = CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryPW_ICS, f_sPassWord_ICS);

    f_defaultDbName      = CxApplication::findConfig(CS_SectionProgramConfig, CS_EntryDefaultDbName, defaultDbName(), false);

    f_bIsAutoStart        = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryIsAutoStart       , f_bIsAutoStart       );
    f_bIsTesting          = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryIsTesting         , f_bIsTesting         );
    f_bIsWriteLog         = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryIsWriteLog        , f_bIsWriteLog        );
    f_bIsStartWatchdog    = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryIsStartWatchdog   , f_bIsStartWatchdog   );
    f_bIsLoadMonsbOperate = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryIsLoadMonsbOperate, f_bIsLoadMonsbOperate);

    f_bIsChannelState     = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryChannelState   ,    f_bIsChannelState );
    f_nChannelBaseAddr    = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryChannelBaseAddr,    f_nChannelBaseAddr);
    f_nMqPort             = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryMQPort         ,    f_nMqPort         );
    f_bIsByteInverse      = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryByteInverse    ,    f_bIsByteInverse  );

    f_nDBtype             = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryDBType_ICS    ,     f_nDBtype  );
    f_lShareMemSize       = CxApplication::findConfig(CS_SectionProgramConfig,  CS_EntryShareMemSize    ,   f_lShareMemSize  );

    CxLogManager::setLogFileTactics(f_oGclConfig);

    f_bGclConfigLoad = true;
}


const string & GclConfig::deployPath()
{
    if (f_sApplicationPath.empty())
    {
        f_sApplicationPath =  CxApplication::applicationDeployPath();
    }
    return f_sApplicationPath;
}
const string &GclConfig::configPath()
{
    if (f_sConfigPath.empty())
    {
        f_sConfigPath = CxFileSystem::mergeFilePath( deployPath(), CS_DirNameConfig );
    }
    return f_sConfigPath;
}
const string &GclConfig::databasePath()
{
    if (f_sDatabasePath.empty())
    {
        f_sDatabasePath = CxFileSystem::mergeFilePath( deployPath(), CS_DirNameDatabase );
    }
    return f_sDatabasePath;
}

const string &GclConfig::measurePath()
{
    if (f_sMeasurePath.empty())
    {
        f_sMeasurePath = CxFileSystem::mergeFilePath( CxFileSystem::mergeFilePath( deployPath(), CS_DirNameMeasure ), CxApplication::applicationTargetName() );
    }
    return f_sMeasurePath;
}

const string &GclConfig::measureCommonPath()
{
    if (f_sMeasureCommonPath.empty())
    {
        f_sMeasureCommonPath = measurePath();
        f_sMeasureCommonPath = CxFileSystem::parentPath(f_sMeasureCommonPath);
    }
    return f_sMeasureCommonPath;
}

const string &GclConfig::dataPath()
{
    if (f_sDataPath.empty())
    {
        f_sDataPath = CxFileSystem::mergeFilePath( CxFileSystem::mergeFilePath( deployPath(), CS_DirNameData ), CxApplication::applicationTargetName() );
    }
    return f_sDataPath;
}

const string &GclConfig::dataCommonPath()
{
    if (f_sDataCommonPath.empty())
    {
        f_sDataCommonPath = dataPath();
        f_sDataCommonPath = CxFileSystem::parentPath(f_sDataCommonPath);
    }
    return f_sDataCommonPath;
}

const string &GclConfig::modelPath()
{
    if (f_sModelPath.empty())
    {
        f_sModelPath = CxFileSystem::mergeFilePath( CxFileSystem::mergeFilePath( deployPath(), CS_DirNameModel ), CxApplication::applicationTargetName() );
    }
    return f_sModelPath;
}

const string &GclConfig::modelCommonPath()
{
    if (f_sModelCommonPath.empty())
    {
        f_sModelCommonPath = modelPath();
        f_sModelCommonPath = CxFileSystem::parentPath(f_sModelCommonPath);
    }
    return f_sModelCommonPath;
}

const string &GclConfig::tempPath()
{
    if (f_sTempPath.empty())
    {
        f_sTempPath = CxFileSystem::mergeFilePath( deployPath(), CS_DirNameTemp );
    }
    return f_sTempPath;
}


const string &GclConfig::logPath()
{
    if (f_sLogPath.empty())
    {
        f_sLogPath = CxFileSystem::mergeFilePath( CxFileSystem::mergeFilePath( deployPath(), CS_DirNameLog ), CxApplication::applicationTargetName() );
    }
    return f_sLogPath;
}

const string &GclConfig::logCommonPath()
{
    if (f_sLogCommonPath.empty())
    {
        f_sLogCommonPath = logPath();
        f_sLogCommonPath = CxFileSystem::parentPath(f_sLogCommonPath);
    }
    return f_sLogCommonPath;
}


const string &GclConfig::reportPath()
{
    if (f_sReportPath.empty())
    {
        f_sReportPath = CxFileSystem::mergeFilePath( deployPath(), CS_DirNameReport );
    }
    return f_sReportPath;
}

const string &GclConfig::reportDefaultFilePath()
{
    if (f_sReportDefaultFilePath.empty())
    {
        f_sReportDefaultFilePath = CxFileSystem::mergeFilePath( reportPath() , CxApplication::applicationTargetName()+".report.txt" );
    }
    return f_sReportDefaultFilePath;
}

const string &GclConfig::terminalPath()
{
    if (f_sTerminalPath.empty())
    {
        f_sTerminalPath = CxFileSystem::mergeFilePath( deployPath(), CS_DirNameTerminal );
    }
    return f_sTerminalPath;
}

const string &GclConfig::businessPath()
{
    if (f_sBusinessPath.empty())
    {
        f_sBusinessPath = CxFileSystem::mergeFilePath( deployPath(), CS_DirNameBusiness );
    }
    return f_sBusinessPath;
}

const string &GclConfig::describePath()
{
    if (f_sDescribePath.empty())
    {
        f_sDescribePath = CxFileSystem::mergeFilePath( deployPath(), CS_DirNameDescribe );
    }
    return f_sDescribePath;
}


const string &GclConfig::monsbMeasurePath()
{
    if (f_sMonsbMeasurePath.empty())
    {
        f_sMonsbMeasurePath = CxFileSystem::mergeFilePath( measurePath(),   CS_FileNameMonsbMeasure );
        if(!CxFileSystem::isExist(f_sMonsbMeasurePath))
        {
            f_sMonsbMeasurePath = CxFileSystem::mergeFilePath( measureCommonPath(),   CS_FileNameMonsbMeasure );
        }
    }
    return f_sMonsbMeasurePath;
}


const string &GclConfig::ycaddMeasurePath()
{
    if (f_sYcaddMeasurePath.empty())
    {
        f_sYcaddMeasurePath = CxFileSystem::mergeFilePath( measurePath(),   CS_FileNameYcaddMeasure  );
        if(!CxFileSystem::isExist(f_sYcaddMeasurePath))
        {
            f_sYcaddMeasurePath = CxFileSystem::mergeFilePath( measureCommonPath(),   CS_FileNameYcaddMeasure );
        }
    }
    return f_sYcaddMeasurePath;
}

const string &GclConfig::strawMeasurePath()
{
    if (f_sStrawMeasurePath.empty())
    {
        f_sStrawMeasurePath = CxFileSystem::mergeFilePath( measurePath(),   CS_FileNameStrawMeasure  );
        if(!CxFileSystem::isExist(f_sStrawMeasurePath))
        {
            f_sStrawMeasurePath = CxFileSystem::mergeFilePath( measureCommonPath(),   CS_FileNameStrawMeasure );
        }
    }
    return f_sStrawMeasurePath;
}

const string &GclConfig::calcMeasurePath()
{
    if (f_sCalcMeasurePath.empty())
    {
        f_sCalcMeasurePath = CxFileSystem::mergeFilePath( measurePath(),   CS_FileNameCalcMeasure  );
        if(!CxFileSystem::isExist(f_sCalcMeasurePath))
        {
            f_sCalcMeasurePath = CxFileSystem::mergeFilePath( measureCommonPath(),   CS_FileNameCalcMeasure );
        }
    }
    return f_sCalcMeasurePath;
}


const string &GclConfig::monsbOperatesPath()
{
    if (f_sMonsbOperatesPath.empty())
    {
        f_sMonsbOperatesPath = CxFileSystem::mergeFilePath( measurePath(),   CS_FileNameMonsbOperates  );
    }
    return f_sMonsbOperatesPath;
}

const string &GclConfig::scriptPath()
{
    if (f_sScriptPath.empty())
    {
        f_sScriptPath = CxFileSystem::mergeFilePath( CxFileSystem::mergeFilePath( deployPath(),   CS_DirNameScript), CxApplication::applicationTargetName() );
    }
    return f_sScriptPath;
}

const string &GclConfig::scriptCommonPath()
{
    if (f_sScriptCommonPath.empty())
    {
        f_sScriptCommonPath = scriptPath();
        f_sScriptCommonPath = CxFileSystem::parentPath(f_sScriptCommonPath);
    }
    return f_sScriptCommonPath;
}

const string &GclConfig::uiPath()
{
    if (f_sUiPath.empty())
    {
        f_sUiPath = CxFileSystem::mergeFilePath( deployPath(),   CS_DirNameUi  );
    }
    return f_sUiPath;
}

bool GclConfig::isAutoStart()
{
    return f_bIsAutoStart;
}

void GclConfig::setIsAutoStart(bool value)
{
    f_bIsAutoStart = value;
}

bool GclConfig::isTesting()
{
    return f_bIsTesting;
}

void GclConfig::setIsTesting(bool value)
{
    f_bIsTesting = value;
}

bool GclConfig::isWriteLog()
{
    return f_bIsWriteLog;
}

void GclConfig::setIsWriteLog(bool value)
{
    f_bIsWriteLog = value;
}

bool GclConfig::isStartWatchdog()
{
    return f_bIsStartWatchdog;
}

void GclConfig::setIsStartWatchdog(bool value)
{
    f_bIsStartWatchdog = value;
}

bool GclConfig::isLoadMonsbOperate()
{
    return f_bIsLoadMonsbOperate;
}

void GclConfig::setIsLoadMonsbOperate(bool value)
{
    f_bIsLoadMonsbOperate = value;
}

const string &GclConfig::defaultDbName()
{
    if (f_defaultDbName.empty())
        f_defaultDbName = CxApplication::applicationTargetName() + ".db";
    return f_defaultDbName;
}

string fn_uiFilePath(const string & sFullName)
{
    string sUiFileName = sFullName + ".ui";
    sUiFileName = CxApplication::findConfig(CS_SectionUiFiles, sFullName, sUiFileName);
    return CxFileSystem::mergeFilePath(GclConfig::uiPath(), sUiFileName);
}

string GclConfig::protocolUiFilePath(const string &sSimpleName)
{
    return fn_uiFilePath("Protocol." + sSimpleName);
}

string GclConfig::channelUiFilePath(const string &sSimpleName)
{
    return fn_uiFilePath("Channel." + sSimpleName);
}

string GclConfig::businessUiFilePath(const string &sSimpleName)
{
    return fn_uiFilePath("Business." + sSimpleName);
}

//获取链路状态
bool GclConfig::isChanState()
{
    return f_bIsChannelState;
}

void GclConfig::setIsChanState(bool value)
{
    f_bIsChannelState = value;
    CxApplication::saveConfig(CS_SectionProgramConfig,  CS_EntryChannelState,CxString::toString(value));

}
//获取链路状态基址
int GclConfig::getChannelBaseAddr()
{
    return f_nChannelBaseAddr;
}

void GclConfig::setChannelBaseAddr(int value)
{
    f_nChannelBaseAddr = value+CGChannelBase;
    CxApplication::saveConfig(CS_SectionProgramConfig,  CS_EntryChannelBaseAddr,CxString::toString(value));

}

int GclConfig::mqPort()
{
    return f_nMqPort;
}

void GclConfig::setMqPort(int value)
{
    f_nMqPort = value;
    CxApplication::saveConfig(CS_SectionProgramConfig, CS_EntryMQPort, CxString::toString(value));
}

string GclConfig::mqIp()
{
    return f_sMqIp;
}

void GclConfig::setMqIp(const string &value)
{
    f_sMqIp = value;
    CxApplication::saveConfig(CS_SectionProgramConfig, CS_EntryMQIP, value);
}

bool GclConfig::isByteInverse()
{
    return f_bIsByteInverse;
}

void GclConfig::setByteInverse(bool value)
{
    f_bIsByteInverse = value;
    CxApplication::saveConfig(CS_SectionProgramConfig, CS_EntryByteInverse, CxString::toString(value));

}

int GclConfig::getDBType_ICS()
{
    return f_nDBtype;
}

void GclConfig::setDBType_ICS(int value)
{
    f_nDBtype = value;
    CxApplication::saveConfig(CS_SectionProgramConfig, CS_EntryDBType_ICS, CxString::toString(value));

}

string GclConfig::getDSN_ICS()
{
    return f_sDSN_ICS;
}

void GclConfig::setDSN_ICS(const string & value)
{
    f_sDSN_ICS = value;
    CxApplication::saveConfig(CS_SectionProgramConfig, CS_EntryDSN_ICS, value);

}

string GclConfig::getUser_ICS()
{
    return f_sUser_ICS;
}

void GclConfig::setUser_ICS(const string & value)
{
    f_sUser_ICS = value;
    CxApplication::saveConfig(CS_SectionProgramConfig, CS_EntryUSER_ICS, value);
}

string GclConfig::getPassWord_ICS()
{
    return f_sPassWord_ICS;
}

void GclConfig::setPassWord_ICS(const string & value)
{
    f_sUser_ICS = value;
    CxApplication::saveConfig(CS_SectionProgramConfig, CS_EntryPW_ICS, value);

}

uint64 GclConfig::getShareMemSize()
{
    return f_lShareMemSize;
}

void GclConfig::setShareMemSize(uint64 value)
{
    f_lShareMemSize = value;
    CxApplication::saveConfig(CS_SectionProgramConfig,  CS_EntryShareMemSize,CxString::toString(value));
}

std::string GclConfig::log_directory() const
{
    return GclConfig::logPath();
}

std::string GclConfig::log_fileName(int type, int reason, int source, int target, int tag) const
{
    static std::string m = "system.log";
    return m;
}

bool GclConfig::log_hasFileName(int type, int reason, int source, int target, int tag) const
{
    return false;
}
