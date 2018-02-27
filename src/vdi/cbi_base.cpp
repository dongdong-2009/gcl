#include "cbi_base.h"




static std::vector<CBIBase*> f_cbi;



CBIBase::CBIBase()
{
}

CBIBase::~CBIBase()
{
}

void CBIBase::toContext(CxISetSkv &context) const
{
    context.setValue(CS_EntryBusinessName, _businessName);
}

void CBIBase::fromContext(const CxIGetSkv &context)
{
    _businessName = context.getValue(CS_EntryBusinessName, _businessName);
}

void CBIBase::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    assert(oDescribeUnit != NULL);
    CxDescribeString* oBusinessNameDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryBusinessName);
    oBusinessNameDescribe->setIsReadOnly(true);
    oBusinessNameDescribe->initValue(_businessName);
}

std::vector<std::string> CBIBase::reportSelf() const
{
    std::vector<std::string> sReports;
    sReports.push_back("*** Business Report ***");
    sReports.push_back(std::string("typeSimpleName: " + CxString::toString(typeSimpleName())));
    toReport(sReports);
    return sReports;
}

void CBIBase::start()
{
    businessAfterStart();
}

void CBIBase::stop()
{
    businessAfterStop();
}


int CBIManager::load()
{
    return 0;
}

CBIBase *CBIManager::loadBusiness(const std::string &sName, const std::string &sTypeSimpleName, const std::string &sFilePath)
{
    CBIBase* oBusiness = CBIFactoryManager::createObject( sTypeSimpleName);
//    if (oBusiness)
//    {
//        cxPromptCheck1(CxFileSystem::isExist(sFilePath),,sFilePath);
//        oBusiness->setBusinessName(sName);
//        CxSkverBase * oProfile = CxSkverManager::beginUse(sFilePath);
//        if (oProfile)
//        {
//            oProfile->prepareGet(CS_SectionBusiness);
//            oBusiness->fromContext(*oProfile);
//            CxSkverManager::endUse(oProfile);
//        }
//        f_cbi.push_back(oBusiness);
//    }
    return oBusiness;
}

CBIBase *CBIManager::business(const int &Id)
{
    CBIBase* oBusiness = NULL;
    for(int i=0;i<f_cbi.size();++i)
    {
        oBusiness = f_cbi.at(i);
        if(oBusiness==NULL) continue;
        if(oBusiness->businessId()==Id)break;
    }
    return oBusiness;
}
