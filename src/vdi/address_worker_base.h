#ifndef ADDRESS_WORKER_BASE_H
#define ADDRESS_WORKER_BASE_H

#include "protocol_global.h"

#include "worker_base.h"

#include "vdi_declspec.h"

struct AddressPort
{
    int address;
    int port;
};

inline bool operator==(AddressPort c1, AddressPort c2) { return c1.address == c2.address && c1.port == c2.port; }
inline bool operator< (AddressPort c1, AddressPort c2) { return c1.address <  c2.address || ( c1.address == c2.address && c1.port <  c2.port); }
inline uint qHash(AddressPort key) { return uint(( key.address << 8 ) + key.port); }

struct StationAddressPort
{
    int stationId;
    int address;
    int port;
};


class GM_GCL_VDI_API AddressWorkerBase : public WorkerBase
{

    GM_TYPEFULLNAME(WorkerBase, WorkerStrings::address())

public:
    AddressWorkerBase();

    ~AddressWorkerBase();

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    inline const int& address() const { return _address; }
    inline void setAddress(int value){ _address = value; }

    inline const int& port() const { return _port; }
    inline void setPort(int value){ _port = value; }

    inline const int& stationId() const { return _stationId; }
    inline void setStationId(int value){ _stationId = value; }

    const std::string& stationName() const { return _stationName; }
    inline void setStationName(const std::string& value){ _stationName = value; }

    template<class AddressWorkerSon>
    static AddressWorkerSon* addressWorker(const std::vector<AddressWorkerSon*>* oList, int iAddress){
        GM_PLISTO_FIND(oList, AddressWorkerSon, address(), iAddress)
    }

    template<class AddressWorkerSon>
    static AddressWorkerSon* addressWorker(const std::vector<AddressWorkerSon*>* oList, int iAddress, int iPort){
        for (size_t i = 0; i < oList->size(); ++i){
            AddressWorkerSon* o = oList->at(i);
            if ( o->address() == iAddress && o->port() == iPort){
                return o;
            }
        }
        return NULL;
    }

    static bool lessThan(const AddressWorkerBase * worker1, const AddressWorkerBase * worker2)
    {
        return worker2->address() > worker1->address();
    }

protected:
    virtual void toReport(std::vector<std::string>& sReports) const;

    virtual void toDescribe(CxDescribeUnit& describes);

private:
    int _address;
    int _port;
    int _stationId;
    std::string _stationName;

};


template<class AddressWorkerSon>
class AddressWorkerTableTemplate : public WorkerTableTemplate<AddressWorkerSon>
{
public:
    AddressWorkerTableTemplate()
    {
        _maxValidAddress = 15000;
        _minValidAddress = 0;
        _addressIsHexDisplay = false;
        _maxValidPort = 255;
        _minValidPort = 0;
        _portIsHexDisplay = false;
        _maxAddress = -1;
        _minAddress = -1;
        _maxAddressWorker = NULL;
        _minAddressWorker = NULL;
        _isSingleAddress = false;
        _isContinueAddress = false;
        _addressCount = 0;
    }

