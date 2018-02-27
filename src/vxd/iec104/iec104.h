#ifndef SDK_SERVER_H
#define SDK_SERVER_H

#include "iec104_protocol.h"

class IEC_104 //: public IPsmReceivedMessageNotify, public IPsmReceivedFileNotify, public CxInterinfoIn_I
{
public:
    static int start(int argc, char * argv[]);

private:
    static int reg();

    static void exitServer();

//    static int testSendMessage(const std::string & sMsg, const std::string & sParam = std::string());

//protected:
//    void received_message_command(const PsmProtocol * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach & attach);

//    void received_file(const PsmProtocol * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach & attach);

//    void interinfo_in_line( const std::string& sInfo, const msepoch_t & dtIn, int iSource = 0, int eInType = 0, int iTag = 0 );

};

#endif // SDK_SERVER_H
