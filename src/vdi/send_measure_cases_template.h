#ifndef SEND_MEASURE_CASE_TEMPLATE_H
#define SEND_MEASURE_CASE_TEMPLATE_H

#include "cases_base.h"
#include "measure_manager_template.h"


struct SendMeasureArgvs
{
    int source;
    int reason;

    static SendMeasureArgvs NullArgs () {
        SendMeasureArgvs rResult;
        rResult.source = -1;
        rResult.reason = -1;
        return rResult;
    }
};


enum SendMeasureFailEnum
{
    SendMeasureFail_Unknow = -1,
    SendMeasureFail_IO_Invalid = -2,
    SendMeasureFail_IO_Send = -3,
    SendMeasureFail_Noprovide = -4,
    SendMeasureFail_Nomeasure = -5,
    SendMeasureFail_Exscope = -6
};


template<class MeasureSon, typename T>
class IProtocolSendMeasureTemplate
{
public:
    //single
    virtual int sendMeasure(const MeasureSon* oMeasure, T value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) = 0;
    //group
    virtual int sendMeasures(const std::map<const MeasureSon*, T>& measuresValue, int iMeasureSonType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) = 0;
    //all
    virtual int sendAllMeasure(int iMeasureSonType, T * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs()) = 0;

};


template<class MeasureSon, typename T>
class SendMeasureCasesTemplate : public CasesBase
{
    GM_TYPEFULLNAME(CasesBase, ProtocolStrings::SendMeasureTypeSimpleName<MeasureSon>())

public:

    SendMeasureCasesTemplate() { }

    ~SendMeasureCasesTemplate() { }

    inline const IProtocolSendMeasureTemplate<MeasureSon, T>* protocolSendMeasure(){ return _protocolSendMeasure; }

    int sendMeasure(const MeasureSon* oMeasure, T value)
    {
        return _protocolSendMeasure->sendMeasure(oMeasure, value);
    }

    int sendMeasure(int iMeasureId, T value)
    {
        const MeasureSon* oMeasure = MeasureManagerTemplate<MeasureSon, T>::measure(iMeasureId);
        if (oMeasure)
            return sendMeasure(oMeasure, value);
        else
            return -1;
    }

    int sendMeasure(const MeasureSon* oMeasure)
    {
        return sendMeasure(oMeasure, oMeasure->value());
    }

    int sendMeasure(int iMeasureId)
    {
        const MeasureSon* oMeasure = MeasureManagerTemplate<MeasureSon, T>::measure(iMeasureId);
        if (oMeasure)
            return sendMeasure(oMeasure);
        else
            return -1;
    }

    int sendMeasures(const std::map<const MeasureSon*, T>& measuresValue, int iMeasureSonType)
    {
        return _protocolSendMeasure->sendMeasures(measuresValue, iMeasureSonType);
    }

    int sendMeasures(const std::vector<int>& oMeasureIds)
    {
        std::map<const MeasureSon*, T> measureValues = MeasureSonManager::measureValues(oMeasureIds);
        return sendMeasures(measureValues);
    }

    int sendMeasures(const std::vector<const MeasureSon*>& measures)
    {
        std::map<const MeasureSon*, T> measureValues;
        for (size_t i = 0; i < measures.size(); ++i) {
            const MeasureSon* measure = measures.at(i);
            measureValues[measure] = measure->value();
        }
        return sendMeasures(measureValues);
    }

    int sendAllMeasure(int iMeasureSonType)
    {
        T * pt = NULL;
        return _protocolSendMeasure->sendAllMeasure(iMeasureSonType, pt);
    }

protected:
    typedef MeasureManagerTemplate<MeasureSon, T> MeasureSonManager;

    inline void setProtocol(IProtocolSendMeasureTemplate<MeasureSon, T>* oProtocol){ _protocolSendMeasure = oProtocol; }

    friend class ProtocolBase;

private:
    IProtocolSendMeasureTemplate<MeasureSon, T>* _protocolSendMeasure;

};

#endif // SEND_MEASURE_CASE_TEMPLATE_H
