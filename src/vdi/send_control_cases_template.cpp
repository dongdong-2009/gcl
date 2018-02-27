
#include "send_control_cases_template.h"

SendControlArgvs fn_nullArgs()
{
    SendControlArgvs rResult;
    rResult.source = -1;
    rResult.reason = -1;
    rResult.extension = "";
    return rResult;
}

static SendControlArgvs f_mSendControlArgvs = fn_nullArgs();

const SendControlArgvs &SendControlArgvs::nullArgs()
{
    return f_mSendControlArgvs;
}
