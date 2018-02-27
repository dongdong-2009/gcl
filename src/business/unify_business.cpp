#include "unify_business.h"


UnifyBusinessFactory* fn_regUnifyBusinessFactory()
{
    TBIFactoryManager::createAndRegister<UnifyBusinessFactory>();
    return NULL;
}

UnifyBusinessFactory* UnifyBusinessFactory::s_instance = fn_regUnifyBusinessFactory();


using namespace std;


UnifyBusiness::UnifyBusiness()
{
}

UnifyBusiness::~UnifyBusiness()
{
}

void UnifyBusiness::toContext(CxISetSkv &context) const
{
    PsmBusiness::toContext(context);

}

void UnifyBusiness::fromContext(const CxIGetSkv &context)
{
    PsmBusiness::fromContext(context);

}

void UnifyBusiness::toReport(std::vector<string> &sReports) const
{
    PsmBusiness::toReport(sReports);

}

void UnifyBusiness::businessAfterStart()
{
    PsmBusiness::businessAfterStart();

}

void UnifyBusiness::businessAfterStop()
{
    PsmBusiness::businessAfterStop();

}


//******************************************************************************************************

void UnifyBusiness::received_message_command(const PsmProtocol *oPsm, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach * oAttach)
{
    PsmBusiness::received_message_command(oPsm, pCommand, pParam, iParamLength, oAttach);
}

void UnifyBusiness::received_file_write(const PsmProtocol *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<GString> &fileData, const PsmAttach * oAttach)
{
    PsmBusiness::received_file_write(oPsm, pathInfo, fileData, oAttach);

}

void UnifyBusiness::msObjects_changed(const string &sTypeName, const std::vector<string> &sNames)
{
    PsmBusiness::msObjects_changed(sTypeName, sNames);

}
void UnifyBusiness::received_realtime_data_request(const PsmProtocol *oPsm, const PsmAttach *oAttach)
{
    PsmBusiness::received_realtime_data_request(oPsm,oAttach);

}

void UnifyBusiness::received_realtime_data_post(const PsmProtocol *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    PsmBusiness::received_realtime_data_post(oPsm,iParamType,pData,iDataLength,iParamCount,oAttach);
}

