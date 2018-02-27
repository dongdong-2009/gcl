#ifndef STRAW_ADDRESS_WORKER_H
#define STRAW_ADDRESS_WORKER_H


#include "protocol_global.h"

#include "measure_address_worker.h"
#include "straw_measure.h"


class StrawAddressWorkerTable;

class StrawAddressWorker : public MeasureAddressWorkerTemplate<StrawMeasure, std::string>
{

    GM_TYPEFULLNAME(AddressWorkerBase, WorkerStrings::Straw())

};


class GM_GCL_VDI_API StrawAddressWorkerTable : public MeasureAddressWorkerTableTemplate<StrawAddressWorker, StrawMeasure, std::string>
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

    void describeSelf(CxDescribeUnit* oDescribeUnit) const {
        MeasureAddressWorkerTableTemplate<StrawAddressWorker, StrawMeasure, std::string>::describeSelf(oDescribeUnit);
        const std::vector<StrawMeasure*>* oStraws;
        std::vector<StrawMeasure*> straws;
        if (strawType() == StrawType_0)
            oStraws = StrawManager::measures();
        else {
            straws = StrawManager::measures(strawType());
            oStraws = &straws;
        }
        std::map<std::string, std::string> foreignKeyValues;
        for (size_t i = 0; i < oStraws->size(); ++i) {
            StrawMeasure* oStraw = oStraws->at(i);
            foreignKeyValues[CxString::toString((oStraw->measureId()))] = oStraw->measureName();
        }
        oDescribeUnit->setForeignKeyValues(foreignKeyValues);
        oDescribeUnit->setLocalKeyName(CS_EntryMeasureId);
        oDescribeUnit->setLocalCaptionName(CS_EntryWorkerName);
        oDescribeUnit->setFuncDescribeDefaultValue(&describeDefaultValue);
    }

    inline StrawTypeEnum strawType() const {
        int iMeasureSonType = MeasureAddressWorkerTableTemplate<StrawAddressWorker, StrawMeasure, std::string>::measureSonType();
        return g_fn_strawTypeEnum(iMeasureSonType);
    }
    inline void setStrawType(StrawTypeEnum eStrawType) {
        MeasureAddressWorkerTableTemplate<StrawAddressWorker, StrawMeasure, std::string>::setMeasureSonType(eStrawType);
    }

};

#endif // STRAW_ADDRESS_WORKER_H
