#ifndef CICS_BUSINESS_H
#define CICS_BUSINESS_H


#include <business/psm_business.h>


class CicsBusiness : public PsmBusiness , public ICxTimerNotify
{
public:
    CicsBusiness();

    ~CicsBusiness();

    inline const std::string& typeSimpleName() const { return CS_BusinessTypeSimpleName_Cics; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv & context);

    void createHead(std::map<std::string, std::string> &sHeads);

    int sendSingleTemplateMessage(const std::string & sTemplate, const std::map<std::string, std::string> & sHeads);

    int sendSingleTemplateFile(const std::string & sTemplate, const std::map<std::string, std::string> & sHeads) {}

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

protected:
     void timer_timeOut(const CxTimer* oTimer);

private:
    int explain(const char * sParam, int iParamLength);

    int dealCicsXml(const std::string & sMsgNo, const std::map<std::string, std::string> & major, const std::map<std::string, std::map<std::string, std::string> > & details);

    int requestUserInfo();

    int requestPower();

private:
    int _queryInfoInterval;
    msepoch_t _syncUserInfoTime;
    msepoch_t _syncPowerTime;
    msepoch_t _queryUserInfoTime;
    msepoch_t _queryPowerTime;
    CxTimer * _queryInfoTm;

};



class CicsBusinessFactory : public TBIFactoryBase
{

    GM_SINGLETON_DECLARATION(CicsBusinessFactory)

public:
    std::string factoryName()
    {
        return CS_BusinessTypeSimpleName_Cics;
    }

    TBIBase* createObject()
    {
        return new CicsBusiness();
    }
};

#endif // CICS_BUSINESS_H
