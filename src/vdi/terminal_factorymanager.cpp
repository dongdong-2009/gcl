
#include "custom_terminal.h"

std::vector<CxFactoryTemplate<TerminalBase> *> *TerminalBase::factoriesContainer()
{
    static std::vector<CxFactoryTemplate<TerminalBase> *> m;
    return &m;
}

void TerminalBase::factoriesCreateAndRegister()
{
    static bool bNotInit = true;

    if (bNotInit)
    {
        TerminalFactoryManager::createAndRegister<CustomTerminalFactory>();
        bNotInit = false;
    }
}

