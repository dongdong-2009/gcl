#ifndef SEND_MEASURE_CYCLE_H
#define SEND_MEASURE_CYCLE_H


#include "cycle_base.h"
#include "send_measure_cases_template.h"


template<class MeasureSon, typename T>
class SendMeasureCycleTemplate : public CycleBase
{
public:
    SendMeasureCycleTemplate()
    {
        _protocolSendMeasure = NULL;
        _measureSonType = -1;
    }

    inline const IProtocolSendMeasureTemplate<MeasureSon, T>* protocolSendMeasure(){ return _protocolSendMeasure; }

    inline int measureSonType() const { return _measureSonType; }

    inline void setMeasureSonType(int value) { _measureSonType = value; }

protected:
    int cycleSendImpl()
    {
        if (_protocolSendMeasure) {
            if (_measureSonType > 0) {
                return _protocolSendMeasure->sendAllMeasure(_measureSonType);
            } else if (_measureSonType < 0) {
                int iSend = 0;
                const std::vector<int>& measureSonTypes = MeasureSon::measureSonTypes();
                for (size_t i = 0; i < measureSonTypes.size(); ++i) {
                    int iMeasureSonType = measureSonTypes.at(i);
                    iSend += _protocolSendMeasure->sendAllMeasure(iMeasureSonType);
                }
                return iSend;
            }
        }
        return -1;
    }

    inline void setProtocol(IProtocolSendMeasureTemplate<MeasureSon, T>* oProtocol){ _protocolSendMeasure = oProtocol; }

    friend class ProtocolBase;

private:
    IProtocolSendMeasureTemplate<MeasureSon, T>* _protocolSendMeasure;
    int _measureSonType;

};

#endif // SEND_MEASURE_CYCLE_H
