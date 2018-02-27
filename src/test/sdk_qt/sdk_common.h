#ifndef SDK_COMMON_H
#define SDK_COMMON_H


#include "cxstring.h"
#include "cxtime.h"

#include <QString>
#include <QMutex>
#include <QDateTime>

#include "gcl_common_sdk.h"
#include "psm_common.h"

#define cs_datetime_format_ ("yyyy-MM-dd hh:mm:ss")


class MyMutexScope
{
private:
    QMutex * m_mutex;

public:
    inline MyMutexScope(QMutex * mutex) : m_mutex(mutex) {
        m_mutex->lock();
    }

    inline MyMutexScope(QMutex & mutex) : m_mutex(& mutex) {
        m_mutex->lock();
    }

    inline ~MyMutexScope() { m_mutex->unlock(); }

};


struct MyMessageCommand
{
    std::string command;
    std::string param;
    PsmAttach attach;
    bool isValid;
};

struct MyFileTransfer
{
    std::string filePath;
    PsmAttach attach;
    bool isValid;
};

struct MyRealtimeDataRequest
{
    PsmAttach attach;
    bool isValid;
};

struct MyRealtimeDataPost
{
    int paramType;
    int paramCount;
    std::string param;
    PsmAttach attach;
    bool isValid;
};

struct yx_t
{
    int address;
    int quality;
    int state;
    int res;
    long long datetime;
};

struct yc_t
{
    int address;
    int quality;
    double value;
    long long datetime;
};

struct yw_t
{
    int address;
    int quality;
    char value[128];
    long long datetime;
};

class SdkCommon
{
public:
    static QString gbkToQString(const std::string& s);

    static QStringList gbkToQStrings(const std::vector<std::string>& ss);

    static QStringList gbkToQStrings(const std::string * oSs, int iCount);

    static std::vector<std::string> gbkToStdStrings(const QStringList & ss);

    static std::string gbkToStdString(const QString& s);


    static bool DialogInput(const QString& sPrompt, QString& sVaule);

    static bool DialogInput2(const QString &sTitle, const QString &sValue1Title, const QString &sValue2Title, QString &sValue1, QString &sValue2, int iValue1EchoMode = 0, int iValue2EchoMode = 0);

    static bool DialogInput3(const QString &sTitle, const QString &sValue1Title, const QString &sValue2Title, const QString &sValue3Title, QString &sValue1, QString &sValue2, QString &sValue3, int iValue1EchoMode = 0, int iValue2EchoMode = 0, int iValue3EchoMode = 0);


    static const std::vector<yx_t> & yxs();

    static yx_t * getYx(int iAddress);

    static void setYx(int address, int quality, int state, long long datetime);

    static const std::vector<yc_t> & ycs();

    static yc_t * getYc(int iAddress);

    static void setYc(int address, int quality, double value, long long datetime);

    static const std::vector<yw_t> & yws();

    static yw_t * getYw(int iAddress);

    static void setYw(int address, int quality, const char * value, long long datetime);

    static std::vector<int> dealYx_01(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach);

    static void dealYx_02(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach) {}

    static std::vector<int> dealYc_01(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach);

    static void dealYc_02(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach) {}

    static std::vector<int> dealYw_01(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach);

    static void dealYw_02(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach) {}

    static bool dialogInput(const QString& sPrompt, QString& sVaule);

};



#endif // SDK_COMMON_H
