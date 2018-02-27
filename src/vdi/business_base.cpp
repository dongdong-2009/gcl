#include "business_base.h"




static std::vector<BusinessBase*> f_businesses;



BusinessBase::BusinessBase()
{
    _protocol = NULL;
}

BusinessBase::~BusinessBase()
{
    if (_protocol)
    {
        unregisterProtocol();
        _protocol = NULL;
    }
}

void BusinessBase::toContext(CxISetSkv &context) const
{
    context.setValue(CS_EntryBusinessName, _businessName);
}

void BusinessBase::fromContext(const CxIGetSkv &context)
{
    _businessName = context.getValue(CS_EntryBusinessName, _businessName);
}

void BusinessBase::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    assert(oDescribeUnit != NULL);
    CxDescribeString* oBusinessNameDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryBusinessName);
    oBusinessNameDescribe->setIsReadOnly(true);
    oBusinessNameDescribe->initValue(_businessName);
}

std::vector<std::string> BusinessBase::reportSelf() const
{
    std::vector<std::string> sReports;
    sReports.push_back("*** Business Report ***");
    sReports.push_back(std::string("typeSimpleName: " + CxString::toString(typeSimpleName())));
    toReport(sReports);
    return sReports;
}

void BusinessBase::setProtocol(ProtocolBase *oProtocol)
{
    unregisterProtocol();
    _protocol = oProtocol;
    registerProtocol(oProtocol);
}

void BusinessBase::start()
{
    businessAfterStart();
}

void BusinessBase::stop()
{
    businessAfterStop();
}


int BusinessManager::load()
{
    std::string sFilePath = CxFileSystem::mergeFilePath(GclConfig::businessPath(), "business.config.ini");
    CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
    if (oProfile)
    {
        std::string sNameTypes = oProfile->getValue(CS_SectionBusinesses, CS_EntryBusinessNameTypes, sNameTypes);
        sNameTypes = CxString::unquote(sNameTypes, '\"');
        std::vector<std::string> nameTypes = CxString::split( sNameTypes, ',' );
        for (size_t i = 0; i < nameTypes.size(); ++i)
        {
            std::string sNameType = nameTypes.at(i);
            //name=simplename
            std::string sName = CxString::token(sNameType, '=');
            std::string sTypeSimpleName = sNameType;
            std::string sFilePath = CxFileSystem::mergeFilePath(GclConfig::businessPath(), sName + ".ini");

            ///create business
            BusinessBase* oBusiness = BusinessFactoryManager::createObject(sTypeSimpleName);
            if (oBusiness)
            {
                oBusiness->setBusinessName(sName);
                CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
                if (oProfile)
                {
                    oProfile->prepareGet(sName);
                    oBusiness->fromContext(*oProfile);
                }
                f_businesses.push_back(oBusiness);
            }
        }
        CxSkverManager::endUse(oProfile);
    }
    return f_businesses.size();
}

BusinessBase *BusinessManager::loadBusiness(const std::string &sName, const std::string &sTypeSimpleName, const std::string &sFilePath)
{
    BusinessBase* oBusiness = BusinessFactoryManager::createObject(sTypeSimpleName);
    if (oBusiness)
    {
        cxPromptCheck1(CxFileSystem::isExist(sFilePath),,sFilePath);
        oBusiness->setBusinessName(sName);
        CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
        if (oProfile)
        {
            oProfile->prepareGet(CS_SectionBusiness);
            oBusiness->fromContext(*oProfile);
            CxSkverManager::endUse(oProfile);
        }
        f_businesses.push_back(oBusiness);
    }
    return oBusiness;
}

BusinessBase *BusinessManager::business(const int &Id)
{
    BusinessBase* oBusiness = NULL;
    for(int i=0;i<f_businesses.size();++i)
    {
        oBusiness = f_businesses.at(i);
        if(oBusiness==NULL) continue;
        if(oBusiness->businessId()==Id)break;
    }
    return oBusiness;
}
