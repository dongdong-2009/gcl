#ifndef YCADD_ADDRESS_WORKER_H
#define YCADD_ADDRESS_WORKER_H

#include "protocol_global.h"


#include "measure_address_worker.h"
#include "ycadd_measure.h"


class YcaddAddressWorkerTable;

class GM_GCL_VDI_API YcaddAddressWorker : public MeasureAddressWorkerTemplate<YcaddMeasure, double>
{

    GM_TYPEFULLNAME(AddressWorkerBase, WorkerStrings::Ycadd())

public:
    YcaddAddressWorker();

    ~YcaddAddressWorker();

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    inline const double& bianbi() { return _bianbi; }
    inline void setBianbi(double value) { _bianbi = value; }

    inline double toMeasureValue(const double& value) const { return value * _bianbi; }

    inline double getMeasureValue() const {
        return (measure()) ? (measure()->value() * _bianbi) : (value() * _bianbi);
    }

protected:
    friend class YcaddAddressWorkerTable;

private:
    double _bianbi;

};


class GM_GCL_VDI_API YcaddAddressWorkerTable : public MeasureAddressWorkerTableTemplate<YcaddAddressWorker, YcaddMeasure, double>
{
public:
    static std::string describeDefaultValue(CxDescribeUnit* oDescribeUnit, const CxDescribeBase* oDescribe, int iRowIndex) {
        if (oDescribe->name() == CS_EntryWorkerAddress && oDescribe->typeSimpleName() == CxDescribeInteger::TYPESIMPLENAME()) {
            const CxDescribeInteger* oIntegerDescribe = reinterpret_cast<const CxDescribeInteger*>(oDescribe);
            int iAddress = oIntegerDescribe->minValue() + iRowIndex;
            return CxString::toString(iAddress);
        }
        return "";
    }

    void describeSelf(CxDescribeUnit* oDescribeUnit) const
    {
        MeasureAddressWorkerTableTemplate<YcaddAddressWorker, YcaddMeasure, double>::describeSelf(oDescribeUnit);
        const std::vector<YcaddMeasure*>* oYcadds;
        std::vector<YcaddMeasure*> ycadds;
        if (ycaddType() == YcaddType_0)
            oYcadds = YcaddManager::measures();
        else {
            ycadds = YcaddManager::measures(ycaddType());
            oYcadds = &ycadds;
        }
        std::map<std::string, std::string> foreignKeyValues;
        for (size_t i = 0; i < oYcadds->size(); ++i) {
            YcaddMeasure* oYcadd = oYcadds->at(i);
            foreignKeyValues[CxString::toString(oYcadd->measureId())] = oYcadd->measureName();
        }
        oDescribeUnit->setForeignKeyValues(foreignKeyValues);
        oDescribeUnit->setLocalKeyName(CS_EntryMeasureId);
        oDescribeUnit->setLocalCaptionName(CS_EntryWorkerName);
        oDescribeUnit->setFuncDescribeDefaultValue(&describeDefaultValue);
        CxDescribeDouble* oBianbiDescribe = oDescribeUnit->createAndRegister<CxDescribeDouble>(CS_EntryBianbi);
        oBianbiDescribe->initValue(1);
    }

    inline YcaddTypeEnum ycaddType() const
    {
        int iMeasureSonType = MeasureAddressWorkerTableTemplate<YcaddAddressWorker, YcaddMeasure, double>::measureSonType();
        return g_fn_ycaddTypeEnum(iMeasureSonType);
    }

    inline void setYcaddType(YcaddTypeEnum eYcaddType)
    {
        MeasureAddressWorkerTableTemplate<YcaddAddressWorker, YcaddMeasure, double>::setMeasureSonType(eYcaddType);
    }

};
#endif // !defined(YCADD_ADDRESS_WORKER_H)
