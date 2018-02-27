#ifndef MEASURE_ADDRESS_WORKER_H
#define MEASURE_ADDRESS_WORKER_H

#include "protocol_global.h"

#include "address_worker_base.h"
#include "measure_manager_template.h"


template<class MeasureSon, typename T>
class MeasureAddressWorkerTemplate : public AddressWorkerBase
{
public:
    MeasureAddressWorkerTemplate()
    {
        _defaultValue = CxValueType::originalValue<T>();
        _value = _defaultValue;
        _lastChangeSourceId = -1;
        _lastChangeReasonId = -1;
        _lastChangeTime = 0;
        _lastRefreshTime = 0;
        _outLineInterval = 0;
        _measure = NULL;
    }

    ~MeasureAddressWorkerTemplate()
    {

    }

    void toContext(CxISetSkv& context) const
    {
        AddressWorkerBase::toContext(context);
        int iMeasureId = CxValueType::maxValue<int>();
        if (_measure) { iMeasureId = _measure->measureId(); }
        context.setValue(CS_EntryMeasureId, iMeasureId);
    }
    //From 上下文关联
    void fromContext(const CxIGetSkv& context)
    {
        AddressWorkerBase::fromContext(context);
        int iMeasureId = CxValueType::maxValue<int>();
        iMeasureId = context.getValue(CS_EntryMeasureId, iMeasureId);
        _measure = MeasureManagerTemplate<MeasureSon, T>::measure(iMeasureId);
    }

    inline const T& defaultValue() const { return _defaultValue; }
    inline void setDefaultValue(const T& value) { _defaultValue = value; }

    inline const T& value() const { return _value; }
    //最后更改源ID
    inline const int& lastChangeSourceId() const { return _lastChangeSourceId; }
    //最后更改源ID
    inline const int& lastChangeReasonId() const { return _lastChangeReasonId; }
    //最后变化时间
    inline const msepoch_t& LastChangedTime() const { return _lastChangeTime; }
    //最后刷新时间
    inline const msepoch_t& lastRefreshTime() const { return _lastRefreshTime; }
    //掉线间隔
    inline const int& outLineInterval() const { return _outLineInterval; }
    inline void setOutLineInterval(const int& value) { _outLineInterval = value; }
    //是否在线
    inline bool isOnLine() const { return (_outLineInterval > 0 && CxTime::milliSecondDifferToNow(_lastRefreshTime) > _outLineInterval) ? false : true ; }
    //有效值
    inline const T& validValue() const { return isOnLine() ? _value : _defaultValue ; }

    inline const MeasureSon* measure() const { return _measure; }
    inline void setMeasure(MeasureSon* value){ _measure = value; }

    //是否有效的测量值
    inline bool isValidMeasureValue() const { return (_measure) ? _measure->isValidValue() : false; }

    virtual T toMeasureValue(const T& value) const { return value; }

    virtual T getMeasureValue() const { return (_measure) ? _measure->value() : _value; }

protected:
    //设置值
    inline void setValue(const T& workerValue, const T& measureValue, int iSourceId = -1, int iReason = -1)
    {
        _lastRefreshTime = CxTime::currentSystemTime();
        if (_value != workerValue)
        {
            _value = workerValue;
            _lastChangeReasonId = iReason;
            _lastChangeSourceId = iSourceId;
            _lastChangeTime = CxTime::currentSystemTime();
        }
        if (_measure)
        {
            T value = toMeasureValue(measureValue);
            MeasureManagerTemplate<MeasureSon, T>::receiveValue(_measure, value, iSourceId, iReason);
        }
    }

    template<class, class, typename> friend class MeasureAddressWorkerTableTemplate;

private:
    T _defaultValue;
    T _value;
    int _lastChangeSourceId;
    int _lastChangeReasonId;
    msepoch_t _lastChangeTime;
    msepoch_t _lastRefreshTime;
    int _outLineInterval;
    MeasureSon* _measure;

};


