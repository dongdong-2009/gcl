#ifndef CASES_BASE_H
#define CASES_BASE_H

#include "protocol_global.h"

#include "vdi_declspec.h"

class GM_GCL_VDI_API CasesBase
{

    GM_TYPEFULLNAME_BASE(CS_CasesTypeSimpleName_Cases)

public:
    CasesBase();

    virtual ~CasesBase();

    void start();

    void stop();

    std::vector<std::string> reportSelf() const;

protected:
    virtual void toStart() {}

    virtual void toStop() {}

    virtual void toReport(std::vector<std::string>& sReports) const {}

};
#endif // !defined CASES_BASE_H
