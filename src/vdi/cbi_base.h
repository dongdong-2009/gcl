#ifndef CBI_BASE_H
#define CBI_BASE_H


#include "protocol_global.h"
#include <ccxx/cxdescribe.h>
#include "protocol_base.h"

class CBIBase;

class CBIFactoryBase : public CxFactoryTemplate<CBIBase>
{

};


class GM_GCL_VDI_API CBIBase
{
public:
    static std::vector<CxFactoryTemplate<CBIBase> *> * factoriesContainer();

    static void factoriesCreateAndRegister();

public:
    CBIBase();

    virtual ~CBIBase();

    virtual const std::string& typeSimpleName() const = 0;

    virtual void toContext(CxISetSkv& context) const;

    virtual void fromContext(const CxIGetSkv& context);

    virtual void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    std::vector<std::string> reportSelf() const;

    inline const std::string& businessName() const { return _businessName; }
    inline void setBusinessName(const std::string& value) { _businessName = value; }

    inline int businessId() const { return _businessId; }
    inline void setBusinessId(int value) { _businessId = value; }

    virtual int sendCustomData(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData)=0;

    inline CxInterinfoOutStream outBusinessPrompt() {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Business); r.source(_businessId); r << CxGlobal::promptString << typeSimpleName() << " Business : "; return r;
    }

#if defined(GM_DEBUG)
    inline CxInterinfoOutStream outBusinessDebug() {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Business); r.source(_businessId); r << CxGlobal::debugString << typeSimpleName() << " Business : "; return r;
    }
#else
#undef outBusinessDebug
    inline CxInterinfoNoOutStream outBusinessDebug() { return CxInterinfoNoOutStream(); }
#define CX_NO_outBusinessDebug_MACRO while (false) outBusinessDebug
#define outBusinessDebug CX_NO_outBusinessDebug_MACRO
#endif

    void start();

    void stop();

protected:
    virtual void toReport(std::vector<std::string>& sReports) const {}

    virtual void businessAfterStart() {}

    virtual void businessAfterStop() {}

protected:
    inline CxInterinfoOutStream outPromptInfo(int iReason = 0) {CxInterinfoOutStream r; r.noend(); r.reason(iReason); r << CxGlobal::promptString << typeSimpleName() << "Business : "; return r;}

protected:
    std::string _businessName;
    int _businessId;
};


class CBIFactoryManager : public CxFactoryManagerTemplate<CBIBase>
{

};


class GM_GCL_VDI_API CBIManager
{
public:
    static int load();
    static CBIBase *business(const int &Id);
private:
    static CBIBase * loadBusiness(const std::string &sName, const std::string &sTypeSimpleName, const std::string& sFilePath);
};


#endif // CBI_BASE_H
