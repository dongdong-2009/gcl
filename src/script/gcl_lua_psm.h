#ifndef GCL_LUA_PSM_H
#define GCL_LUA_PSM_H


#include <vdi/psm_protocol.h>


class GclLuaPsm : public IPsmProtocolNotify
{
public:
    static void registerPsmProtocolNotify();

    static void registerLuaPsm();

protected:
    void received_message_command(const PsmProtocol * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL);

    void received_file_write(const PsmProtocol * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL);

    void received_realtime_data_request(const PsmProtocol * oPsm, const PsmAttach * oAttach = NULL);

    void received_realtime_data_post(const PsmProtocol * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

};

#endif // GCL_LUA_PSM_H
