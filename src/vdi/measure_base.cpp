#include "measure_base.h"





MeasureBase::MeasureBase(){
    _measureId = CxValueType::minValue<int>();
    _containerId = ci_defaultMeasureContainerId;
    _containerType = ci_defaultMeasureContainerId;
    _measurePos = CxValueType::minValue<int>();
    _measureName="";
    _property="";
    _url="";
}

MeasureBase::~MeasureBase(){
}

void MeasureBase::toContext(CxISetSkv& context) const
{
    context.setValue(CS_EntryMeasureId, _measureId);
    context.setValue(CS_EntryMeasureName, _measureName);
    context.setValue(CS_EntryContainerId, _containerId);
    context.setValue(CS_EntryContainerType, _containerType);
    context.setValue(CS_EntryProperty, _property);
    context.setValue(CS_EntryUrl, _url);
}

void MeasureBase::fromContext(const CxIGetSkv& context)
{
    _measureId = context.getValue(CS_EntryMeasureId, _measureId);
    _measureName = context.getValue(CS_EntryMeasureName, _measureName);
    _containerId = context.getValue(CS_EntryContainerId, _containerId);
    _containerType = context.getValue(CS_EntryContainerType, _containerType);
    _property = context.getValue(CS_EntryProperty, _property);
    _url = context.getValue(CS_EntryUrl, _url);
}

std::string MeasureBase::reportSelf()
{
    std::vector<std::string> sReports;
    sReports.push_back(std::string("MeasureId=" + CxString::toString(measureId())));
    sReports.push_back(std::string("MeasureName=" + CxString::toString(measureName())));
    sReports.push_back(std::string("ContainerId=" + CxString::toString(containerId())));
    sReports.push_back(std::string("ContainerType=" + CxString::toString(containerType())));
    sReports.push_back(std::string("Property=" +_property));
    sReports.push_back(std::string("Url=" + _url));

    toReport(sReports);
    return CxString::join( sReports , ' ');
}

void MeasureBase::toReport(std::vector<std::string> &sReports) const
{
}

//从缓冲区读入
int MeasureBase::fromBuffer(uchar *buffer, int len)
{
    if((buffer==NULL)||(len<size())) return GC_MEASURE_ERROR;
    int *v = (int*)buffer;
    _measureId  = *v++;
    _measurePos = *v;
    return size();
}
//输出到缓冲区
int MeasureBase::toBuffer(uchar *buffer, int len)
{
    if((buffer==NULL)||(len<size())) return GC_MEASURE_ERROR;
    int *v = (int*)buffer;
    *v++ = _measureId;
    *v   = _measurePos;
    return size();
}
