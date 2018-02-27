#ifndef TERMINAL_MANAGER_H
#define TERMINAL_MANAGER_H


#include "protocol_global.h"

#include "terminal_base.h"


class GM_GCL_VDI_API TerminalManager
{
public:
    static TerminalBase * createTerminal(const std::string &sTerminalName, const std::string& sTerminalTypeSimpleName, const std::string& sProtocolTypeSimpleName, const std::string& sBusinessTypeSimpleName, const std::string& sChannelTypeSimpleName);

    static const std::vector<TerminalBase*>* terminals();

    static std::vector<TerminalBase*> terminals(const std::vector<std::string> & sTerminalNames);

    static std::vector<TerminalBase*> terminals(const std::vector<int> & sTerminalIds);

    static std::vector<std::string> terminalNames();

    static bool deleteTerminal(TerminalBase* oTerminal);

    static TerminalBase* terminal(const std::string& sTerminalName, bool caseSensitive = false);

    static TerminalBase* terminal(int iTerminalId);

    static TerminalBase* terminalByProtocol(int iProtocolId);

    static TerminalBase* terminalByProtocol(const ProtocolBase * oProtocol);

    static TerminalBase* terminalByChannel(int iChannelId);

    static TerminalBase* terminalByBusiness(int iBusinessId);

    static TerminalBase* terminalByRemoteTerminalId(int iRemoteTerminalId);

    static inline int remoteTerminalIdByProtocol(const ProtocolBase * oProtocol);

    static ProtocolBase * findProtocol(const std::string& sProtocolTypeSimpleName);

    static void start();  //开始

    static void stop();  //停止

    static bool isStarted(); //重新启动

    static int interinfoIn_line( const std::string & sInfo, const msepoch_t & dtIn, int iSource, int eInType, int iTag);

    static int interinfoIn_cmd( const std::string & sCommand, const std::map<std::string, std::string> & sParams, const msepoch_t & dtIn, int iSource, int eInType, int iTag);

    static std::vector<std::string> reportSelf(const std::vector<std::string> & sTerminalNames = std::vector<std::string>(), const std::vector<std::string> & sTerminalIds = std::vector<std::string>());

private:
    static void append( TerminalBase* oTerminal);

    static void remove( TerminalBase* oTerminal);

public:
    static std::string protocolName(const std::string& sTerminalName);

    static std::string protocolName(const TerminalBase* oTerminal);

    static std::string businessName(const std::string& sTerminalName);

    static std::string businessName(const TerminalBase* oTerminal);

    static std::string channelName(const std::string& sProtocolName);

    static std::string channelName(TerminalBase* oTerminal);

    static std::string channelName(const ProtocolBase* oProtocol);

    static std::string workerTableFileName(TerminalBase* oTerminal, const std::string& sTableName);

    static std::string workerTableFileName(const ProtocolBase* oProtocol, const std::string& sTableName);

    static std::string workerTableFileName(const std::string& sProtocolName, const std::string& sTableName);

public:
    static void enableInterinfoOut(TerminalBase * oTerminal, bool bOnly = false);

    static void disableInterinfoOut(TerminalBase * oTerminal);


};

inline int TerminalManager::remoteTerminalIdByProtocol(const ProtocolBase *oProtocol)
{
    if (oProtocol)
    {
        TerminalBase * oTerminal = terminalByProtocol(oProtocol);
        if (oTerminal)
            return oTerminal->remoteTerminalId();
    }
    return 0;
}

#endif // !defined(TERMINAL_MANAGER_H)