    void describeSelf(CxDescribeUnit* oDescribeUnit) const {
        WorkerTableTemplate<AddressWorkerSon>::describeSelf(oDescribeUnit);
        CxDescribeInteger* oWorkerAddressDescribe    = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryWorkerAddress);   oWorkerAddressDescribe->initValue(-1);     oWorkerAddressDescribe->setMinValue(_minValidAddress); oWorkerAddressDescribe->setMaxValue(_maxValidAddress); oWorkerAddressDescribe->SetIsHexDisplay(_addressIsHexDisplay);
        CxDescribeInteger* oWorkerPortDescribe       = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryWorkerPort);      oWorkerPortDescribe->initValue(0);         oWorkerPortDescribe->setMinValue(_minValidPort); oWorkerPortDescribe->setMaxValue(_maxValidPort); oWorkerPortDescribe->SetIsHexDisplay(_portIsHexDisplay);
        CxDescribeInteger* oWorkerStationIdDescribe  = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryWorkerStationId); oWorkerStationIdDescribe->initValue(1);    oWorkerStationIdDescribe->setMinValue(CxValueType::minValue<ushort>()); oWorkerStationIdDescribe->setMaxValue(CxValueType::maxValue<ushort>());
        CxDescribeString*  oWorkerStationNameDescribe= oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryWorkerStationName);oWorkerStationNameDescribe->initValue("");
    }

    inline const int& maxValidAddress() const { return _maxValidAddress; }
    inline void setMaxValidAddress(int value) { _maxValidAddress = value; }

    inline const int& minValidAddress() const { return _minValidAddress; }
    inline void setMinValidAddress(int value) { _minValidAddress = value; }

    inline const bool& addressIsHexDisplay() const { return _addressIsHexDisplay; }
    inline void setAddressIsHexDisplay(bool value) { _addressIsHexDisplay = value; }

    inline const int& maxValidPort() const { return _maxValidPort; }
    inline void setMaxValidPort(int value) { _maxValidPort = value; }

    inline const int& minValidPort() const { return _minValidPort; }
    inline void setMinValidPort(int value) { _minValidPort = value; }

    inline const bool& portIsHexDisplay() const { return _portIsHexDisplay; }
    inline void setPortIsHexDisplay(bool value) { _portIsHexDisplay = value; }

    inline const int& maxAddress() const { return _maxAddress; }

    inline const int& minAddress() const { return _minAddress; }

    inline const AddressWorkerBase* maxAddressWorker() const { return _maxAddressWorker; }

    inline const AddressWorkerBase* minAddressWorker() const { return _minAddressWorker; }

    inline bool isSingleAddress() const { return _isSingleAddress; }

    inline bool isContinueAddress() const { return _isContinueAddress; }

    inline int addressCount() const { return _addressCount; }

    AddressWorkerSon* workerByAddress(const int& iAddress) const
    {
        const std::vector<AddressWorkerSon*>* oWorkers = WorkerTableTemplate<AddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            AddressWorkerSon* oWorker = oWorkers->at(i);
            if (oWorker->address() == iAddress){
                return oWorker;
            }
        }
        return NULL;
    }

    AddressWorkerSon* workerByAddressPort(const int& iAddress, const int& iPort) const
    {
        const std::vector<AddressWorkerSon*>* oWorkers = WorkerTableTemplate<AddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            AddressWorkerSon* oWorker = oWorkers->at(i);
            if (oWorker->address() == iAddress && oWorker->port() == iPort){
                return oWorker;
            }
        }
        return NULL;
    }

    AddressWorkerSon* workerByStationIdAddress(const int& iStationId, const int& iAddress) const
    {
        const std::vector<AddressWorkerSon*>* oWorkers = WorkerTableTemplate<AddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            AddressWorkerSon* oWorker = oWorkers->at(i);
            if (oWorker->stationId() == iStationId && oWorker->address() == iAddress)
            {
                return oWorker;
            }
        }
        return NULL;
    }

    AddressWorkerSon* workerByStationIdCode(const int& iStationId, const std::string & sWorkerCode) const
    {
        const std::vector<AddressWorkerSon*>* oWorkers = WorkerTableTemplate<AddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            AddressWorkerSon* oWorker = oWorkers->at(i);
            if (oWorker->stationId() == iStationId && oWorker->workerCode() == sWorkerCode)
            {
                return oWorker;
            }
        }
        return NULL;
    }

    std::vector<AddressWorkerSon*> workerByStationId(const int& iStationId) const
    {
        std::vector<AddressWorkerSon*> workers2;
        const std::vector<AddressWorkerSon*>* oWorkers = WorkerTableTemplate<AddressWorkerSon>::workers();
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            AddressWorkerSon* oWorker = oWorkers->at(i);
            if (oWorker->stationId() == iStationId){
                workers2.push_back(oWorker);
            }
        }
        return workers2;
    }

    void refreshStatisticses()
    {
        const std::vector<AddressWorkerSon*>* oAddressWorkerSons = WorkerTableTemplate<AddressWorkerSon>::workers();
        _maxAddress = CxValueType::minValue<int>();
        _minAddress = CxValueType::maxValue<int>();
        _maxAddressWorker = NULL;
        _minAddressWorker = NULL;
        _isSingleAddress = true;
        _stationIdWorkers.clear();
        std::map<int, int> addressCounts;
        for (size_t i = 0; i < oAddressWorkerSons->size(); ++i) {
            AddressWorkerSon* oWorker = oAddressWorkerSons->at(i);
            int iAddress = oWorker->address();
            if (iAddress > _maxAddress) {
                _maxAddress = iAddress;
                _maxAddressWorker = oWorker;
            }
            if (iAddress < _minAddress) {
                _minAddress = iAddress;
                _minAddressWorker = oWorker;
            }
            if (addressCounts.find(iAddress) != addressCounts.end())
            {
                _isSingleAddress = false;
                int iCount = addressCounts[iAddress];
                ++iCount;
                addressCounts[iAddress] = iCount;
            } else {
                addressCounts[iAddress] = iAddress;
            }
            int iStationId = oWorker->stationId();
            if (_stationIdWorkers.find(iStationId) != _stationIdWorkers.end())
            {
                std::vector<AddressWorkerSon*>& addressWorkers = _stationIdWorkers[iStationId];
                addressWorkers.push_back(oWorker);
            }
            else
            {
                std::vector<AddressWorkerSon*> addressWorkers;
                addressWorkers.push_back(oWorker);
                _stationIdWorkers[iStationId] = addressWorkers;
            }
        }
        _addressCount = addressCounts.size();
        _isContinueAddress = (_maxAddress - _minAddress == _addressCount - 1);
        if (_minAddress == CxValueType::maxValue<int>())
        {
            _minAddress = -1;
        }
        _stationIds = CxContainer::keys(_stationIdWorkers);
        _stationIdMins.clear();
        _stationIdMaxes.clear();
        _stationIdIsContinueAddresses.clear();
        for (size_t i = 0; i < _stationIds.size(); ++i)
        {
            int iStationId = _stationIds.at(i);
            std::vector<AddressWorkerSon*>& addressWorkers = _stationIdWorkers[iStationId];
            int maxAddress = CxValueType::minValue<int>();
            int minAddress = CxValueType::maxValue<int>();
            for (size_t j = 0; j < addressWorkers.size(); ++j)
            {
                AddressWorkerSon* oWorker = addressWorkers.at(j);
                if (oWorker->address() > maxAddress)
                {
                    maxAddress = oWorker->address();
                }
                if (oWorker->address() < minAddress)
                {
                    minAddress = oWorker->address();
                }
            }
            bool isContinueAddress = ((maxAddress - minAddress) == (addressWorkers.size() - 1));
            _stationIdMins[iStationId] = minAddress;
            _stationIdMaxes[iStationId] = maxAddress;
            _stationIdIsContinueAddresses[iStationId] = isContinueAddress;
        }

    }

    const std::map<int, std::vector<AddressWorkerSon*> >& stationIdWorkers() const { return _stationIdWorkers; }

    const std::vector<int>& stationIds() const { return _stationIds; }

    const std::map<int, int>& stationIdMins() const { return _stationIdMins; }

    const std::map<int, int>& stationIdMaxes() const { return _stationIdMaxes; }

    const std::map<int, bool>& stationIdIsContinueAddresses() const { return _stationIdIsContinueAddresses; }

protected:

private:
    int _maxValidAddress;
    int _minValidAddress;
    bool _addressIsHexDisplay;
    int _maxValidPort;
    int _minValidPort;
    bool _portIsHexDisplay;
    int _maxAddress;
    int _minAddress;
    const AddressWorkerBase* _maxAddressWorker;
    const AddressWorkerBase* _minAddressWorker;
    bool _isSingleAddress;
    bool _isContinueAddress;
    int _addressCount;
    std::vector<int> _stationIds;
    std::map<int, std::vector<AddressWorkerSon*> > _stationIdWorkers;
    std::map<int, int> _stationIdMins;
    std::map<int, int> _stationIdMaxes;
    std::map<int, bool> _stationIdIsContinueAddresses;

};

#endif // !defined(ADDRESS_WORKER_BASE_H)
