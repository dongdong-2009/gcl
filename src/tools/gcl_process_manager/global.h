#ifndef GLOBAL_H
#define GLOBAL_H
#include <ccxx/tinyxml.h>
#include <ccxx/cxapplication.h>
#include <ccxx/cxinterinfo.h>
#include <ccxx/cxfile.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>

#undef UNICODE
//#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <tlhelp32.h>
#include <winsock2.h>
#include <psapi.h>

#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <QtGui>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <qmutex.h>
#include <QDateTime>

typedef long long msepoch_t; //毫秒计数
typedef long long int64;
typedef unsigned long long uint64;

const char CCHexChar16[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

#define GM_TIME_UTC_MST (-7)
#define GM_TIME_UTC_    (0)
#define GM_TIME_UTC_CCT (+8)

#define GM_TIME_UTC_LOCAL GM_TIME_UTC_CCT

#define GM_TIME_UTC_DIFF_MS (GM_TIME_UTC_LOCAL * 60 * 60 * 100)

class GlobalFunction
{
public:
    static bool fn_isValidHexCharater( const std::string & s );
    static std::vector<char> fn_fromHexstring(const std::string &s);
    static std::string fn_toHexstring(const char *pData, int iLength, bool bHasEmptyChar);

    static msepoch_t fn_currentMsepoch();
    static std::string fn_toString(int i);
    static std::string fn_toString(const msepoch_t &lMsepoch);
    static std::string fn_toString(int y, int m, int d, int h, int mi, int se, int ms);
    static std::string fn_toString(const ulong &n);
    static std::string fn_toString(const uint64 &n);


    static void fn_applicationInit(int argc, const char *argv[]);
    static const std::string getApplicationFilePath();
    static const std::string getApplicationConfigFilePath();
    static const std::string getApplicationPath();

    static std::string fn_format(const char *sFormat, ...);


};

#endif // GLOBAL_H
