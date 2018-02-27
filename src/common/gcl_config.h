#ifndef GCL_CONFIG_H
#define GCL_CONFIG_H

#include <ccxx/cxglobal.h>
#include <ccxx/cxlog.h>

class GclConfig : public CxLogFileTactics_I
{
public:
    static void start();

    static void stop();

    static void load();

    //发布的目录
    static const std::string & deployPath       ();

    //配置目录
    static const std::string & configPath       ();

    //数据库路径
    static const std::string & databasePath     ();

    //Data路径
    static const std::string & dataPath      ();

    //Data公共路径
    static const std::string & dataCommonPath();

    //Model路径
    static const std::string & modelPath      ();

    //Model公共路径
    static const std::string & modelCommonPath();

    //Measure路径
    static const std::string & measurePath      ();

    //Measure路径
    static const std::string & measureCommonPath();

    //temp路径
    static const std::string & tempPath         ();

    //日志路径
    static const std::string & logPath          ();

    //日志公共路径
    static const std::string & logCommonPath    ();

    //日志路径
    static const std::string & reportPath       ();

    //日志路径
    static const std::string & reportDefaultFilePath();

    //terminal路径
    static const std::string & terminalPath     ();

    //business路径
    static const std::string & businessPath     ();

    //descript路径
    static const std::string & describePath     ();

    //脚本路径
    static const std::string & scriptPath       ();

    //脚本公共路径
    static const std::string & scriptCommonPath ();

    //Ui路径
    static const std::string & uiPath           ();

    //monsb.measure.txt file path
    static const std::string & monsbMeasurePath ();

    //ycadd.measure.txt file path
    static const std::string & ycaddMeasurePath ();

    //straw.measure.txt file path
    static const std::string & strawMeasurePath ();

    //calc.measure.txt file path
    static const std::string & calcMeasurePath ();

    //monsb.operates.txt file path
    static const std::string & monsbOperatesPath();

    static bool isAutoStart                 ();
    static void setIsAutoStart(bool value);

    static bool isTesting                   ();
    static void setIsTesting(bool value);

    static bool isWriteLog                  ();
    static void setIsWriteLog(bool value);

    static bool isStartWatchdog             ();
    static void setIsStartWatchdog(bool value);

    static bool isLoadMonsbOperate          ();
    static void setIsLoadMonsbOperate(bool value);

    static const std::string & defaultDbName();

    static std::string protocolUiFilePath(const std::string & sSimpleName);

    static std::string channelUiFilePath(const std::string & sSimpleName);

    static std::string businessUiFilePath(const std::string & sSimpleName);

//链路监控
    static bool isChanState();
    static void setIsChanState(bool value);

    static  int getChannelBaseAddr();
    static void setChannelBaseAddr(int value);

//MQ参数
    static int  mqPort();
    static void setMqPort(int value);

    static std::string mqIp();
    static void setMqIp(const std::string & value);

//字节序
    static bool isByteInverse();
    static void setByteInverse(bool value);

//数据库
    static int getDBType_ICS();
    static void setDBType_ICS(int value);

    static std::string getDSN_ICS();
    static void setDSN_ICS(const std::string &value);

    static std::string getUser_ICS();
    static void setUser_ICS(const std::string &value);

    static std::string getPassWord_ICS();
    static void setPassWord_ICS(const std::string &value);

//ShareMem
    static uint64 getShareMemSize();
    static void setShareMemSize(uint64 value);

    static std::string regstringIp(){     return std::string("^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"); }

    static std::string regstringNumber(){ return std::string("^[0-9]+\\.{0,1}[0-9]{0,2}$"); }

    static std::string regstringName(){   return std::string("^[a-zA-Z0-9_-]{3,16}$"); }

    static std::string regstringHex2(){   return std::string("^[0-9a-fA-F]{1,2}$"); }

    static std::string regstringHex4(){   return std::string("^[0-9a-fA-F]{1,4}$"); }

    static std::string regstringHex8(){   return std::string("^[0-9a-fA-F]{1,8}$"); }

    static std::string regstringDate(){   return std::string("^(^(\\d{4}|\\d{2})(\\-|\\/|\\.)\\d{1,2}\\3\\d{1,2}$)|(^\\d{4}年\\d{1,2}月\\d{1,2}日$)$"); }

    static std::string regstringPath(){   return std::string("[a-z|A-Z]:([\\\\][\\w|\\s|\\-|\\.|\\x81-\\xfe|\\x40-\\xfe]+)+$"); }

public:
    std::string log_directory() const;

    std::string log_fileName( int type, int reason, int source, int target, int tag ) const;

    bool log_hasFileName( int type, int reason, int source, int target, int tag ) const;

};

#define MinTerminalId (0x01800000)
#define MaxTerminalId (0x018FFFFF)

#define isValidTerminalId(id) ((id > 0x01800000) && (id < 0x018FFFFF))

#endif // GCL_CONFIG_H

