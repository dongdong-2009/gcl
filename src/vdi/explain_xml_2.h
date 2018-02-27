#ifndef EXPLAIN_XML_H
#define EXPLAIN_XML_H

#include <ccxx/ccxx.h>

class XmlMsObjectTemplate
{
public:
    inline XmlMsObjectTemplate() {
        _currentBodyPos = 0;
    }
    inline XmlMsObjectTemplate(const XmlMsObjectTemplate &o) {
        _currentBodyPos = o._currentBodyPos;
        _fileName = o._fileName;
        _head = o._head;
        _bodyBase = o._bodyBase;
        _last = o._last;
    }
    inline XmlMsObjectTemplate &operator=(const XmlMsObjectTemplate & other)
    {
        if (this != &other)
        {
            _currentBodyPos = other._currentBodyPos;
            _fileName = other._fileName;
            _head = other._head;
            _bodyBase = other._bodyBase;
            _last = other._last;
        }
        return *this;
    }
    inline ~XmlMsObjectTemplate() { }

    bool init( const std::vector<std::string> & sLines , const std::string & sDescriptHead , const std::string & sDescriptEnd );

    std::string toString();

    std::vector<std::string> toLines();

    void setHead(const std::string & sVarName, const std::string & sVarValue);

    void setBody(const std::string & sVarName, const std::string & sVarValue);

    void appendBody();

public:
    std::string _fileName;

private:
    std::vector<std::string> _head;
    std::vector<std::string> _bodyBase;
    std::vector<std::string> _last;
    std::vector<std::vector<std::string> > _bodys;
    size_t _currentBodyPos;

};

class XmlMsObjectTemplateManager
{
public:
    static int registerMessageTempltes();

    static int loadMessageTempltes();

    static const XmlMsObjectTemplate * findTemplate(const std::string & sFileName);

    //not detail, only one record
    static std::string toSingleTemplateString(const std::string & sCommand, const std::map<std::string, std::string> & sHeads);

    //has detail, is table
    static std::string toMultipleTemplateString(const std::string & sCommand, const std::map<std::string, std::string> & sHeads, const std::vector<std::map<std::string, std::string> > & sDetailList);

    //has detail, is table
    static std::string toMainTemplateString(const std::string & sCommand, const std::map<std::string, std::string> & sHeads, const std::vector<std::map<std::string, std::string> > & sDetailList);

    //has detail, is table
    static std::string toMainTemplateString(const std::string & sCommand, const std::map<std::string, std::string> & sHeads, const std::map<std::string, std::map<std::string, std::string> > & sDetails);

    //not detail, only one record
    static bool createSimpleTemplate(const std::string & sCommand, const std::map<std::string, std::string> & sHeads,std::string &sFileName);

    //has detail, is table
    static bool createDetailTemplate(const std::string & sCommand, const std::map<std::string, std::string> & sHeads, const std::vector<std::map<std::string, std::string> > & sDetailList,std::string &sFileName);

};


#endif // EXPLAIN_XML_H
