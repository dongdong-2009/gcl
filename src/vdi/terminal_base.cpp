#include "terminal_base.h"



TerminalBase::TerminalBase()
{
    _terminalId = 0;
    _terminalName = "";
    _protocol = NULL;
    _business = NULL;
}



TerminalBase::~TerminalBase()
{
    if (_business)
    {
        delete _business;
    }
    if (_protocol)
    {
        delete _protocol;
    }
}

void TerminalBase::setTerminalId(int value)
{
    _terminalId = value;
    if(_protocol)_protocol->setTerminalId(value);
}

void TerminalBase::toContext(CxISetSkv& context) const
{
    if (_protocol) {
        std::string sProtocolTypeSimpleName = _protocol->typeSimpleName();
        context.setValue(CS_EntryProtocol, sProtocolTypeSimpleName);
    }
    if (_business) {
        std::string sBusinessTypeSimpleName = _business->typeSimpleName();
        context.setValue(CS_EntryBusiness, sBusinessTypeSimpleName);
    }
    context.setValue(CS_EntryTerminalId, _terminalId);
}


void TerminalBase::fromContext(const CxIGetSkv& context)
{
     setTerminalId( context.getValue(CS_EntryTerminalId, _terminalId) );
}

std::vector<std::string> TerminalBase::reportSelf() const
{
    std::vector<std::string> sReports;
    sReports.push_back("*** terminal Report ***");
    sReports.push_back(std::string("TypeSimpleName=" + CxString::toString(typeSimpleName())));
    sReports.push_back(std::string("TerminalName=" + CxString::toString(_terminalName)));
    sReports.push_back(std::string("TerminalId=" + CxString::toString(_terminalId)));
    toReport(sReports);
    if (_protocol)
    {
        CxContainer::append(sReports, _protocol->reportSelf());
    }
    if (_business)
    {
        CxContainer::append(sReports, _business->reportSelf());
    }
    return sReports;
}

void TerminalBase::toReport(std::vector<std::string> &sReports) const
{

}

