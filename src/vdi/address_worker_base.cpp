#include "address_worker_base.h"


AddressWorkerBase::AddressWorkerBase()
{
    _address = -1;
    _port = -1;
    _stationId = -1;
    _stationName = "";
}

AddressWorkerBase::~AddressWorkerBase()
{

}

void AddressWorkerBase::toContext(CxISetSkv& context) const
{
    WorkerBase::toContext(context);
    context.setValue(CS_EntryWorkerAddress, _address);
    context.setValue(CS_EntryWorkerPort, _port);
    context.setValue(CS_EntryWorkerStationId, _stationId);
    context.setValue(CS_EntryWorkerStationName, _stationName);
}

void AddressWorkerBase::fromContext(const CxIGetSkv& context)
{
    WorkerBase::fromContext(context);
    _address = context.getValue(CS_EntryWorkerAddress, _address);
    _port = context.getValue(CS_EntryWorkerPort, _port);
    _stationId = context.getValue(CS_EntryWorkerStationId, _stationId);
    _stationName = context.getValue(CS_EntryWorkerStationName, _stationName);
}

void AddressWorkerBase::toReport(std::vector<std::string> &sReports) const
{
    sReports.push_back(CxString::format("Address=%d\nPort=%d\nStationId=%d\nStationName=%s", _address, _port, _stationId, _stationName.c_str()));
}

void AddressWorkerBase::toDescribe(CxDescribeUnit &describes)
{

}




