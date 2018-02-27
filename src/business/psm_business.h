#ifndef PSM_BUSINESS_H
#define PSM_BUSINESS_H


#include <vdi/psm_protocol.h>
#include <vdi/explain_xml.h>
#include <vdi/user_master.h>
#include <vdi/tbi_base.h>
#include <vdi/terminal_manager.h>
#include "tbi_base.h"


class PsmBusiness : public TBIBase, public IPsmProtocolNotify, public CxIMsObjectDataSubject
{
public:
    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv & context);

    void describeSelf(CxDescribeUnit * oDescribeUnit) const;

    int sendMessage(const std::string & sMsg, const std::string & sParam = std::string());

    int sendFile(const std::string & sFilePath);

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void businessAfterStart();

    void businessAfterStop();

    void registerProtocol(ProtocolBase* oProtocol);

    void unregisterProtocol();

    void received_message_command(const PsmProtocol * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL);

    void received_file_write(const PsmProtocol * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL);

    void received_realtime_data_request(const PsmProtocol * oPsm, const PsmAttach * oAttach = NULL);

    void received_realtime_data_post(const PsmProtocol * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

    void msObject_changed(const CxMsObjectData & oMsObject, int iChangeType) {}

    void msObjects_changed(const std::string & sTypeName, const std::vector<std::string> & sNames) {}

protected:
    inline PsmProtocol * psmProtocol() {
        if (_protocol && _protocol->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
        {
            return (PsmProtocol *)_protocol;
        }
        return NULL;
    }

    inline int getUserId() {
        return UserMaster::userIdByTerminalId(TerminalManager::remoteTerminalIdByProtocol(_protocol));
    }

    inline std::string localTypeName(const std::string & sTypeName) {
        return CxContainer::value(_mapingTypeNames, sTypeName);
    }

    inline std::string typeName(const std::string & sLocalTypeName) {
        return CxContainer::key(_mapingTypeNames, sLocalTypeName);
    }

    inline CxMsObjectType::StructType_Enum localStructType(const std::string & sTypeName)
    {
        if (sTypeName.size() > 0)
        {
            std::map<std::string, int>::const_iterator it = _localStructTypies.find(sTypeName);
            if (it != _localStructTypies.end())
                return (CxMsObjectType::StructType_Enum)it->second;
        }
        return CxMsObjectType::StructType_None;
    }

    inline DataOperateType_Enum localOperateType(const std::string & sTypeName)
    {
        if (sTypeName.size() > 0)
        {
            std::map<std::string, int>::const_iterator it = _localOperateTypies.find(sTypeName);
            if (it != _localOperateTypies.end())
                return (DataOperateType_Enum)it->second;
        }
        return DataOperateType_None;
    }

    inline const std::string & localFieldName(const std::string & sFieldName) {
        return CxContainer::value(_mapingFieldNames, sFieldName, sFieldName);
    }

protected:
    std::vector<std::string> _transparentCommands;//SampleNo=YPBM
    std::map<std::string, int> _localStructTypies; //UserInfo=1
    std::map<std::string, int> _localOperateTypies; //UserInfo=1
    std::map<std::string, std::string> _mapingTypeNames; //00304=UserInfo
    std::map<std::string, std::string> _mapingFieldNames;//SampleNo=YPBM

    uchar _sendBuf[GCL_LEN_DT];
    int   _sendLen;

};

#endif // PSM_BUSINESS_H
