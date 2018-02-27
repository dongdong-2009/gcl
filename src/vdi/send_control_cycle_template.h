#ifndef SEND_CONTROL_CYCLE_TEMPLATE_H
#define SEND_CONTROL_CYCLE_TEMPLATE_H


#include "cycle_base.h"
#include "send_control_cases_template.h"


template<class MeasureSon, typename T>
class SendControlCycleTemplate : public CycleBase
{
public:
    SendControlCycleTemplate() {
        _protocolSendControl = NULL;
    }

    inline const IProtocolSendControlTemplate<MeasureSon, T>* protocolSendControl(){ return _protocolSendControl; }

    inline int measureSonType() const { return _measureSonType; }

    inline void setMeasureSonType(int value) { _measureSonType = value; }

    inline T defaultToValue() const { return _defaultToValue; }

    inline void setDefaultToValue(const T& t) { _defaultToValue = t; }

    inline void holdOnAllContron(int interval) { _holdOnAllControlInterval = interval; }

protected:
    int cycleSendImpl() {
        if (_protocolSendControl) {
            if (_holdOnAllControlInterval <= 0) {
                return _protocolSendControl->sendAllControl(_defaultToValue);
            } else {
                _holdOnAllControlInterval -= sendInterval();
            }
        }
        return -1;
    }

    inline void setProtocol(IProtocolSendControlTemplate<MeasureSon, T>* oProtocol){ _protocolSendControl = oProtocol; }

    friend class ProtocolBase;

private:
    IProtocolSendControlTemplate<MeasureSon, T>* _protocolSendControl;
    int _measureSonType;
    T _defaultToValue;
    int _holdOnAllControlInterval;

};

#endif // SEND_CONTROL_CYCLE_TEMPLATE_H
