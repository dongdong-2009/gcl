#include "send_monsb_control_cases.h"

#include "terminal_manager.h"

int SendMonsbControlManager::sendControl(const MonsbMeasure *oMeasure, int fromValue, int toValue, int iSourceId)
{
    typedef SendControlCasesTemplate<MonsbMeasure, int> SendMonsbControlCasesT;

    int result = 0;
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase* oTerminal = oTerminals->at(i);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol && oProtocol->protocolId() != iSourceId)
        {
            SendMonsbControlCasesT* oCases = oProtocol->casesSon<SendMonsbControlCasesT>();
            if (oCases)
            {
                result += oCases->sendControl(oMeasure, fromValue, toValue);
            }
        }
    }
    return result;
}

int SendMonsbControlManager::sendControl(int iMeasureId, int toValue, int iSourceId)
{
	return 0;
}

int SendMonsbControlManager::statisticseMeasureCasesCount(const MonsbMeasure *oMeasure, int iSourceId)
{
    int iCasesCounts = 0;
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i) {
        TerminalBase* oTerminal = oTerminals->at(i);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol && oProtocol->protocolId() != iSourceId){
            SendMonsbControlCases* oCases = oProtocol->casesSon<SendMonsbControlCases>();
            if (oCases && oCases->provideSendControl(oMeasure->measureSonType(), oMeasure)){
                ++ iCasesCounts;
            }
        }
    }
    return iCasesCounts;
}

