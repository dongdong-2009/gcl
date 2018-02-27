#ifndef MONSB_ADDRESS_WORKER_H
#define MONSB_ADDRESS_WORKER_H

#include "protocol_global.h"


#include "measure_address_worker.h"
#include "monsb_measure.h"
#include "monsb_control_task.h"

class MonsbAddressWorkerTable;

class MonsbAddressWorker : public MeasureAddressWorkerTemplate<MonsbMeasure, int>
{

    GM_TYPEFULLNAME(AddressWorkerBase, WorkerStrings::Monsb())

};


class GM_GCL_VDI_API MonsbAddressWorkerTable : public MeasureAddressWorkerTableTemplate<MonsbAddressWorker, MonsbMeasure, int>
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
        MeasureAddressWorkerTableTemplate<MonsbAddressWorker, MonsbMeasure, int>::describeSelf(oDescribeUnit);
        const std::vector<MonsbMeasure*>* oMonsbs;
        std::vector<MonsbMeasure*> monsbs;
        if (monsbType() == MonsbType_0)
            oMonsbs = MonsbManager::measures();
        else {
            monsbs = MonsbManager::measures(monsbType());
            oMonsbs = &monsbs;
        }
        std::map<std::string, std::string> foreignKeyValues;
        for (size_t i = 0; i < oMonsbs->size(); ++i) {
            MonsbMeasure* oMonsb = oMonsbs->at(i);
            foreignKeyValues[CxString::toString((oMonsb->measureId()))] = oMonsb->measureName();
        }
        oDescribeUnit->setForeignKeyValues(foreignKeyValues);
        oDescribeUnit->setLocalKeyName(CS_EntryMeasureId);
        oDescribeUnit->setLocalCaptionName(CS_EntryWorkerName);
        oDescribeUnit->setFuncDescribeDefaultValue(&describeDefaultValue);
    }

    inline MonsbTypeEnum monsbType() const {
        int iMeasureSonType = MeasureAddressWorkerTableTemplate<MonsbAddressWorker, MonsbMeasure, int>::measureSonType();
        return g_fn_monsbTypeEnum(iMeasureSonType);
    }
    inline void setMonsbType(MonsbTypeEnum eMonsbType) {
        MeasureAddressWorkerTableTemplate<MonsbAddressWorker, MonsbMeasure, int>::setMeasureSonType(eMonsbType);
    }

    MonsbControlTask* receiveControl(MonsbAddressWorker* oWorker, int fromValue, int toValue, int iSourceId){
        const MonsbMeasure* oMonsb = oWorker->measure();
        if (oMonsb){
            return MonsbControlTaskManager::receiveControl(oMonsb, fromValue, toValue, iSourceId);
        }
        return NULL;
    }

    MonsbControlTask* receiveControl(int iAddress, int fromValue, int toValue, int iSourceId){
        MonsbAddressWorker* oWorker = AddressWorkerTableTemplate<MonsbAddressWorker>::workerByAddress(iAddress);
        if (oWorker && oWorker->measure()){
            return MonsbControlTaskManager::receiveControl(oWorker->measure(), fromValue, toValue, iSourceId);
        }
        return NULL;
    }

    MonsbControlTask* receiveControl(int iAddress, int iSourceId){
        MonsbAddressWorker* oWorker = AddressWorkerTableTemplate<MonsbAddressWorker>::workerByAddress(iAddress);
        if (oWorker && oWorker->measure()){
            const MonsbMeasure* oMeasure = oWorker->measure();
            return MonsbControlTaskManager::receiveControl(oMeasure, oMeasure->value(), oMeasure->nextState(), iSourceId,iAddress);
        }
        return NULL;
    }
    //选择
    MonsbControlTask* receiveSelect(int iAddress, int fromValue, int toValue, int iSourceId){
        MonsbAddressWorker* oWorker = AddressWorkerTableTemplate<MonsbAddressWorker>::workerByAddress(iAddress);
        if (oWorker && oWorker->measure()){
            return MonsbControlTaskManager::receiveSelect(oWorker->measure(), fromValue, toValue, iSourceId,iAddress);
        }
        return NULL;
    }

};
#endif // !defined(MONSB_ADDRESS_WORKER_H)
