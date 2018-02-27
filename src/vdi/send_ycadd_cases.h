#ifndef SEND_YCADD_CASES_H
#define SEND_YCADD_CASES_H

#include "protocol_global.h"


#include "send_measure_cases_template.h"
#include "ycadd_measure.h"


template<> inline std::string ProtocolStrings::SendMeasureTypeSimpleName<YcaddMeasure>(){ return "sendYcadd"; }


class IProtocolSendYcadd : public IProtocolSendMeasureTemplate<YcaddMeasure, double>
{
public:
    //single
    int sendMeasure(const YcaddMeasure* oMeasure, double value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs())
    {
        return sendYcadd(oMeasure, value, argvs);
    }

    //group
    int sendMeasures(const std::map<const YcaddMeasure*, double>& measuresValue, int iMeasureSonType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs())
    {
        return sendYcadds(measuresValue, g_fn_ycaddTypeEnum(iMeasureSonType), argvs);
    }

    //all
    int sendAllMeasure(int iMeasureSonType, double * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs())
    {
        return sendAllYcadd(g_fn_ycaddTypeEnum(iMeasureSonType), value, argvs);
    }

    virtual int sendYcadd(const YcaddMeasure* oYcadd, double value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs())
    {
        return SendMeasureFail_Noprovide;
    }

    virtual int sendYcadds(const std::map<const YcaddMeasure*, double>& YcaddValues, YcaddTypeEnum eYcaddType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs())
    {
        return SendMeasureFail_Noprovide;
    }

    virtual int sendAllYcadd(YcaddTypeEnum eYcaddType, double * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs())
    {
        return SendMeasureFail_Noprovide;
    }

};

class SendYcaddCases : public SendMeasureCasesTemplate<YcaddMeasure, double>
{
public:

};

#endif // !defined(SEND_YCADD_CASES_H)
