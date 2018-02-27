#ifndef TBI_BASE_H
#define TBI_BASE_H


#include "protocol_global.h"
#include <ccxx/cxdescribe.h>
#include "protocol_base.h"

class TBIBase;

class TBIFactoryBase : public CxFactoryTemplate<TBIBase>
{

};

class GM_GCL_VDI_API TBIBase
{
public:
    static std::vector<CxFactoryTemplate<TBIBase> *> * factoriesContainer();

    static void factoriesCreateAndRegister();

public:
    TBIBase();

    virtual ~TBIBase();

    virtual const std::string& typeSimpleName() const = 0;

    virtual void toContext(CxISetSkv& context) const;

    virtual void fromContext(const CxIGetSkv& context);

    virtual void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    std::vector<std::string> reportSelf() const;

    inline const std::string& businessName() const { return _businessName; }
    inline void setBusinessName(const std::string& value) { _businessName = value; }

    inline int businessId() const { return _businessId; }
    inline void setBusinessId(int value) { _businessId = value; }

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

    inline const ProtocolBase* protocol() const { return _protocol; }
    void setProtocol(ProtocolBase* oProtocol);

    void start();

    void stop();

protected:
    virtual void toReport(std::vector<std::string>& sReports) const {}

    virtual void businessAfterStart() {}

    virtual void businessAfterStop() {}

    virtual void registerProtocol(ProtocolBase* oProtocol) {}

    virtual void unregisterProtocol() {}

protected:
    inline CxInterinfoOutStream outPromptInfo(int iReason = 0) {CxInterinfoOutStream r; r.noend(); r.reason(iReason); r << CxGlobal::promptString << typeSimpleName() << "Business : "; return r;}

protected:
    std::string _businessName;
    int _businessId;
    ProtocolBase * _protocol;

};


class TBIFactoryManager : public CxFactoryManagerTemplate<TBIBase>
{

};


class GM_GCL_VDI_API TBIManager
{
public:
    static int load();
    static TBIBase *business(const int &Id);
private:
    static TBIBase * loadBusiness(const std::string &sName, const std::string &sTypeSimpleName, const std::string& sFilePath);
};


#endif // TBI_BASE_H
