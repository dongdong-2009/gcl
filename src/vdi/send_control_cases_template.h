#ifndef SEND_CONTROL_CASES_TEMPLATE_H
#define SEND_CONTROL_CASES_TEMPLATE_H

#include "cases_base.h"
#include "measure_manager_template.h"


struct SendControlArgvs
{
public:
    int source;
    int reason;
    std::string extension;

    static const SendControlArgvs & nullArgs();
};


enum SendControlFailEnum {
    SendControlFail_Unknow = -1,
    SendControlFail_IO_Invalid = -2,
    SendControlFail_IO_Send = -3,
    SendControlFail_Noprovide = -4,
    SendControlFail_Nomeasure = -5,
    SendControlFail_Exscope = -6,
    SendControlFail_NoWorkCode = -7
};


template<class MeasureSon, typename T>
class IProtocolSendControlTemplate
{
public:
    //single
    virtual int sendControl(const MeasureSon* oMeasure, T fromValue, T toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs()) { return SendControlFail_Noprovide; }
    //group
    virtual int sendControls(const std::vector<std::pair<const MeasureSon*, T> >& measuresToValue, const std::vector<SendControlArgvs> & argvs = std::vector<SendControlArgvs>()) { return SendControlFail_Noprovide; }
    //all
    virtual int sendAllControl(T toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs()) { return SendControlFail_Noprovide; }
    //query
    virtual bool provideSendControl(int iMeasureSonType, const MeasureSon* oMeasure = NULL) = 0;

};


template<class MeasureSon, typename T>
class SendControlCasesTemplate : public CasesBase
{

    GM_TYPEFULLNAME(CasesBase, ProtocolStrings::SendControlTypeSimpleName<MeasureSon>())

public:

    SendControlCasesTemplate() {
        _protocolSendControl = NULL;
    }

    ~SendControlCasesTemplate() {
    }

    inline const IProtocolSendControlTemplate<MeasureSon, T>* protocolSendControl(){ return _protocolSendControl; }

    inline void setProtocol(IProtocolSendControlTemplate<MeasureSon, T>* oProtocol){ _protocolSendControl = oProtocol; }

    int sendControl(const MeasureSon* oMeasure, T fromValue, T toValue){
        return _protocolSendControl->sendControl(oMeasure, fromValue, toValue);
    }

    int sendControl(const MeasureSon* oMeasure, T toValue){
        if (oMeasure) {
            return _protocolSendControl->sendControl(oMeasure, oMeasure->value(), toValue);
        } else {
            return SendControlFail_Nomeasure;
        }
    }

    int sendControl(int iMeasureId, T fromValue, T toValue)
    {
        const MeasureSon* oMeasure = MeasureManagerTemplate<MeasureSon, T>::measure(iMeasureId);
        if (oMeasure)
            return sendControl(oMeasure, fromValue, toValue);
        else
            return SendControlFail_Nomeasure;
    }

    int sendControl(int iMeasureId, T toValue)
    {
        const MeasureSon* oMeasure = MeasureManagerTemplate<MeasureSon, T>::measure(iMeasureId);
        if (oMeasure)
            return sendControl(oMeasure, toValue);
        else
            return SendControlFail_Nomeasure;
    }

    int sendControls(const std::vector<std::pair<const MeasureSon*, T> > & measuresToValue)
    {
        return _protocolSendControl->sendControls(measuresToValue);
    }

    int sendControls(const std::vector<int>& oMeasureIds, T toValue)
    {
        std::vector<std::pair<const MeasureSon*, T> > measureValues = MeasureSonManager::measureValueList(oMeasureIds, toValue);
        return sendControls(measureValues);
    }

    int sendControls(const std::vector<const MeasureSon*> & measures, T toValue)
    {
        std::vector<std::pair<const MeasureSon*, T> > measureValues = MeasureSonManager::measureValueList(measures, toValue);
        return sendControls(measureValues);
    }

    int sendAllControl(T toValue)
    {
        return _protocolSendControl->sendAllControl(toValue);
    }

    bool provideSendControl(int iMeasureSonType, const MeasureSon* oMeasure = NULL)
    {
        return _protocolSendControl->provideSendControl(iMeasureSonType, oMeasure);
    }

protected:
    typedef MeasureManagerTemplate<MeasureSon, T> MeasureSonManager;

private:
    IProtocolSendControlTemplate<MeasureSon, T>* _protocolSendControl;


};
#endif // SEND_CONTROL_CASES_TEMPLATE_H


