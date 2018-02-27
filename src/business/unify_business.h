#ifndef UNIFY_BUSINESS_H
#define UNIFY_BUSINESS_H


#include <business/psm_business.h>


class UnifyBusiness : public PsmBusiness
{
public:
    UnifyBusiness();

    ~UnifyBusiness();

    inline const std::string& typeSimpleName() const { return CS_BusinessTypeSimpleName_Unify; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv & context);

protected:
    void toReport(std::vector<std::string>& sReports) const;

protected:
    void businessAfterStart();

    void businessAfterStop();

protected:
    void received_message_command(const PsmProtocol * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL);

    void received_file_write(const PsmProtocol * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL);

    void received_realtime_data_request(const PsmProtocol * oPsm, const PsmAttach * oAttach = NULL);

    void received_realtime_data_post(const PsmProtocol * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

protected:
    void msObjects_changed(const std::string & sTypeName, const std::vector<std::string> & sNames);

};



class UnifyBusinessFactory : public TBIFactoryBase
{

    GM_SINGLETON_DECLARATION(UnifyBusinessFactory)

public:
    std::string factoryName()
    {
        return CS_BusinessTypeSimpleName_Unify;
    }

    TBIBase* createObject()
    {
        return new UnifyBusiness();
    }
};


#endif // UNIFY_BUSINESS_H
