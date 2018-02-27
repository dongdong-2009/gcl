#ifndef FDC_H
#define FDC_H


#include <ccxx/ccxx.h>


class FdcItem
{
public:   
    std::string fdc;
    std::string rfilePath;
    int beupdate;
    std::string version;

    FdcItem();
    ~FdcItem();

    void toContext(CxISetSkv& context) const;
    void fromContext(const CxIGetSkv& context);

};

class Fdc
{
public:

public:
    std::string name;
    std::string rpath;
    std::string configcmd;
    std::string title;
    std::string varName;
    std::string varParams;
    msepoch_t   modifyTime;
    std::string modifyContent;
    std::string modifier;
    std::vector<FdcItem> items;

    Fdc();
    ~Fdc();

    void toContext(CxISetSkv& context) const;
    void fromContext(const CxIGetSkv& context);

    std::string getProductRfilepath(const FdcItem & fdcItem) { return CxFileSystem::mergeFilePath(rpath, fdcItem.rfilePath); }
    std::string getProductRfilepath(const std::string & sRfilePath) { return CxFileSystem::mergeFilePath(rpath, sRfilePath); }
    std::string getProductFilepath(const FdcItem & fdcItem);
    std::string getProductFilepath(const std::string & sRfilePath);

    FdcItem * find(const std::string & sRfilePath);

};

class FdcManager
{
public:
    static const std::vector<Fdc> & getFdcs();

    static int load();

    static void save();

    static Fdc * findByName(const std::string & sName);

    static int add(const Fdc & fdc);

    static void removeByName(const std::string & sName);

    static const std::string & getProductRootPath();

    static void setProductRootPath(const std::string &sPath);

    static std::string getProdoctFilePath(const std::string &sRpath, const std::string &sFileName);

    static const std::vector<std::string> & getProductLevel1Paths();

    static bool isBeupdate(const std::string & sRfilePath);

};


#endif // FDC_H
