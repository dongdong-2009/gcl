#ifndef TERMINAL_FACTORY_MANAGER_H
#define TERMINAL_FACTORY_MANAGER_H

#include "protocol_global.h"


#include "terminal_base.h"


class TerminalFactoryManager : public CxFactoryManagerTemplate<TerminalBase>
{
};
#endif // !defined(TERMINAL_FACTORY_MANAGER_H)
