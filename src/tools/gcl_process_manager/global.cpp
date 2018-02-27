#include "global.h"


using namespace std;

char pathCharacter = '\\';
string applicationFilePath = "";
string applicationConfigFilePath = "";
string applicationPath = "";

//void GlobalFunction::fn_applicationInit(int argc, const char *argv[])
//{
//    string sName;
//    string sPath;


//    if( argc > 0)
//    {
//        applicationFilePath = argv[0];
//        size_t num1 = applicationFilePath.find_last_of("/\\");
//        if( num1 != string::npos )
//        {
//            sName = applicationFilePath.substr( num1+1 );
//            sPath = applicationFilePath.substr(0, num1);
//            applicationPath = sPath;
//        }

//        size_t num2 = sName.find_last_of(".");
//        if(num2 != string::npos)
//        {
//            sName = sName.substr(0, num2);
//        }
//        sName = sName.append(".xml");

//        size_t index = applicationFilePath.find("\\");
//        if(index != string::npos)
//            pathCharacter = '\\';
//        else
//            pathCharacter = '/';
//        sPath.push_back(pathCharacter);
//        applicationConfigFilePath = sPath.append(sName);

//    }
//}

const string GlobalFunction::getApplicationFilePath()
{
    return applicationFilePath;
}


const string GlobalFunction::getApplicationConfigFilePath()
{
    applicationConfigFilePath = CxFileSystem::mergeFilePath( CxApplication::configPath(), "gcl_daemon.extend.xml");

    return applicationConfigFilePath;
}

const string GlobalFunction::getApplicationPath()
{
    return applicationPath;
}

bool GlobalFunction::fn_isValidHexCharater( const string & s )
{
    const char *pch = s.data();
    const char *pchEnd = pch + s.size();
    while( pch < pchEnd )
    {
        if( ! (( *pch >= '0' && *pch <= '9') || ( *pch >= 'a' && *pch <= 'f') || ( *pch >= 'A' && *pch <= 'F' )) )
            return false;
        pch++;
    }
    return true;
}

vector<char> GlobalFunction::fn_fromHexstring(const string &s)
{
    string s2 = s;
    string::iterator it;
    for (it = s2.begin(); it != s2.end();)
    {
        if(*it == ' ')
            it = s2.erase(it);
        else
            ++it;
    }
    vector<char> res((s2.size() + 1)/ 2);
    char *result = res.data() + res.size();

    bool odd_digit = true;
    for (int i = s2.size() - 1; i >= 0; --i) {
        int ch = s2[i];
        int tmp;
        if (ch >= '0' && ch <= '9')
            tmp = ch - '0';
        else if (ch >= 'a' && ch <= 'f')
            tmp = ch - 'a' + 10;
        else if (ch >= 'A' && ch <= 'F')
            tmp = ch - 'A' + 10;
        else
            continue;
        if (odd_digit) {
            --result;
            *result = tmp;
            odd_digit = false;
        } else {
            *result |= tmp << 4;
            odd_digit = true;
        }
    }

    vector<char> rData;
    rData.resize(res.data() + res.size() - result);
    ::copy(result, res.data() + res.size(), rData.begin());
//    res.remove(0, result - res.data());
    return rData;
}


string GlobalFunction::fn_toHexstring(const char *pData, int iLength, bool bHasEmptyChar = true)
{
    if (bHasEmptyChar)
    {
        vector<char> hex(iLength * 3);
        char *hexData = hex.data();
//        const uchar *data = (const uchar *)pData;
        for (int i = 0; i < iLength; ++i) {
            int j = (pData[i] >> 4) & 0xf;
            if (j <= 9)
                hexData[i*3] = (j + '0');
            else
                hexData[i*3] = (j + 'a' - 10);
            j = pData[i] & 0xf;
            if (j <= 9)
                hexData[i*3+1] = (j + '0');
            else
                hexData[i*3+1] = (j + 'a' - 10);
            hexData[i*3+2] = 0x20;
        }
        return string(hex.data(), hex.size());
    }
    else
    {
        vector<char> hex(iLength * 2);
        char *hexData = hex.data();
//        const uchar *data = (const uchar *)pData;
        for (int i = 0; i < iLength; ++i) {
            int j = (pData[i] >> 4) & 0xf;
            if (j <= 9)
                hexData[i*2] = (j + '0');
            else
                hexData[i*2] = (j + 'a' - 10);
            j = pData[i] & 0xf;
            if (j <= 9)
                hexData[i*2+1] = (j + '0');
            else
                hexData[i*2+1] = (j + 'a' - 10);
        }
        return string(hex.data(), hex.size());
    }
}

msepoch_t GlobalFunction::fn_currentMsepoch()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

    const uint64 iShift = 116444736000000000ULL; // (27111902 << 32) + 3577643008

    union {
        FILETIME as_file_time;
        uint64 as_integer; // 100-nanos since 1601-Jan-01
    } caster;
    caster.as_file_time = ft;

    caster.as_integer -= iShift; // filetime is now 100-nanos since 1970-Jan-01
    uint64 iMicros = caster.as_integer / 10; // truncate to microseconds

    // and cannot be before 1970-Jan-01
    time_t t = static_cast<time_t>(iMicros / 1000000UL); // seconds since epoch
    // microseconds -- static casts supress warnings
    unsigned int sub_sec = static_cast<unsigned int>(iMicros % 1000000UL);
    msepoch_t dt = t;
    return dt * 1000 + sub_sec / 1000;
}

unsigned int julianDayFromGregorianDate1(int year, int month, int day)
{
    // Gregorian calendar starting from October 15, 1582
    // Algorithm from Henry F. Fliegel and Thomas C. Van Flandern
    return (1461 * (year + 4800 + (month - 14) / 12)) / 4
            + (367 * (month - 2 - 12 * ((month - 14) / 12))) / 12
            - (3 * ((year + 4900 + (month - 14) / 12) / 100)) / 4
            + day - 32075;
}

