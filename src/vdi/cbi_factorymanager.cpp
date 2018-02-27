
#include "cbi_base.h"



std::vector<CxFactoryTemplate<CBIBase> *> *CBIBase::factoriesContainer()
{
    static std::vector<CxFactoryTemplate<CBIBase> *> m;
    return &m;
}

void CBIBase::factoriesCreateAndRegister()
{
    static bool bNotInit = true;

    if (bNotInit)
    {

        bNotInit = false;
    }
}
