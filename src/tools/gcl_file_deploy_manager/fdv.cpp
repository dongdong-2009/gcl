#include "fdv.h"


#include "fdc.h"


using namespace std;


vector<Fdv> f_fdvs;


Fdv::Fdv()
{
    modifyTime = 0;
}

Fdv::~Fdv()
{

}

void Fdv::toContext(CxISetSkv &context) const
{
    context.setValue(string("md5"),             md5)          ;
    context.setValue(string("fileName"),        fileName)     ;
    context.setValue(string("version"),           version)        ;
    context.setValue(string("title"),           title)        ;
    context.setValue(string("moduleName"),      moduleName)   ;
    context.setValue(string("desc"),            desc)         ;
    context.setValue(string("modifyTime"),      modifyTime)   ;
    context.setValue(string("modifyContent"),   modifyContent);
    context.setValue(string("modifier"),        modifier)     ;
}

void Fdv::fromContext(const CxIGetSkv &context)
{
    md5 =           context.getValue(string("md5"),             md5)          ;
    fileName =      context.getValue(string("fileName"),        fileName)     ;
    version =         context.getValue(string("version"),           version)        ;
    title =         context.getValue(string("title"),           title)        ;
    moduleName =    context.getValue(string("moduleName"),      moduleName)   ;
    desc =          context.getValue(string("desc"),            desc)         ;
    modifyTime =    context.getValue(string("modifyTime"),      modifyTime)   ;
    modifyContent = context.getValue(string("modifyContent"),   modifyContent);
    modifier =      context.getValue(string("modifier"),        modifier)     ;
}



const std::vector<Fdv> &FdvManager::getFdvs()
{
    return f_fdvs;
}

int FdvManager::load()
{
    string myFilePath = CxFileSystem::mergeFilePath(FdcManager::getProductRootPath(), "file_deploy_list.xml");
    f_fdvs = CxSkverManager::loadStructs<Fdv>(myFilePath);
    return f_fdvs.size();
}

void FdvManager::save()
{
    string myFilePath = CxFileSystem::mergeFilePath(FdcManager::getProductRootPath(), "file_deploy_list.xml");
    CxSkverManager::saveStructs<Fdv>(myFilePath, f_fdvs);
}

Fdv *FdvManager::findByMd5(const string &sMd5)
{
    for (size_t i = 0; i < f_fdvs.size(); ++i)
    {
        Fdv & fdv = f_fdvs.at(i);
        if (fdv.md5 == sMd5)
        {
            return & fdv;
        }
    }
    return NULL;
}

int FdvManager::add(const Fdv &fdv)
{
    if (fdv.md5.size() > 0 && ! findByMd5(fdv.md5))
    {
        f_fdvs.push_back(fdv);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void FdvManager::removeByMd5(const string &sMd5)
{
    for(std::vector<Fdv>::iterator it = f_fdvs.begin(); it != f_fdvs.end();)
    {
        const Fdv & fdv = * it;
        if (fdv.md5 == sMd5)
        {
            f_fdvs.erase(it);
            continue;
        }
        ++it;
    }
}

