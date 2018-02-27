#ifndef TERMINAL_STRINGS_H
#define TERMINAL_STRINGS_H

#include "protocol_global.h"


inline std::string g_fn_terminalFilePath(const std::string& sTerminalName)
{
    std::string sTerminalFileName = sTerminalName + ".ini";
    return CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), sTerminalFileName);
}


#endif // !defined(TERMINAL_STRINGS_H)
