#include "ycadd_address_worker.h"


YcaddAddressWorker::YcaddAddressWorker()
{
    _bianbi = 1;
}

YcaddAddressWorker::~YcaddAddressWorker()
{
}

void YcaddAddressWorker::toContext(CxISetSkv& context) const
{
    MeasureAddressWorkerTemplate<YcaddMeasure, double>::toContext(context);
    context.setValue(CS_EntryBianbi, _bianbi);
}

void YcaddAddressWorker::fromContext(const CxIGetSkv& context)
{
    MeasureAddressWorkerTemplate<YcaddMeasure, double>::fromContext(context);
    _bianbi = context.getValue(CS_EntryBianbi, _bianbi);
   //顺序
//    if(measure())
//    {
//        cxDebug()<<_bianbi<<measure()->measureId()<<cxEndLine;
//    }
}

