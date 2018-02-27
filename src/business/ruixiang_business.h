#ifndef RUIXIANG_BUSINESS_H
#define RUIXIANG_BUSINESS_H

#include <business/psm_business.h>

class RuixiangBusiness : public PsmBusiness
{
public:
    RuixiangBusiness();

    ~RuixiangBusiness();

    inline const std::string& typeSimpleName() const { return CS_BusinessTypeSimpleName_Ruixiang; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv & context);

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void businessAfterStart();

    void businessAfterStop();

    void received_message_command(const PsmProtocol * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL);

    void received_file_write(const PsmProtocol * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL);

    void msObjects_changed(const std::string & sTypeName, const std::vector<std::string> & sNames);

private:
    bool saveToFile(const std::vector<const CxMsObjectData *> & msObjects , const std::string & sFilePath);

    int explain(const std::vector<std::string> & fileData, const std::string & sFileName);

    bool sendFileToRuiXiang(const std::string &sTypeName, const std::vector<std::string> & sNames, const std::string &sFileName);

    bool sendMessageToRuiXiang(const std::string &sTypeName, const std::vector<std::string> & sNames);

private:
    std::string _deviceCode;
    std::string _terminalCode;
    std::map<std::string, std::string> _requestTypies;
    std::map<std::string, std::string> _keyTsValues;
    std::map<std::string, std::string> _typeFileNames;
    std::string _cardNumber;

};


class RuixiangBusinessFactory : public TBIFactoryBase
{

    GM_SINGLETON_DECLARATION(RuixiangBusinessFactory)

public:
    std::string factoryName()
    {
        return CS_BusinessTypeSimpleName_Ruixiang;
    }

    TBIBase* createObject()
    {
        return new RuixiangBusiness();
    }
};

#endif // RUIXIANG_BUSINESS_H
