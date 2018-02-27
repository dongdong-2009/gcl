#ifndef SEND_MONSB_CONTROL_CASES_H
#define SEND_MONSB_CONTROL_CASES_H


#include "send_control_cases_template.h"
#include "monsb_measure.h"

template<> inline std::string ProtocolStrings::SendControlTypeSimpleName<MonsbMeasure>(){ return "SendMonsbControl"; }

struct MonsbControlFromToState : public std::pair<int, int>
{

};

class IProtocolSendMonsbControl : public IProtocolSendControlTemplate<MonsbMeasure, int>
{
public:
};

class SendMonsbControlCases : public SendControlCasesTemplate<MonsbMeasure, int>
{
public:

};

class GM_GCL_VDI_API SendMonsbControlManager
{
public:
    //发送控制指令
    static int sendControl(const MonsbMeasure* oMeasure, int fromValue, int toValue, int iSourceId);

    static int sendControl(int iMeasureId, int toValue, int iSourceId);

    static int statisticseMeasureCasesCount(const MonsbMeasure* oMeasure, int iSourceId);

};

#endif // SEND_MONSB_CONTROL_CASES_H
