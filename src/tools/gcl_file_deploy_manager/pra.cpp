#include "pra.h"


using namespace std;


static std::string f_sProjectPath;
static vector<Pra> f_pras;


Pra::Pra()
{
    port_l = 0;
}

Pra::~Pra()
{

}

void Pra::toContext(CxISetSkv &context) const
{
    context.setValue(string("name"),             name)          ;
    context.setValue(string("rpath"),        rpath)     ;
    context.setValue(string("res1"),           res1)    ;
    context.setValue(string("title"),           title)        ;
    context.setValue(string("ip_r"),      ip_r)   ;
    context.setValue(string("port_r"),      port_r)   ;
    context.setValue(string("ip_l"),        ip_l)     ;
    context.setValue(string("port_l"),      port_l)   ;
    context.setValue(string("desc"),            desc)         ;
    context.setValue(string("remark"),   remark);
}

void Pra::fromContext(const CxIGetSkv &context)
{
    name =           context.getValue(string("name"),             name)          ;
    rpath =      context.getValue(string("rpath"),        rpath)     ;
    res1 =         context.getValue(string("res1"),           res1)  ;
    title =         context.getValue(string("title"),           title)        ;
    ip_r =    context.getValue(string("ip_r"),      ip_r)   ;
    port_r =    context.getValue(string("port_r"),      port_r)   ;
    desc =          context.getValue(string("desc"),            desc)         ;
    port_l =    context.getValue(string("port_l"),      port_l)   ;
    remark = context.getValue(string("remark"),   remark);
    ip_l =      context.getValue(string("ip_l"),        ip_l)     ;
}

string Pra::getAreaPath() const
{
    return CxFileSystem::mergeFilePath(f_sProjectPath, rpath);
}

string Pra::getProductPath(const Fdc *oFdc) const
{
    if (oFdc)
    {
        return CxFileSystem::fullFilePath(f_sProjectPath, rpath, oFdc->rpath);
    }
    else
    {
        return string();
    }
}

string Pra::getProductFilepath(const Fdc *oFdc, const FdcItem &fdcItem) const
{
    if (oFdc)
    {
        return CxFileSystem::fullFilePath(f_sProjectPath, CxFileSystem::mergeFilePath(rpath, oFdc->rpath), fdcItem.rfilePath);
    }
    else
    {
        return string();
    }
}

string Pra::getProductFilepath(const Fdc *oFdc, const string &sRfilePath) const
{
    if (oFdc)
    {
        return CxFileSystem::fullFilePath(f_sProjectPath, CxFileSystem::mergeFilePath(rpath, oFdc->rpath), sRfilePath);
    }
    else
    {
        return string();
    }
}

string Pra::getConfigArgs(const Fdc *oFdc) const
{
    if (oFdc)
    {
        return CxFileSystem::fullFilePath(f_sProjectPath, CxFileSystem::mergeFilePath(rpath, oFdc->rpath), oFdc->configcmd);
    }
    else
    {
        return string();
    }
}

const std::vector<Pra> &PraManager::getPras()
{
    return f_pras;
}

int PraManager::load()
{
    string myFilePath = CxFileSystem::mergeFilePath(f_sProjectPath, "area_list.xml");
    f_pras = CxSkverManager::loadStructs<Pra>(myFilePath);
    myFilePath = CxFileSystem::mergeFilePath(f_sProjectPath, "area_item_list.xml");
    std::vector<PraItem> praItems = CxSkverManager::loadStructs<PraItem>(myFilePath);

    for (size_t i = 0; i < praItems.size(); ++i)
    {
        const PraItem & item = praItems.at(i);
        for (size_t j = 0; j < f_pras.size(); ++j)
        {
            Pra &pra = f_pras.at(j);
            if (pra.name == item.pra)
            {
                pra.items.push_back(item);
            }
        }
    }

    return f_pras.size();
}

void PraManager::save()
{
    string myFilePath = CxFileSystem::mergeFilePath(f_sProjectPath, "area_list.xml");
    CxSkverManager::saveStructs<Pra>(myFilePath, f_pras);

    std::vector<PraItem> praItems;
    for (size_t j = 0; j < f_pras.size(); ++j)
    {
        Pra &pra = f_pras.at(j);
        for (size_t i = 0; i < pra.items.size(); ++i)
        {
            PraItem & item = pra.items.at(i);
            item.pra = pra.name;
            praItems.push_back(item);
        }
    }
    myFilePath = CxFileSystem::mergeFilePath(f_sProjectPath, "area_item_list.xml");
    CxSkverManager::saveStructs<PraItem>(myFilePath, praItems);
}

Pra *PraManager::findByName(const string &sName)
{
    for (size_t i = 0; i < f_pras.size(); ++i)
    {
        Pra & pra = f_pras.at(i);
        if (pra.name == sName)
        {
            return & pra;
        }
    }
    return NULL;
}

int PraManager::add(const Pra &pra)
{
    if (pra.name.size() > 0 && ! findByName(pra.name))
    {
        f_pras.push_back(pra);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void PraManager::removeByName(const string &sName)
{
    for(typename std::vector<Pra>::iterator it = f_pras.begin(); it != f_pras.end();)
    {
        const Pra & pra = * it;
        if (pra.name == sName)
        {
            f_pras.erase(it);
            continue;
        }
        ++it;
    }
}

const string &PraManager::getProjectRootPath()
{
    return f_sProjectPath;
}

void PraManager::setProjectRootPath(const string &sPath)
{
    f_sProjectPath = CxFileSystem::convertPathStyle(sPath);
}

string PraManager::getFullPath(const string &sRpath, const string &sFileName)
{
    return CxFileSystem::fullFilePath(f_sProjectPath, sRpath, sFileName);
}

Pra *PraManager::findByFdc(const string &sFdc)
{
    for (size_t i = 0; i < f_pras.size(); ++i)
    {
        Pra & pra = f_pras.at(i);
        std::vector<PraItem> & praItems = pra.items;
        for (size_t j = 0; j < praItems.size(); ++j)
        {
            PraItem & praItem = praItems.at(j);
            if (praItem.fdc == sFdc)
            {
                return & pra;
            }
        }
    }

    return NULL;
}

PraItem::PraItem()
{

}

PraItem::~PraItem()
{

}

void PraItem::toContext(CxISetSkv &context) const
{
    context.setValue(string("pra"),             pra)          ;
    context.setValue(string("fdc"),             fdc)          ;
    context.setValue(string("res1"),           res1)     ;
    context.setValue(string("desc"),           desc)        ;
}

void PraItem::fromContext(const CxIGetSkv &context)
{
    pra =           context.getValue(string("pra"),             pra)          ;
    fdc =           context.getValue(string("fdc"),             fdc)          ;
    res1 =      context.getValue(string("res1"),        res1)     ;
    desc =         context.getValue(string("desc"),           desc)        ;
}
