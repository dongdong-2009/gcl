#include "print_manager.h"

#include <vdi/terminal_manager.h>
#include "print_protocol.h"

int PrintManager::print(const char *sText, int iLength)
{
    PrintProtocol * oRfidProtocol = (PrintProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Print);
    if (oRfidProtocol)
    {
        return oRfidProtocol->print(sText, iLength);
    }
    return 0;
}