unsigned int julianDayFromDate1(int year, int month, int day)
{
    if (year < 0)
        ++year;

    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15)))) {
        return julianDayFromGregorianDate1(year, month, day);
    } else if (year < 1582 || (year == 1582 && (month < 10 || (month == 10 && day <= 4)))) {
        // Julian calendar until October 4, 1582
        // Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
        int a = (14 - month) / 12;
        return (153 * (month + (12 * a) - 3) + 2) / 5
                + (1461 * (year + 4800 - a)) / 4
                + day - 32083;
    } else {
        // the day following October 4, 1582 is October 15, 1582
        return 0;
    }
}

void fn_decodeLocalTm(const msepoch_t &dt, int &y, int &m, int &d, int &h, int &mi, int &se, int &ms)
{
    msepoch_t msecs = dt + GM_TIME_UTC_DIFF_MS;
    int ddays = msecs / 86400000;
    msecs %= 86400000;
    if (msecs < 0) {
        // negative
        --ddays;
        msecs += 86400000;
    }

    unsigned int jd = julianDayFromDate1(1970, 1, 1);
    if (ddays >= 0)
        ddays = (jd + ddays >= jd) ? jd + ddays : 0;
    else
        ddays = (jd + ddays < jd) ? jd + ddays : 0;

    if (ddays >= 2299161) {
        // Gregorian calendar starting from October 15, 1582
        // This algorithm is from Henry F. Fliegel and Thomas C. Van Flandern
        msepoch_t ell, n, i, j;
        ell = msepoch_t(ddays) + 68569;
        n = (4 * ell) / 146097;
        ell = ell - (146097 * n + 3) / 4;
        i = (4000 * (ell + 1)) / 1461001;
        ell = ell - (1461 * i) / 4 + 31;
        j = (80 * ell) / 2447;
        d = ell - (2447 * j) / 80;
        ell = j / 11;
        m = j + 2 - (12 * ell);
        y = 100 * (n - 49) + i + ell;
    } else {
        // Julian calendar until October 4, 1582
        // Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
        ddays += 32082;
        int dd = (4 * ddays + 3) / 1461;
        int ee = ddays - (1461 * dd) / 4;
        int mm = ((5 * ee) + 2) / 153;
        d = ee - (153 * mm + 2) / 5 + 1;
        m = mm + 3 - 12 * (mm / 10);
        y = dd - 4800 + (mm / 10);
        if (y <= 0)
            --y;
    }

    int mds = msecs % 86400000;
    if (msecs < 0) {
        // % not well-defined for -ve, but / is.
        int negdays = (86400000 - msecs) / 86400000;
        mds = (msecs + negdays * 86400000) % 86400000;
    }

    h = mds / 3600000;
    mi = (mds % 3600000) / 60000;
    se = (mds / 1000) % 60;
    ms = (mds % 1000);
}

string GlobalFunction::fn_toString(int y, int m, int d, int h, int mi, int se, int ms)
{
    char dts[23] = {'2', '0', '1', '2', '/', '1', '2', '/', '1', '2', ' ', '1', '2', ':', '1', '2', ':', '1', '2', ':', '1', '2', '2'};

    if (y > -1 && y < 10000 && m > 0 && m < 13 && d > 0 && d < 32 && h > -1 && h < 24 && mi > -1 && mi < 60 && se > -1 && se < 100)
    {
        int hc, lc;

        int yy = y % 100;

        hc = yy / 10;
        lc = yy % 10;
        dts[2] = CCHexChar16[ hc ];
        dts[3] = CCHexChar16[ lc ];

        hc = m / 10;
        lc = m % 10;
        dts[5] = CCHexChar16[ hc ];
        dts[6] = CCHexChar16[ lc ];

        hc = d / 10;
        lc = d % 10;
        dts[8] = CCHexChar16[ hc ];
        dts[9] = CCHexChar16[ lc ];

        hc = h / 10;
        lc = h % 10;
        dts[11] = CCHexChar16[ hc ];
        dts[12] = CCHexChar16[ lc ];

        hc = mi / 10;
        lc = mi % 10;
        dts[14] = CCHexChar16[ hc ];
        dts[15] = CCHexChar16[ lc ];

        hc = se / 10;
        lc = se % 10;
        dts[17] = CCHexChar16[ hc ];
        dts[18] = CCHexChar16[ lc ];

        hc = ms / 100;
        lc = (ms % 100) / 10;
        dts[20] = CCHexChar16[ hc ];
        dts[21] = CCHexChar16[ lc ];
        lc = ms % 10;
        dts[22] = CCHexChar16[ lc ];
    }
    else
    {
    }

    return string(dts);
}

string GlobalFunction::fn_toString(const msepoch_t &lMsepoch)
{
    int y, m, d, h, mi, se, ms;
    fn_decodeLocalTm(lMsepoch, y, m, d, h, mi, se, ms);
    return fn_toString(y , m, d, h, mi, se, ms);
}

string GlobalFunction::fn_toString(int i)
{
    char str[10+1];
    sprintf(str, "%d", i);
    return string(str);
}

string GlobalFunction::fn_toString(const ulong &n)
{
    stringstream ss;
    ss << n;
    return ss.str();
}

string GlobalFunction::fn_toString(const uint64 &n)
{
    stringstream ss;
    ss << n;
    return ss.str();
}

string GlobalFunction::fn_format(const char *sFormat, ...)
{
    char buffer[512];
    va_list args;
    va_start (args, sFormat);
    vsprintf (buffer, sFormat, args);
    va_end (args);
    return buffer;
}
