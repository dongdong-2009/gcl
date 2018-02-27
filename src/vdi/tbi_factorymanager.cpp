
#include "tbi_base.h"



std::vector<CxFactoryTemplate<TBIBase> *> *TBIBase::factoriesContainer()
{
    static std::vector<CxFactoryTemplate<TBIBase> *> m;
    return &m;
}

void TBIBase::factoriesCreateAndRegister()
{
//    static bool bNotInit = true;

//    if (bNotInit)
//    {
//        bNotInit = false;
//    }
}
