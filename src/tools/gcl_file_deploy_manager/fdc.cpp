#include "fdc.h"


using namespace std;


static std::string f_sProductPath;
static vector<Fdc> f_fdcs;


Fdc::Fdc()
{
    modifyTime = 0;
}

Fdc::~Fdc()
{

}

void Fdc::toContext(CxISetSkv &context) const
{
    context.setValue(string("name"),             name)          ;
    context.setValue(string("rpath"),        rpath)     ;
    context.setValue(string("configcmd"),           configcmd)    ;
    context.setValue(string("title"),           title)        ;
    context.setValue(string("varName"),      varName)   ;
    context.setValue(string("varParams"),            varParams)         ;
    context.setValue(string("modifyTime"),      modifyTime)   ;
    context.setValue(string("modifyContent"),   modifyContent);
    context.setValue(string("modifier"),        modifier)     ;
}

void Fdc::fromContext(const CxIGetSkv &context)
{
    name =           context.getValue(string("name"),             name)          ;
    rpath =      context.getValue(string("rpath"),        rpath)     ;
    configcmd =         context.getValue(string("configcmd"),           configcmd)  ;
    title =         context.getValue(string("title"),           title)        ;
    varName =    context.getValue(string("varName"),      varName)   ;
    varParams =          context.getValue(string("varParams"),            varParams)         ;
    modifyTime =    context.getValue(string("modifyTime"),      modifyTime)   ;
    modifyContent = context.getValue(string("modifyContent"),   modifyContent);
    modifier =      context.getValue(string("modifier"),        modifier)     ;
}

string Fdc::getProductFilepath(const FdcItem &fdcItem)
{
    return CxFileSystem::fullFilePath(f_sProductPath, rpath, fdcItem.rfilePath);
}

string Fdc::getProductFilepath(const string &sRfilePath)
{
    return CxFileSystem::fullFilePath(f_sProductPath, rpath, sRfilePath);
}

FdcItem *Fdc::find(const string &sRfilePath)
{
    for (size_t i = 0; i < items.size(); ++i)
    {
        FdcItem & item = items.at(i);
        if (item.rfilePath == sRfilePath)
        {
            return & item;
        }
    }
    return NULL;
}



const std::vector<Fdc> &FdcManager::getFdcs()
{
    return f_fdcs;
}

int FdcManager::load()
{
    string myFilePath = CxFileSystem::mergeFilePath(f_sProductPath, "product_list.xml");
    f_fdcs = CxSkverManager::loadStructs<Fdc>(myFilePath);
    myFilePath = CxFileSystem::mergeFilePath(f_sProductPath, "product_item_list.xml");
    std::vector<FdcItem> fdcItems = CxSkverManager::loadStructs<FdcItem>(myFilePath);

    for (size_t i = 0; i < fdcItems.size(); ++i)
    {
        const FdcItem & item = fdcItems.at(i);
        for (size_t j = 0; j < f_fdcs.size(); ++j)
        {
            Fdc &fdc = f_fdcs.at(j);
            if (fdc.name == item.fdc)
            {
                fdc.items.push_back(item);
            }
        }
    }

    return f_fdcs.size();
}

void FdcManager::save()
{
    string myFilePath = CxFileSystem::mergeFilePath(f_sProductPath, "product_list.xml");
    CxSkverManager::saveStructs<Fdc>(myFilePath, f_fdcs);

    std::vector<FdcItem> fdcItems;
    for (size_t j = 0; j < f_fdcs.size(); ++j)
    {
        Fdc &fdc = f_fdcs.at(j);
        for (size_t i = 0; i < fdc.items.size(); ++i)
        {
            FdcItem & item = fdc.items.at(i);
            item.fdc = fdc.name;
            fdcItems.push_back(item);
        }
    }
    myFilePath = CxFileSystem::mergeFilePath(f_sProductPath, "product_item_list.xml");
    CxSkverManager::saveStructs<FdcItem>(myFilePath, fdcItems);
}

Fdc *FdcManager::findByName(const string &sName)
{
    for (size_t i = 0; i < f_fdcs.size(); ++i)
    {
        Fdc & fdc = f_fdcs.at(i);
        if (fdc.name == sName)
        {
            return & fdc;
        }
    }
    return NULL;
}

int FdcManager::add(const Fdc &fdc)
{
    if (fdc.name.size() > 0 && ! findByName(fdc.name))
    {
        f_fdcs.push_back(fdc);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void FdcManager::removeByName(const string &sName)
{
    for(std::vector<Fdc>::iterator it = f_fdcs.begin(); it != f_fdcs.end();)
    {
        const Fdc & fdc = * it;
        if (fdc.name == sName)
        {
            f_fdcs.erase(it);
            continue;
        }
        ++it;
    }
}

const string &FdcManager::getProductRootPath()
{
    return f_sProductPath;
}

void FdcManager::setProductRootPath(const string &sPath)
{
    f_sProductPath = CxFileSystem::convertPathStyle(sPath);
}

string FdcManager::getProdoctFilePath(const string &sRpath, const string &sFileName)
{
    return CxFileSystem::fullFilePath(f_sProductPath, sRpath, sFileName);
}

const std::vector<string> &FdcManager::getProductLevel1Paths()
{
    static string a[] = {"ics_3rd", "ics_apps", "ics_fes", "ics_gds", "ics_gis", "ics_lua", "ics_svr", "ics_test", "ics_tools"};
    static std::vector<string> m(a, a+sizeof(a)/sizeof(a[0]));
    return m;
}

bool FdcManager::isBeupdate(const string &sRfilePath)
{
    string sSuffixName = CxString::toLower(CxFileSystem::extractFileSuffixName(sRfilePath));
    if (sSuffixName.find(".ini") != string::npos
            || sSuffixName.find(".txt") != string::npos
            || sSuffixName.find(".xml") != string::npos
            || sSuffixName.find(".log") != string::npos
            || sSuffixName.find(".efl") != string::npos
            || sSuffixName.find(".db") != string::npos)
    {
        return false;
    }
    return true;
}



FdcItem::FdcItem()
{

}

FdcItem::~FdcItem()
{

}

void FdcItem::toContext(CxISetSkv &context) const
{
    context.setValue(string("product"),             fdc)          ;
    context.setValue(string("rfilepath"),        rfilePath)     ;
    context.setValue(string("beupdate"),           beupdate)    ;
    context.setValue(string("version"),           version)        ;
}

void FdcItem::fromContext(const CxIGetSkv &context)
{
    fdc =           context.getValue(string("product"),             fdc)          ;
    rfilePath =      context.getValue(string("rfilepath"),        rfilePath)     ;
    beupdate =         context.getValue(string("beupdate"),           beupdate)  ;
    version =         context.getValue(string("version"),           version)        ;
}
