#ifndef SEND_MONSB_CASES_H
#define SEND_MONSB_CASES_H


#include "protocol_global.h"


#include "send_measure_cases_template.h"
#include "monsb_measure.h"

template<> inline std::string ProtocolStrings::SendMeasureTypeSimpleName<MonsbMeasure>(){ return "sendMonsb"; }

class IProtocolSendMonsb : public IProtocolSendMeasureTemplate<MonsbMeasure, int>
{
public:
    //single
    int sendMeasure(const MonsbMeasure* oMeasure, int value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return sendMonsb(oMeasure, value, argvs);
    }
    //group
    int sendMeasures(const std::map<const MonsbMeasure*, int>& measuresValue, int iMeasureSonType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return sendMonsbs(measuresValue, g_fn_monsbTypeEnum(iMeasureSonType), argvs);
    }
    //all
    int sendAllMeasure(int iMeasureSonType, int * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return sendAllMonsb(g_fn_monsbTypeEnum(iMeasureSonType), value, argvs);
    }

    virtual int sendMonsb(const MonsbMeasure* oMonsb, int value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return SendMeasureFail_Noprovide;
    }

    virtual int sendMonsbs(const std::map<const MonsbMeasure*, int>& monsbValues, MonsbTypeEnum eMonsbType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return SendMeasureFail_Noprovide;
    }

    virtual int sendAllMonsb(MonsbTypeEnum eMonsbType, int * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) {
        return SendMeasureFail_Noprovide;
    }

};

class SendMonsbCases : public SendMeasureCasesTemplate<MonsbMeasure, int>
{
public:

};

#endif // !defined(SEND_MONSB_CASES_H)
