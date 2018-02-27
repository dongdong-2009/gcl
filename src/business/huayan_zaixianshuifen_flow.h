#ifndef HUAYAN_ZAIXIANSHUIFEN_FLOW_H
#define HUAYAN_ZAIXIANSHUIFEN_FLOW_H


#include <vdi/protocol_global.h>

//<--在线水分
//>--在线水分

class HuayanZaixianshuifenFlow
{
public:
    static void ruixianReceivedCommandMessage(const std::string & sCommand, const char * pParam, int iParamLength);

    static void cicsReceivedXmlContent(const std::string & sMsgNo, const std::map<std::string, std::string> & major, const std::map<std::string, std::map<std::string, std::string> > & details);

    static void asm570ReceivedCommandMessage(const std::string & sCommand, const std::string &sParam);

    static void init();

};

#endif // HUAYAN_ZAIXIANSHUIFEN_FLOW_H
