#ifndef TERMINAL_LOADER_H
#define TERMINAL_LOADER_H

#include "protocol_global.h"

#include "terminal_manager.h"


class GM_GCL_VDI_API TerminalLoader
{
public:
    static void load(const std::string& filePath);

    static void save(const std::string& filePath);

    static void load(const std::string & filePath,const std::string &sTerminalName);

    static TerminalBase* loadTerminal(const std::string &sTerminalName, const std::string &sTerminalTypeSimpleName, const std::string& sFilePath);

    static void loadProtocolTables(ProtocolBase* oProtocol, const std::string& sPath);

    static void saveProtocolTables(ProtocolBase* oProtocol, const std::string& sPath);

    static void reloadTerminalContexts(TerminalBase* oTerminal, const std::string& sFilePath);

    static void reloadProtocolTable(TerminalBase* oTerminal, const std::string& sTableName, const std::string& sFilePath);

    static void saveProtocolTable(TerminalBase* oTerminal, const std::string& sTableName, const std::string& sFilePath);

    static void saveTerminal(TerminalBase* oTerminal, const std::string& sFilePath);

    static void saveTerminalContexts(TerminalBase* oTerminal, const std::string& sFilePath);

};

#endif // TERMINAL_LOADER_H
