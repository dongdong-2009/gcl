#include "explain_xml.h"

using namespace std;


static vector<XmlMsObjectTemplate> f_xmlTemplates;
static map<string, pair<string, string> > f_xmlTemplateDescripts;


int XmlMsObjectTemplateManager::registerMessageTempltes()
{
    f_xmlTemplateDescripts.clear();
    string sXmlTemplateDescripts = CxApplication::findConfig(CS_SectionProgramConfig, "XmlTemplateDescripts", std::string());
    vector<string> sLines = CxString::split(sXmlTemplateDescripts, ';');
    for (size_t i = 0; i < sLines.size(); ++i)
    {
        string sLine = sLines.at(i);
        string sKey = CxString::token(sLine, ',');
        string sBody = CxString::token(sLine, ',');
        string sEnd = sLine;
        if (sKey.size() > 0 && sBody.size() > 0 && sEnd.size() > 0)
        {
            f_xmlTemplateDescripts[sKey] = pair<string, string>(sBody, sEnd);
        }
    }
    return true;
    //00101,RuChangShiBie,</xml_end>;00102,</xml_end>,</xml_end>;00104,</xml_end>,</xml_end>;00105,</xml_end>,</xml_end>;00152,</xml_end>,</xml_end>;00153,</xml_end>,</xml_end>;00155,</xml_end>,</xml_end>;00157,</xml_end>,</xml_end>;00159,ZhiYangDanZB,</xml_end>;00201,</xml_end>,</xml_end>;00203,</xml_end>,</xml_end>;00207,</xml_end>,</xml_end>;00205,</xml_end>,</xml_end>;00301,</xml_end>,</xml_end>;00303,</xml_end>,</xml_end>;00306,</xml_end>,</xml_end>;00309,</xml_end>,</xml_end>;
}

int XmlMsObjectTemplateManager::loadMessageTempltes(const string &sPath)
{
    registerMessageTempltes();
    f_xmlTemplates.clear();

    string sFileTemplatePath = CxFileSystem::mergeFilePath( sPath , "file_template" );
    for(map<string, pair<string, string> >::const_iterator it = f_xmlTemplateDescripts.begin(); it != f_xmlTemplateDescripts.end(); ++it)
    {
        const string & sCommand = it->first;
        const pair<string, string> & sDescript = it->second;
        string sFileName = "template_" + sCommand + ".xml";
        string sFilePath = CxFileSystem::mergeFilePath( sFileTemplatePath , sFileName );
        if (CxFileSystem::isExist(sFilePath))
        {
            vector<string> sLines;
            CxFile::load(sFilePath, sLines);
            XmlMsObjectTemplate xmlTemplate;
            if (xmlTemplate.init(sLines, sDescript.first, sDescript.second))
            {
                xmlTemplate._fileName = sFileName;
                f_xmlTemplates.push_back(xmlTemplate);
            }
        }
    }

    return f_xmlTemplates.size();
}


const XmlMsObjectTemplate * XmlMsObjectTemplateManager::findTemplate(const string &sFileName)
{
    for (size_t i = 0; i < f_xmlTemplates.size(); ++i)
    {
        const XmlMsObjectTemplate & mTemplate = f_xmlTemplates.at(i);
        if (mTemplate._fileName == sFileName)
        {
            return & mTemplate;
        }
    }
    return NULL;
}


std::string XmlMsObjectTemplateManager::toSingleTemplateString(const string &sCommand, const std::map<string, string> & sHeads, const std::map<std::string, std::string> * oMapingFieldNames)
{
    string sFileName = "template_" + sCommand + ".xml";
    const XmlMsObjectTemplate * pTemplate = XmlMsObjectTemplateManager::findTemplate(sFileName);
    if (pTemplate)
    {
        XmlMsObjectTemplate mTemplate;
        mTemplate = * pTemplate;
        for(std::map<string, string>::const_iterator it = sHeads.begin(); it != sHeads.end(); ++it)
        {
            mTemplate.setHead(CxContainer::value(oMapingFieldNames, it->first, it->first), it->second);
        }
        return mTemplate.toString();
    }
    return "";
}

