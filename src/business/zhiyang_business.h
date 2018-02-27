#ifndef ZHIYANG_BUSINESS_H
#define ZHIYANG_BUSINESS_H


#include "cics_business.h"


class ZhiyangBusiness : public CicsBusiness
{
public:
    inline const std::string& typeSimpleName() const { return CS_BusinessTypeSimpleName_Zhiyang; }

};



class ZhiyangBusinessFactory : public TBIFactoryBase
{

    GM_SINGLETON_DECLARATION(ZhiyangBusinessFactory)

public:
    std::string factoryName()
    {
        return CS_BusinessTypeSimpleName_Zhiyang;
    }

    TBIBase* createObject()
    {
        return new ZhiyangBusiness();
    }
};

#endif // ZHIYANG_BUSINESS_H
