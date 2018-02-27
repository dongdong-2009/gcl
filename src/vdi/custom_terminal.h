#ifndef CUSTOM_TERMINAL_H
#define CUSTOM_TERMINAL_H

#include "protocol_global.h"


#include "terminal_base.h"

class GM_GCL_VDI_API CustomTerminal : public TerminalBase
{

    GM_TYPEFULLNAME(TerminalBase, CS_Custom)

public:

};

class GM_GCL_VDI_API CustomTerminalFactory : public TerminalFactoryBase
{

//    GM_SINGLETON_DECLARATION(CustomTerminalFactory)

public:
    std::string factoryName() {
        return CS_Custom;
    }

    TerminalBase* createObject() {
        return new CustomTerminal;
    }

};


#endif // !defined(CUSTOM_TERMINAL_H)
