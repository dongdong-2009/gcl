#include "zhiyang_business.h"


ZhiyangBusinessFactory* fn_regZhiyangBusinessFactory()
{
    TBIFactoryManager::createAndRegister<ZhiyangBusinessFactory>();
    return NULL;
}

ZhiyangBusinessFactory* ZhiyangBusinessFactory::s_instance = fn_regZhiyangBusinessFactory();


using namespace std;

