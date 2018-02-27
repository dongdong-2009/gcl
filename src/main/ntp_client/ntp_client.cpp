#include "ntp_client.h"


using namespace std;


static const std::string CS_SectionNtpClient = "NtpClient";
static const std::string CS_EntryNtpInterval = "NtpInterval";


static int f_iNtpInterval = 30000;

static NtpClient * fn_oNtpClient()
{
    CxApplication::registStartFunction(NtpClient::start);
    CxApplication::registStopFunction(NtpClient::stop);
    GeneralAccess::registerMessageCommandDeal(NtpClient::dealMessage);
    GeneralAccess::registerFileWriteDeal(NtpClient::dealFileWrite);

    static NtpClient m;
    return &m;
}

static NtpClient * m_oNtpClient = fn_oNtpClient();


void fn_setSystemTime(msepoch_t dtNtp)
{
    int y; int m; int d; int h; int mi; int se; int ms;
    CxTime::decodeLocalTm(dtNtp, y, m, d, h, mi, se, ms);
#ifdef GM_OS_WIN
    SYSTEMTIME dtSet;
    dtSet.wYear = y;
    dtSet.wMonth = m;
    dtSet.wDayOfWeek = 0;
    dtSet.wDay = d;
    dtSet.wHour = h;
    dtSet.wMinute = mi;
    dtSet.wSecond = se;
    dtSet.wMilliseconds = ms;
    SetSystemTime(&dtSet);
#else
    string dtSet = CxString::format("date -s \"%d-%d-%d %d:%d:%d\"", y, m, d, h, mi, se);
    system(dtSet.c_str());
    system("clock -w");//强制写入到CMOS
#endif
}



void NtpClient::start()
{
    f_iNtpInterval = CxApplication::findConfig(CS_SectionNtpClient, CS_EntryNtpInterval, f_iNtpInterval);
    if (f_iNtpInterval>=1000)
    {
        CxTimerManager::startTimer(NtpClient::timerNtp, f_iNtpInterval);
    }
}

void NtpClient::stop()
{
}

void NtpClient::timerNtp(int iInterval)
{
    string sParam = CxString::format("t1=%ld;",  CxTime::currentMsepoch());
    GeneralAccess::postMessage("post.ntp..", sParam);
}

int NtpClient::dealMessage(const PsmExplainer *oPsm, const string &sCommand, const string &sParam, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    if(sCommand.find(".ntp.") != string::npos)
    {
        msepoch_t t4 = CxTime::currentMsepoch();
        map<string, string> sParams = CxString::splitToMap(sParam,'=',';');
        msepoch_t t1 = CxContainer::valueTo(sParams, string("t1"), ci_longlong_zero);
        msepoch_t t2 = CxContainer::valueTo(sParams, string("t2"), ci_longlong_zero);
        msepoch_t t3 = CxContainer::valueTo(sParams, string("t3"), ci_longlong_zero);
        if (t1 == ci_longlong_zero || t2 == ci_longlong_zero || t3 ==ci_longlong_zero)
        {
            return true;
        }
        msepoch_t dtDiff = (t2 - t1) + (t3 - t4) / 2;
        if (dtDiff > -1000 && dtDiff < 1000)
        {
            msepoch_t dtNtp = t4 + dtDiff;
            if (dtNtp > GM_MSEPOCH_MIN && dtNtp < GM_MSEPOCH_MAX)
            {
                fn_setSystemTime(dtNtp);
                cxPrompt() << "NtpClient setSystemTime success --from:" << CxTime::toString(t4) << "(" << t4 << ") --to:" << CxTime::toString(dtNtp) << "(" << dtNtp << ") --diffMilliseconds:" << dtDiff;
            }
            else
            {
                cxPrompt() << "NtpClient setSystemTime error (dtNtp is valid) --from:" << CxTime::toString(t4) << "(" << t4 << ") --to:" << CxTime::toString(dtNtp) << "(" << dtNtp << ") --diffMilliseconds:" << dtDiff;
            }
        }
        else
        {
            cxPrompt() << "NtpClient setSystemTime error (dtDiff is valid)" << " --diffMilliseconds:" << dtDiff;
        }
        return true;
    }
    return false;
}

int NtpClient::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    return TRUE;
}

