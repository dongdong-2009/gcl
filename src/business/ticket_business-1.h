#ifndef TICKET_BUSINESS_H
#define TICKET_BUSINESS_H

#include <business/psm_business.h>
#include <ccxx/cxefile.h>

class TicketBusiness : public PsmBusiness
{
public:
    TicketBusiness();
    ~TicketBusiness();

    inline const std::string& typeSimpleName() const { return CS_BusinessTypeSimpleName_Ticket; }

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

private:
    int dealData(const PsmProtocol * oPsm,std::map<std::string, std::string> & declare, std::vector<EFileElement> & vObj,const PsmAttach * oAttach = NULL);

    int dealHead(const PsmProtocol * oPsm,std::map<std::string, std::string> & declare, EFileElement &obj,uint32 &functionCode,const PsmAttach * oAttach = NULL);

    int dealTicketMain(const PsmProtocol * oPsm,std::map<std::string, std::string> & declare, EFileElement &Obj,const PsmAttach * oAttach = NULL);

    int dealTicketDetail(const PsmProtocol * oPsm,std::map<std::string, std::string> & declare, EFileElement &Obj,const PsmAttach * oAttach = NULL);
private:
    //    int createDeclare(std::map<std::string, std::string> & declare);
    int createHead(int type,EFileElement &Head,uint32 &functionCode,const PsmAttach * oAttach = NULL);
    //    int createTicket(std::string sql)
    int createEFile();

    int respTickerReq(const PsmProtocol * oPsm, std::map<std::string, std::string> & declare, EFileElement &Head,const PsmAttach * oAttach = NULL);

    int postMessageCommand(const PsmProtocol * oPsm,std::string &sCommand,std::string &sPram,const PsmAttach * oAttach = NULL);


private:

    bool init();

    bool loadTemplateTable();

    bool loadTemplateField(std::vector<GCCommon::GCCommonObj> &vField,std::string &sName);

    bool loadTemplateField(std::vector<std::string> &vField,std::string &sName);

    std::vector<int> getColumnTypesByClassName(GString &sName,std::vector<std::string> &vObj);
private:
    std::map<std::string,std::vector<GCCommon::GCCommonObj> > _mapTableToFiled;
    std::map<std::string,std::vector<std::string> > _mapTableToFiledName;

    std::map<std::string, std::string> _currDeclare,_defaultDeclare;
    std::vector<EFileElement> _vEFileEle;
    std::string _currFunctionCode;
    std::string _recvSouceID;
};



class TicketBusinessFactory : public BusinessFactoryBase
{

    GM_SINGLETON_DECLARATION(TicketBusinessFactory)

    public:
        std::string factoryName()
    {
        return CS_BusinessTypeSimpleName_Ticket;
    }

    BusinessBase* createObject()
    {
        return new TicketBusiness();
    }
};
#endif // TICKET_BUSINESS_H