std::string XmlMsObjectTemplateManager::toMultipleTemplateString(const string &sCommand, const std::map<string, string> &sHeads, const std::vector<std::map<string, string> > &sDetailList, const std::map<std::string, std::string> * oMapingFieldNames)
{
    string sFileName = "template_" + sCommand + ".xml";
    const XmlMsObjectTemplate * pTemplate = XmlMsObjectTemplateManager::findTemplate(sFileName);
    if (pTemplate)
    {
        XmlMsObjectTemplate mTemplate;
        mTemplate = * pTemplate;
        for(std::map<string, string>::const_iterator it = sHeads.begin(); it != sHeads.end(); ++it)
        {
            mTemplate.setHead(CxContainer::value(oMapingFieldNames, it->first, it->first), it->second);
        }
        for (size_t i = 0; i < sDetailList.size(); ++i)
        {
            const map<string, string> & sDetail = sDetailList.at(i);
            mTemplate.appendBody();
            for(std::map<string, string>::const_iterator it = sDetail.begin(); it != sDetail.end(); ++it)
            {
                mTemplate.setBody(CxContainer::value(oMapingFieldNames, it->first, it->first), it->second);
            }
            for(std::map<string, string>::const_iterator it = sHeads.begin(); it != sHeads.end(); ++it)
            {
                mTemplate.setBody(CxContainer::value(oMapingFieldNames, it->first, it->first), it->second);
            }
        }

        return mTemplate.toString();
    }
    return "";
}

std::string XmlMsObjectTemplateManager::toMainTemplateString(const string &sCommand, const std::map<string, string> & sHeads, const std::vector<std::map<std::string, std::string> > & sDetailList, const std::map<std::string, std::string> * oMapingFieldNames)
{
    string sFileName = "template_" + sCommand + ".xml";
    const XmlMsObjectTemplate * pTemplate = XmlMsObjectTemplateManager::findTemplate(sFileName);
    if (pTemplate)
    {
        XmlMsObjectTemplate mTemplate;
        mTemplate = * pTemplate;
        for( std::map<string, string>::const_iterator it = sHeads.begin(); it != sHeads.end(); ++it)
        {
            mTemplate.setHead(CxContainer::value(oMapingFieldNames, it->first, it->first), it->second);
        }
        for (size_t i = 0; i < sDetailList.size(); ++i)
        {
            const map<string, string> & sDetail = sDetailList.at(i);
            mTemplate.appendBody();
            for( std::map<string, string>::const_iterator it = sDetail.begin(); it != sDetail.end(); ++it)
            {
                mTemplate.setBody(CxContainer::value(oMapingFieldNames, it->first, it->first), it->second);
            }
        }
        return mTemplate.toString();
    }
    return "";
}

string XmlMsObjectTemplateManager::toMainTemplateString(const string &sCommand, const std::map<string, string> &sHeads, const std::map<string, std::map<string, string> > & sDetails, const std::map<std::string, std::string> * oMapingFieldNames)
{
    string sFileName = "template_" + sCommand + ".xml";
    const XmlMsObjectTemplate * pTemplate = XmlMsObjectTemplateManager::findTemplate(sFileName);
    if (pTemplate)
    {
        XmlMsObjectTemplate mTemplate;
        mTemplate = * pTemplate;
        for( std::map<string, string>::const_iterator it = sHeads.begin(); it != sHeads.end(); ++it)
        {
            mTemplate.setHead(CxContainer::value(oMapingFieldNames, it->first, it->first), it->second);
        }

        for (std::map<std::string, std::map<std::string, std::string> >::const_iterator it1 = sDetails.begin(); it1 != sDetails.end(); ++it1)
        {
            const std::string & sDetailName = it1->first;
            const std::map<std::string, std::string> & sDetail = it1->second;

            mTemplate.appendBody();
            for( std::map<string, string>::const_iterator it = sDetail.begin(); it != sDetail.end(); ++it)
            {
                mTemplate.setBody(CxContainer::value(oMapingFieldNames, it->first, it->first), it->second);
            }
        }

        return mTemplate.toString();
    }
    return "";
}

