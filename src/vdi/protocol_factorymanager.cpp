
#include "protocol_base.h"

std::vector<CxFactoryTemplate<ProtocolBase> *> *ProtocolBase::factoriesContainer()
{
    static std::vector<CxFactoryTemplate<ProtocolBase> *> m;
    return &m;
}

void ProtocolBase::factoriesCreateAndRegister()
{
    //    static bool bNotInit = true;

    //    if (bNotInit)
    //    {
    //        bNotInit = false;
    //    }
}

