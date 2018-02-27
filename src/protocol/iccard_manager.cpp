#include "iccard_manager.h"


static fn_iccard_process_trace_t fn_iccard_process_trace = (fn_iccard_process_trace_t)0;
static long long f_lastTimeUpdateIccard = 0;
static std::string f_lastIccard;


long long IccardManager::lastTimeUpdateIccard()
{
    return f_lastTimeUpdateIccard;
}

std::string IccardManager::lastIccard()
{
    return f_lastIccard;
}

void IccardManager::registProcessTrace(fn_iccard_process_trace_t fn)
{
    fn_iccard_process_trace = fn;
}

void IccardManager::unregistProcessTrace(fn_iccard_process_trace_t fn)
{
    fn_iccard_process_trace = 0;
}

void IccardManager::setLastIccard(const std::string &value)
{
    f_lastTimeUpdateIccard = CxTime::currentSystemTime();
    f_lastIccard = value;
    if (fn_iccard_process_trace)
    {
        fn_iccard_process_trace(1, 1, value, 0);
    }
}

std::string IccardManager::waitIccard(msepoch_t iTimeOut)
{
    msepoch_t dtNow = CxTime::currentSystemTime();
    if (dtNow > 0)
    {
        msepoch_t iG = CxTime::currentSystemTime() - dtNow;
        while (iG < iTimeOut)
        {
            iG = CxTime::currentSystemTime() - dtNow;
            if ( lastTimeUpdateIccard() - dtNow > 0 )
            {
                return lastIccard();
            }
            CxApplication::processEvents();
        }
    }
    return "";
}
