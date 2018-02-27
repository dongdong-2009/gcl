#ifndef PRA_H
#define PRA_H


#include <ccxx/ccxx.h>
#include "fdc.h"

class PraItem
{
public:
    std::string pra;
    std::string fdc;
    std::string res1;
    std::string desc;

    PraItem();
    ~PraItem();

    void toContext(CxISetSkv& context) const;
    void fromContext(const CxIGetSkv& context);

};

class Pra
{
public:

public:
    std::string name;
    std::string rpath;
    std::string res1;
    std::string title;
    std::string ip_r;
    int port_r;
    std::string ip_l;
    int   port_l;
    std::string desc;
    std::string remark;
    std::vector<PraItem> items;

    Pra();
    ~Pra();

    void toContext(CxISetSkv& context) const;
    void fromContext(const CxIGetSkv& context);

    //*已经包含自身的 rpath
    std::string getAreaPath() const;
    //*区域中的产品相关路径
    inline std::string getProductRpath(const Fdc * oFdc) const { return oFdc ? CxFileSystem::mergeFilePath(rpath, oFdc->rpath) : std::string(); }
    std::string getProductPath(const Fdc * oFdc) const;
    inline std::string getProductRfilepath(const Fdc * oFdc, const FdcItem & fdcItem) const { return oFdc ? CxFileSystem::mergeFilePath(CxFileSystem::mergeFilePath(rpath, oFdc->rpath) , fdcItem.rfilePath) : std::string(); }
    inline std::string getProductRfilepath(const Fdc * oFdc, const std::string & sRfilePath) const { return oFdc ? CxFileSystem::mergeFilePath(CxFileSystem::mergeFilePath(rpath, oFdc->rpath) , sRfilePath) : std::string(); }
    std::string getProductFilepath(const Fdc * oFdc, const FdcItem & fdcItem) const;
    std::string getProductFilepath(const Fdc * oFdc, const std::string & sRfilePath) const;

    std::string getConfigArgs(const Fdc * oFdc) const;

};

class PraManager
{
public:
    static const std::vector<Pra> & getPras();

    static int load();

    static void save();

    static Pra * findByName(const std::string & sName);

    static int add(const Pra & pra);

    static void removeByName(const std::string & sName);

    static const std::string & getProjectRootPath();

    static void setProjectRootPath(const std::string &sPath);

    static std::string getFullPath(const std::string &sRpath, const std::string &sFileName);

    static Pra * findByFdc(const std::string &sFdc);

};


#endif // PRA_H
