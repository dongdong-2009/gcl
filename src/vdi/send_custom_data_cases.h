#ifndef SEND_CUSTOM_DATA_CASES_H
#define SEND_CUSTOM_DATA_CASES_H


#include "cases_base.h"


class IProtocolSendCustomData
{
protected:
    virtual int sendCustomData(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData) = 0;
    virtual int sendCustomData_syn(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData,std::map<std::string, std::string> &mapData) = 0;

    friend class SendCustomDataCases;

};


class GM_GCL_VDI_API SendCustomDataCases : public CasesBase
{

    GM_TYPEFULLNAME(CasesBase, CS_CasesTypeSimpleName_SendCustomData)

public:
    inline const IProtocolSendCustomData * protocolSendCustomData() { return _protocolSendCustomData; }

    inline int sendCustomData(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData)
    {
        return _protocolSendCustomData->sendCustomData(iSourceId, iCommandId, sParams, iDataLength, pData);
    }
    inline int sendCustomData_syn(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData,std::map<std::string, std::string> &mapData)
    {
        return _protocolSendCustomData->sendCustomData_syn(iSourceId, iCommandId, sParams, iDataLength, pData,mapData);
    }


protected:
    inline void setProtocol(IProtocolSendCustomData * oProtocol){ _protocolSendCustomData = oProtocol; }

    friend class ProtocolBase;

private:
    IProtocolSendCustomData * _protocolSendCustomData;

};


#endif // SEND_CUSTOM_DATA_CASES_H
