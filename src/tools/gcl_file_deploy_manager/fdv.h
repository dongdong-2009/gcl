#ifndef FDV_H
#define FDV_H


#include <ccxx/ccxx.h>


class Fdv
{
public:
    std::string md5;
    std::string fileName;
    std::string version;
    std::string title;
    std::string moduleName;
    std::string desc;
    msepoch_t   modifyTime;
    std::string modifyContent;
    std::string modifier;

    Fdv();
    ~Fdv();

    void toContext(CxISetSkv& context) const;
    void fromContext(const CxIGetSkv& context);

};

class FdvManager
{
public:
    static const std::vector<Fdv> & getFdvs();

    static int load();

    static void save();

    static Fdv * findByMd5(const std::string & sMd5);

    static int add(const Fdv & fdv);

    static void removeByMd5(const std::string & sMd5);

};


#endif // FDV_H
