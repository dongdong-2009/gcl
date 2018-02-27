#include "worker_base.h"


std::string WorkerBase::reportSelf()
{
    std::vector<std::string> sReports;
    sReports.push_back("*** Worker Report ***");
    sReports.push_back(CS_EntryWorkerName+"="+_workerName);
    sReports.push_back(CS_EntryWorkerCode+"="+_workerCode);
    sReports.push_back(CS_EntryExtension+"="+_extension);
    toReport(sReports);
    return CxString::join(sReports);
}

void WorkerBase::toContext(CxISetSkv &context) const
{
    context.setValue(CS_EntryWorkerName, _workerName);
    context.setValue(CS_EntryWorkerCode, _workerCode);
    context.setValue(CS_EntryExtension, _extension);
}

void WorkerBase::fromContext(const CxIGetSkv &context)
{
    _workerName = context.getValue(CS_EntryWorkerName, _workerName);
    _workerCode = context.getValue(CS_EntryWorkerCode, _workerCode);
    _extension = context.getValue(CS_EntryExtension, _extension);
}



std::vector<std::string> WorkerTableBase::reportSelf()
{
    std::vector<std::string> sReports;
    sReports.push_back("*** Worker Table Report ***");
    sReports.push_back(std::string("TableName=" + CxString::toString(_tableName)));
    toReport(sReports);
    return sReports;
}

void WorkerTableBase::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    CxDescribeString* oWorkerNameDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryWorkerName); oWorkerNameDescribe->initValue("");
    CxDescribeString* oWorkerCodeDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryWorkerCode); oWorkerCodeDescribe->initValue("");
    CxDescribeString* oExtensionDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryExtension); oExtensionDescribe->initValue("");
}
