#include "cases_base.h"

CasesBase::CasesBase(){
}

CasesBase::~CasesBase(){

}

std::vector<std::string> CasesBase::reportSelf() const
{
    std::vector<std::string> sReports;
    sReports.push_back("*** Cases Report ***");
    sReports.push_back(std::string("TypeSimpleName=") + (typeSimpleName()));
    toReport(sReports);
    return sReports;
}

void CasesBase::start()
{
    toStart();
}

void CasesBase::stop()
{
    toStop();
}