template<class MeasureAddressWorkerSon, class MeasureSon, typename T>
class MeasureAddressWorkerTableTemplate : public AddressWorkerTableTemplate<MeasureAddressWorkerSon>
{
public:
    MeasureAddressWorkerTableTemplate()
    {
        _measureSonType = -1;
    }

    void describeSelf(CxDescribeUnit* oDescribeUnit) const
    {
        AddressWorkerTableTemplate<MeasureAddressWorkerSon>::describeSelf(oDescribeUnit);
        CxDescribeInteger* oWorkerMeasureIdDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryMeasureId, 0);
        oWorkerMeasureIdDescribe->setIsReadOnly(true);
    }

    inline int measureSonType() const { return _measureSonType; }

    inline void setMeasureSonType(int value) { _measureSonType = value; }

    bool isOnlineAll(int iOutLineInterval) const
    {
        const std::vector<MeasureAddressWorkerSon*>* oWorkers = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            if (CxTime::milliSecondDifferToNow(oWorker->lastRefreshTime()) > iOutLineInterval)
                return false;
        }
        return true;
    }

    bool isOnlineOne(int iOutLineInterval) const
    {
        const std::vector<MeasureAddressWorkerSon*>* oWorkers = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            if (CxTime::milliSecondDifferToNow(oWorker->lastRefreshTime()) <= iOutLineInterval)
                return true;
        }
        return false;
    }

    //needtodo test delete "AddressWorkerTableTemplate<MeasureAddressWorkerSon>::"
    MeasureAddressWorkerSon* workerByMeasure(const MeasureSon* oMeasure) const
    {
        const std::vector<MeasureAddressWorkerSon*>* oWorkers = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            if (oWorker->measure() == oMeasure)
                return oWorker;
        }
        return NULL;
    }

    MeasureAddressWorkerSon* workerById(int nID) const
    {
        const std::vector<MeasureAddressWorkerSon*>* oWorkers = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            if(oWorker==NULL) continue;

            MeasureSon* oSon = (MeasureSon*)(oWorker->measure());
            if(oSon!=NULL && oSon->measureId() == nID)
            {
                return oWorker;
            }
        }
        return NULL;
    }

    MeasureSon *measureSonByAddress(int nAddress) const
    {

        const std::vector<MeasureAddressWorkerSon*>* oWorkers = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            if(oWorker->address()== nAddress)
            {
                return (MeasureSon*)(oWorker->measure());
            }
        }
        return NULL;
    }

    MeasureSon *MeasureSonByMeasID(int nMeasID) const
    {
        const std::vector<MeasureAddressWorkerSon*>* oWorkers = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            if(oWorker==NULL) continue;

            MeasureSon* oSon = (MeasureSon*)(oWorker->measure());
            if(oSon!=NULL && oSon->measureId() == nMeasID)
            {
                return oSon;
            }
        }
        return NULL;
    }
    int addressByMeasID(int nMeasID) const
    {
        const std::vector<MeasureAddressWorkerSon*>* oWorkers = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            if(oWorker==NULL) continue;

            MeasureSon* oSon = (MeasureSon*)(oWorker->measure());
            if(oSon!=NULL && oSon->measureId() == nMeasID)
            {
                return oWorker->address();
            }
        }
        return -1;
    }

    int measuresBeforeDelete(const std::vector<const MeasureSon*>& measures)
    {
        const std::vector<MeasureAddressWorkerSon*>* oWorkers = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        std::vector<MeasureAddressWorkerSon*> workerDeletes;
        for (int i = oWorkers->size()-1; i >= 0; --i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            if (std::find( measures.begin(), measures.end(), oWorker->measure()) != measures.end())
                workerDeletes.push_back(oWorker);
///std::vector<const MeasureSon*>  and  std::vector<MeasureSon*>  differ :
//            for (int j = 0; j < measures.count(); ++j) {
//                if (measures.at(j) == oWorker->measure()) {
//                    workerDeletes.push_back(oWorker);
//                    break;
//                }
//            }
        }
        AddressWorkerTableTemplate<MeasureAddressWorkerSon>::deleteWorkers(workerDeletes);
        return workerDeletes.size();
    }

    int validMeasureValueCount() const
    {
        int count = 0;
        const std::vector<MeasureAddressWorkerSon*>* oWorkers = WorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            if (oWorker->isValidMeasureValue())
            {
                count ++;
            }
        }
        return count;
    }

    std::map<int, T> addressValues(bool bIsMeasureValue = true) const
    {
        std::map<int, T>  addressValues;
        const std::vector<MeasureAddressWorkerSon*>* oWorkers = WorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            int iAddress = oWorker->address();
            T tWorkerValue = oWorker->value();
            if (bIsMeasureValue && oWorker->measure())
            {
                tWorkerValue = oWorker->measure()->value();
            }
            addressValues[iAddress] = tWorkerValue;
        }
        return addressValues;
    }

    std::map<int, T> addressValues(T value) const
    {
        std::map<int, T>  addressValues;
        const std::vector<MeasureAddressWorkerSon*>* oWorkers = WorkerTableTemplate<MeasureAddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MeasureAddressWorkerSon* oWorker = oWorkers->at(i);
            addressValues[oWorker->address()] = value;
        }
        return addressValues;
    }

    std::map<int, T> addressValues(const std::map<const MeasureSon*, T>& measureValues , bool bUseMeasureValue = false, int* pStationId = NULL) const
    {
        std::map<int, T>  addressValues;
        int iStationId = -2;
        for (typename std::map<const MeasureSon*, T>::const_iterator it = measureValues.begin(); it != measureValues.end(); ++it)
        {
//        QHashIterator<const MeasureSon*, T> it(measureValues);
//        while (it.hasNext())
//        {
//            it.next();
            const MeasureSon* oMeasure = it->first;
            MeasureAddressWorkerSon* oWorker = workerByMeasure(oMeasure);
            if (oWorker)
            {
                int iAddress = oWorker->address();
                T vallue = ( bUseMeasureValue ) ? oWorker->getMeasureValue() : it->second;
                addressValues[iAddress] = vallue;
                if (iStationId == -2)
                {
                    iStationId = oWorker->stationId();
                }
                else
                {
                    if (iStationId != oWorker->stationId())
                    {
                        iStationId = -1;
                        if (pStationId) *pStationId = iStationId;
                        addressValues.clear();
                        return addressValues;
                    }
                }
            }
        }
        if (pStationId) *pStationId = iStationId;
        return addressValues;
    }

    void receiveWorkerValue(MeasureAddressWorkerSon* oWorker, T tWorkerValue, int iSourceId, int iReason = 0)
    {
        MeasureSon::beginSetValues(_measureSonType, iSourceId);
        T tMonsbValue = tWorkerValue;
        oWorker->setValue(tWorkerValue, tMonsbValue, iSourceId, iReason);
        MeasureSon::endSetValues();
    }

    void receiveWorkerValues(const std::map<MeasureAddressWorkerSon* , T>& workerValues, int iSourceId, int iReason = 0)
    {
        MeasureSon::beginSetValues(_measureSonType, iSourceId);
        for (typename std::map<MeasureAddressWorkerSon*, T>::const_iterator it = workerValues.begin(); it != workerValues.end(); ++it)
        {
            MeasureAddressWorkerSon* oWorker = it->first;
            T tWorkerValue = it->second;
            T tMonsbValue = tWorkerValue;
            oWorker->setValue(tWorkerValue, tMonsbValue, iSourceId, iReason);
        }
        MeasureSon::endSetValues();
    }

    void receiveAddressValues(const std::map<int , T>& addressValues, int iSourceId, int iReason = 0, bool bCreateIfNull = false)
    {
        if (bCreateIfNull)
        {
            std::vector<MeasureAddressWorkerSon*> workers;
            for (typename std::map<int , T>::const_iterator it = addressValues.begin(); it != addressValues.end(); ++it)
            {
//            QHashIterator<int, T> it(addressValues);
//            while (it.hasNext())
//            {
//                it.next();
                int iAddress = it->first;
                MeasureAddressWorkerSon* oWorker = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workerByAddress(iAddress);
                if (! oWorker)
                {
                    oWorker = new MeasureAddressWorkerSon();
                    oWorker->setAddress(iAddress);
                    workers.push_back(oWorker);
                }
            }
            WorkerTableTemplate<MeasureAddressWorkerSon>::appendWorkers(workers);
        };

        MeasureSon::beginSetValues(_measureSonType, iSourceId);
        for (typename std::map<int , T>::const_iterator it = addressValues.begin(); it != addressValues.end(); ++it)
        {
//        QHashIterator<int, T> it(addressValues);
//        while (it.hasNext())
//        {
//            it.next();
            int iAddress = it->first;
            MeasureAddressWorkerSon* oWorker = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workerByAddress(iAddress);
            if (oWorker)
            {
                T tWorkerValue = it->second;
                T tMeasureValue = tWorkerValue;
                oWorker->setValue(tWorkerValue, tMeasureValue, iSourceId, iReason);
            }
        }
        MeasureSon::endSetValues();
    }

    void receiveAddressValues(int iStationId, const std::map<int , T>& addressValues, int iSourceId, int iReason = 0, bool bCreateIfNull = false)
    {
        if (bCreateIfNull)
        {
            std::vector<MeasureAddressWorkerSon*> workers;
            for (typename std::map<int , T>::const_iterator it = addressValues.begin(); it != addressValues.end(); ++it)
            {
//            QHashIterator<int, T> it(addressValues);
//            while (it.hasNext())
//            {
//                it.next();
                int iAddress = it->first;
                MeasureAddressWorkerSon* oWorker = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workerByStationIdAddress(iStationId, iAddress);
                if (! oWorker)
                {
                    oWorker = new MeasureAddressWorkerSon();
                    oWorker->setStationId(iStationId);
                    oWorker->setAddress(iAddress);
                    workers.push_back(oWorker);
                }
            }
            WorkerTableTemplate<MeasureAddressWorkerSon>::appendWorkers(workers);
        };

        MeasureSon::beginSetValues(_measureSonType, iSourceId);
        for (typename std::map<int , T>::const_iterator it = addressValues.begin(); it != addressValues.end(); ++it)
        {
//        QHashIterator<int, T> it(addressValues);
//        while (it.hasNext())
//        {
//            it.next();
            int iAddress = it->first;
            MeasureAddressWorkerSon* oWorker = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workerByStationIdAddress(iStationId, iAddress);
            if (oWorker)
            {
                T tWorkerValue = it->second;
                T tMeasureValue = tWorkerValue;
                oWorker->setValue(tWorkerValue, tMeasureValue, iSourceId, iReason);
            }
        }
        MeasureSon::endSetValues();
    }

    void receiveAddressPortValues(const std::map<AddressPort , T>& addressPortValues, int iSourceId, int iReason = 0, bool bCreateIfNull = false)
    {
        if (bCreateIfNull)
        {
            std::vector<MeasureAddressWorkerSon*> workers;
            for (typename std::map<AddressPort , T>::const_iterator it = addressPortValues.begin(); it != addressPortValues.end(); ++it)
            {
//            QHashIterator<AddressPort, T> it(addressPortValues);
//            while (it.hasNext())
//            {
//                it.next();
                AddressPort rAddressPort = it->first;
                int iAddress = rAddressPort.address;
                int iPort = rAddressPort.port;
                MeasureAddressWorkerSon* oWorker = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workerByAddress(iAddress);
                if (! oWorker)
                {
                    oWorker = new MeasureAddressWorkerSon();
                    oWorker->setAddress(iAddress);
                    oWorker->setPort(iPort);
                    workers.push_back(oWorker);
                }
            }
            WorkerTableTemplate<MeasureAddressWorkerSon>::appendWorkers(workers);
        };


        MeasureSon::beginSetValues(_measureSonType, iSourceId);
        for (typename std::map<AddressPort , T>::const_iterator it = addressPortValues.begin(); it != addressPortValues.end(); ++it)
        {
//        QHashIterator<AddressPort, T> it(addressPortValues);
//        while (it.hasNext())
//        {
//            it.next();
            AddressPort rAddressPort = it->first;
            int iAddress = rAddressPort.address;
            int iPort = rAddressPort.port;
            T tWorkerValue = it->second;
            T tMeasureValue = tWorkerValue;
            MeasureAddressWorkerSon* oWorker = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workerByAddressPort(iAddress, iPort);
            if (oWorker)
            {
                oWorker->setValue(tWorkerValue, tMeasureValue, iSourceId, iReason);
            }
        }
        MeasureSon::endSetValues();
    }

    void receiveCodeValues(int iStationId, const std::map<std::string , T>& addressValues, int iSourceId, int iReason = 0, bool bCreateIfNull = false)
    {
        if (bCreateIfNull)
        {
            std::vector<MeasureAddressWorkerSon*> workers;
            for (typename std::map<std::string, T>::const_iterator it = addressValues.begin(); it != addressValues.end(); ++it)
            {
                std::string sWorkerCode = it->first;
                MeasureAddressWorkerSon* oWorker = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workerByStationIdCode(iStationId, sWorkerCode);
                if (! oWorker)
                {
                    oWorker = new MeasureAddressWorkerSon();
                    oWorker->setStationId(iStationId);
                    oWorker->setWorkerCode(sWorkerCode);
                    workers.push_back(oWorker);
                }
            }
            WorkerTableTemplate<MeasureAddressWorkerSon>::appendWorkers(workers);
        };

        MeasureSon::beginSetValues(_measureSonType, iSourceId);
        for (typename std::map<std::string, T>::const_iterator it = addressValues.begin(); it != addressValues.end(); ++it)
        {
            std::string sWorkerCode = it->first;
            MeasureAddressWorkerSon* oWorker = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workerByStationIdCode(iStationId, sWorkerCode);
            if (oWorker)
            {
                T tWorkerValue = it->second;
                T tMeasureValue = tWorkerValue;
                oWorker->setValue(tWorkerValue, tMeasureValue, iSourceId, iReason);
            }
        }
        MeasureSon::endSetValues();
    }

    void receiveCodeValues(const std::map<std::string, T>& addressValues, int iSourceId, int iReason = 0, bool bCreateIfNull = false)
    {
        if (bCreateIfNull)
        {
            std::vector<MeasureAddressWorkerSon*> workers;
            for (typename std::map<std::string, T>::const_iterator it = addressValues.begin(); it != addressValues.end(); ++it)
            {
                std::string sWorkerCode = it->first;
                MeasureAddressWorkerSon* oWorker = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workerByCode(sWorkerCode);
                if (! oWorker)
                {
                    oWorker = new MeasureAddressWorkerSon();
                    oWorker->setWorkerCode(sWorkerCode);
                    workers.push_back(oWorker);
                }
            }
            WorkerTableTemplate<MeasureAddressWorkerSon>::appendWorkers(workers);
        };

        MeasureSon::beginSetValues(_measureSonType, iSourceId);
        for (typename std::map<std::string, T>::const_iterator it = addressValues.begin(); it != addressValues.end(); ++it)
        {
            std::string sWorkerCode = it->first;
            MeasureAddressWorkerSon* oWorker = AddressWorkerTableTemplate<MeasureAddressWorkerSon>::workerByCode(sWorkerCode);
            if (oWorker)
            {
                T tWorkerValue = it->second;
                T tMeasureValue = tWorkerValue;
                oWorker->setValue(tWorkerValue, tMeasureValue, iSourceId, iReason);
            }
        }
        MeasureSon::endSetValues();
    }

private:
//    typedef MeasureManagerTemplate<MeasureSon, T> MeasureManager;

    int _measureSonType;

};
#endif // MEASURE_ADDRESS_WORKER_H
