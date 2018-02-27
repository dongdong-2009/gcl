#ifndef BUSINESS_BASE_H
#define BUSINESS_BASE_H


#include "protocol_global.h"
#include <ccxx/cxdescribe.h>
#include "protocol_base.h"

class BusinessBase;

class BusinessConfigBase
{
public:
    BusinessConfigBase() {}

    virtual ~BusinessConfigBase() {}

    virtual const std::string& typeSimpleName() const = 0;

    virtual void toContext(CxISetSkv& context) const;

    virtual void fromContext(const CxIGetSkv& context);

    virtual void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    inline const std::string& businessName() const { return _businessName; }
    inline void setBusinessName(const std::string& value) { _businessName = value; }

    inline int businessId() const { return _businessId; }
    inline void setBusinessId(int value) { _businessId = value; }

private:
    std::string _businessName;
    int _businessId;
    std::string _businessTypeSimpleName;
    std::string _terminalName;

friend class BusinessBase;

};


class BusinessFactoryBase : public CxFactoryTemplate<BusinessBase>
{

};


class BusinessBase
{
public:
    static std::vector<CxFactoryTemplate<BusinessBase> *> * factoriesContainer();

    static void factoriesCreateAndRegister();

public:
    BusinessBase();

    virtual ~BusinessBase();

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

    virtual void registerProtocol(ProtocolBase* oProtocol) {}

    virtual void unregisterProtocol() {}

protected:
    inline CxInterinfoOutStream outPromptInfo(int iReason = 0) {CxInterinfoOutStream r; r.noend(); r.reason(iReason); r << CxGlobal::promptString << typeSimpleName() << "Business : "; return r;}

protected:
    std::string _businessName;
    int _businessId;


};


class BusinessFactoryManager : public CxFactoryManagerTemplate<BusinessBase>
{

};


class BusinessManager
{
public:
    static int load();
    static BusinessBase *business(const int &Id);
private:
    static BusinessBase * loadBusiness(const std::string &sName, const std::string &sTypeSimpleName, const std::string& sFilePath);



};


#endif // BUSINESS_BASE_H