bool XmlMsObjectTemplateManager::createSimpleTemplate(const string &sCommand, const std::map<string, string> & sHeads, const std::string & sPath, std::string & sFileName)
{
    string sName = "template_" + sCommand + ".xml";
    const XmlMsObjectTemplate * pTemplate = findTemplate(sName);
    if (pTemplate)
    {
        XmlMsObjectTemplate mTemplate;
        mTemplate = * pTemplate;
        for( std::map<string, string>::const_iterator it = sHeads.begin(); it != sHeads.end(); ++it)
        {
            mTemplate.setHead(it->first, it->second);
        }

        if (! CxFileSystem::isExist(sPath))
            CxFileSystem::createDirMultiLevel(sPath);
        sFileName =  CxFileSystem::mergeFilePath(sPath, CxTime::toString(CxTime::currentMsepoch(), (char)0, (char)0, (char)0)+"_"+ sCommand + ".xml");

        string sParam = mTemplate.toString();
        return  CxFile::save(sFileName,sParam);
    }
    return false;
}

bool XmlMsObjectTemplateManager::createDetailTemplate(const string &sCommand, const std::map<string, string> & sHeads, const std::vector<std::map<std::string, std::string> > & sDetailList, const std::string & sPath, std::string & sFileName)
{
    string sName = "template_" + sCommand + ".xml";
    const XmlMsObjectTemplate * pTemplate = findTemplate(sName);
    if (pTemplate)
    {
        XmlMsObjectTemplate mTemplate;
        mTemplate = * pTemplate;
        for( std::map<string, string>::const_iterator it = sHeads.begin(); it != sHeads.end(); ++it)
        {
            mTemplate.setHead(it->first, it->second);
        }
        for (size_t i = 0; i < sDetailList.size(); ++i)
        {
            const map<string, string> & sDetail = sDetailList.at(i);
            mTemplate.appendBody();
            for( std::map<string, string>::const_iterator it = sDetail.begin(); it != sDetail.end(); ++it)
            {
                mTemplate.setBody(it->first, it->second);
            }
        }

        if (! CxFileSystem::isExist(sPath))
            CxFileSystem::createDirMultiLevel(sPath);
        sFileName =  CxFileSystem::mergeFilePath(sPath, CxTime::toString(CxTime::currentMsepoch(), (char)0, (char)0, (char)0)+"_"+ sCommand + ".xml");

        string sParam = mTemplate.toString();
        return  CxFile::save(sFileName,sParam);
     }
    return false;
}


bool XmlMsObjectTemplate::init(const std::vector<string> &sLines, const string &sDescriptHead, const string &sDescriptEnd)
{
    if (sDescriptHead == sDescriptEnd)
    {
        std::vector<std::string>::const_iterator itXmlEnd = sLines.end();
        for (std::vector<std::string>::const_iterator it = sLines.begin(); it != sLines.end(); ++it)
        {
            const std::string & sLine = * it;
            if (sLine.find(sDescriptEnd) != std::string::npos)
            {
                itXmlEnd = it;
            }
        }
        if (itXmlEnd != sLines.end() && itXmlEnd - sLines.begin() > 1)
        {
            _bodyBase.clear();
            _last.clear();
            _head.resize(itXmlEnd - sLines.begin());
            std::copy(sLines.begin(), itXmlEnd, _head.begin());
            return true;
        }
    }
    else
    {
        std::string sBodyBegin = "<" + sDescriptHead + ">";
        std::string sBodyEnd = "</" + sDescriptHead + ">";
        std::vector<std::string>::const_iterator itHeadEnd = sLines.end();
        std::vector<std::string>::const_iterator itBodyEnd = sLines.end();
        std::vector<std::string>::const_iterator itLastEnd = sLines.end();
        for (std::vector<std::string>::const_iterator it = sLines.begin(); it != sLines.end(); ++it)
        {
            const std::string & sLine = * it;
            if (sLine.find(sBodyBegin) != std::string::npos)
            {
                itHeadEnd = it;
            }
            else if (sLine.find(sBodyEnd) != std::string::npos)
            {
                itBodyEnd = it + 1;
            }
            else if (sLine.find(sDescriptEnd) != std::string::npos)
            {
                itLastEnd = it;
                break;
            }
        }
        if (itHeadEnd != sLines.end() && itLastEnd != sLines.end() && itLastEnd - itHeadEnd > 1)
        {
            _head.resize(itHeadEnd - sLines.begin());
            std::copy(sLines.begin(), itHeadEnd, _head.begin());
            _bodyBase.resize(itBodyEnd - itHeadEnd);
            std::copy(itHeadEnd, itBodyEnd, _bodyBase.begin());
            _last.resize(itLastEnd - itBodyEnd);
            std::copy(itBodyEnd, itLastEnd, _last.begin());
            return true;
        }
    }
    return false;
}

string XmlMsObjectTemplate::toString()
{
    std::vector<std::string> sLines;
    for (size_t i = 0; i < _head.size(); ++i)
    {
        sLines.push_back(_head.at(i));
    }
    for (size_t i = 0; i < _bodys.size(); ++i)
    {
        const std::vector<std::string> & sBody = _bodys.at(i);
        for (size_t j = 0; j < sBody.size(); ++j)
        {
            sLines.push_back(sBody.at(j));
        }
    }
    for (size_t i = 0; i < _last.size(); ++i)
    {
        sLines.push_back(_last.at(i));
    }
    return CxString::join(sLines, cs_line_string);
}

std::vector<string> XmlMsObjectTemplate::toLines()
{
    std::vector<std::string> sLines;
    for (size_t i = 0; i < _head.size(); ++i)
    {
        sLines.push_back(_head.at(i));
    }
    for (size_t i = 0; i < _bodys.size(); ++i)
    {
        const std::vector<std::string> & sBody = _bodys.at(i);
        for (size_t j = 0; j < sBody.size(); ++j)
        {
            sLines.push_back(sBody.at(i));
        }
    }
    for (size_t i = 0; i < _last.size(); ++i)
    {
        sLines.push_back(_last.at(i));
    }
    return sLines;
}

void XmlMsObjectTemplate::setHead(const string &sVarName, const string &sVarValue)
{
    string sNewVarName = "[";
    sNewVarName.append(sVarName);
    sNewVarName.push_back(']');
    for (std::vector<std::string>::iterator it = _head.begin(); it != _head.end(); ++it)
    {
        std::string & sLine = * it;
        size_t iPos = sLine.find(sNewVarName);
        if(iPos != std::string::npos)
        {
            sLine.replace(iPos, sNewVarName.size(), sVarValue);
            break;
        }
    }
}

void XmlMsObjectTemplate::setBody(const string &sVarName, const string &sVarValue)
{
    string sNewVarName = "[";
    sNewVarName.append(sVarName);
    sNewVarName.push_back(']');

    if (_bodys.empty())
    {
        appendBody();
    }
    if (_currentBodyPos < _bodys.size())
    {
        std::vector<std::string> & sBody = _bodys.at(_currentBodyPos);
        for (std::vector<std::string>::iterator it = sBody.begin(); it != sBody.end(); ++it)
        {
            std::string & sLine = * it;
            size_t iPos = sLine.find(sNewVarName);
            if(iPos != std::string::npos)
            {
                sLine.replace(iPos, sNewVarName.size(), sVarValue);
                break;
            }
        }
    }
}

void XmlMsObjectTemplate::appendBody()
{
    _currentBodyPos = _bodys.size();
    _bodys.push_back(_bodyBase);
}
