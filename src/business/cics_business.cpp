#include "cics_business.h"

#include "huayan_zaixianshuifen_flow.h"


CicsBusinessFactory* fn_regCicsBusinessFactory()
{
    TBIFactoryManager::createAndRegister<CicsBusinessFactory>();
    return NULL;
}

CicsBusinessFactory* CicsBusinessFactory::s_instance = fn_regCicsBusinessFactory();


using namespace std;


//Type ：   “Main“ -  表示主从表
//               “Single" -  表示消息内容中只有一条记录
//               “Multiple" -  表示消息内容中有多条记录
//ChildName  - 表示从表XML节点名称
//ChildCount － 表示从表个数
//
//<?xml version="1.0" encoding="utf-8"?>
//<ArrayOfUserInfo>
//  <HEAD>
//    <VERSION></VERSION>
//    <SRC>3</SRC>
//    <DES>4</DES>
//    <MsgNo>00304</MsgNo>
//    <MsgId>00420140529051166006</MsgId>
//    <MsgRef></MsgRef>
//    <TransDate>2015-05-28 16:03:54</TransDate>
//  </HEAD>
//  <UserInfo>
//    <gids>001,009</gids>              <!—角色GUID,角色GUID-->
//  </UserInfo>
int CicsBusiness::explain(const char *sParam, int iParamLength)
{
    if (! sParam || iParamLength <= 0)
    {
        return false;
    }
    TiXmlDocument doc;

    if (strstr(sParam, "< Powers>") != NULL)
    {
        string sSource = sParam;
        sSource = CxString::replace(sSource, "< Powers>", "<Powers>");
        doc.Parse(sSource.c_str());
    }
    else
    {
        doc.Parse(sParam);
    }

    //<?xml ......
    TiXmlNode * root = doc.FirstChild();
    //<Array ......
    if (root && root->Type() == TiXmlNode::TINYXML_DECLARATION)
    {
        root = root->NextSibling();
    }
    if (! root || root->Type() != TiXmlNode::TINYXML_ELEMENT)
    {
        return false;
    }
    TiXmlElement * element = root->ToElement();
    string sType = CxXml::attribute(element, "Type");
    string sChildName = CxXml::attribute(element, "ChildName");
    string sChildCount = CxXml::attribute(element, "ChildCount");
    int iChildCount = 0;
    if (sType == "Main")
    {
        CxString::fromString(sChildCount, iChildCount);
        if (sChildName.empty() || iChildCount <= 0)
            return false;
    }

    string sMsgNo;
    string sTypeName;
    int iObjectCount = 0;
    bool bWriteBegin = false;
    DataOperateType_Enum eOperateType = DataOperateType_None;
    //<HEAD>
    //<UserInfo>
    for( TiXmlElement * node1Level = root->FirstChildElement(); node1Level; node1Level = node1Level->NextSiblingElement() )
    {
        if (! node1Level) break;

        std::map<std::string, std::string> major;
        std::map<std::string, std::map<std::string, std::string> > details;

        if (node1Level->ValueStr() == "HEAD")
        {
            for( TiXmlElement * node2Level = node1Level->FirstChildElement(); node2Level; node2Level = node2Level->NextSiblingElement() )
            {
                if (! node2Level) break;
                if (node2Level->ValueStr() == "MsgNo")
                {
                    sMsgNo = CxXml::text(node2Level);
                }
            }
            if (sMsgNo.empty())
            {
                return false;
            }
        }
        else
        {
            if (sType != "Main")
            {
                for( TiXmlElement * node2Level = node1Level->FirstChildElement(); node2Level; node2Level = node2Level->NextSiblingElement() )
                {
                    if (! node2Level) break;
                    string sKey = node2Level->ValueStr();
                    string sValue = CxXml::text(node2Level);
                    major[sKey] = sValue;
                }
            }
            else
            {
                int iRow = 0;
                for( TiXmlElement * node2Level = node1Level->FirstChildElement(); node2Level; node2Level = node2Level->NextSiblingElement() )
                {
                    if (! node2Level) break;
                    string sKey2 = node2Level->ValueStr();
                    if (sKey2 == sChildName)
                    {
                        std::map<std::string, std::string> detail;
                        for( TiXmlElement * node3Level = node2Level->FirstChildElement(); node3Level; node3Level = node3Level->NextSiblingElement() )
                        {
                            if (! node3Level) break;
                            string sKey3 = node3Level->ValueStr();
                            string sValue3 = CxXml::text(node3Level);
                            detail[sKey3] = sValue3;
                        }
                        details[CxString::toString(iRow)] = detail;
                        ++iRow;
                    }
                    else
                    {
                        string sValue2 = CxXml::text(node2Level);
                        major[sKey2] = sValue2;
                    }
                }
            }
        }
        
        sTypeName = typeName(sMsgNo);
        eOperateType = localOperateType(sTypeName);
        //要么放内存对象库中统一处理，要么放外面处理
        if (sTypeName.size() > 0 && eOperateType == DataOperateType_Write)
        {
            if (iObjectCount == 0)
            {
                int iSource = (_protocol) ? _protocol->protocolId() : 0;
                CxMsObjectDataManager::beginReceived(sTypeName, iSource);
                bWriteBegin = true;
            }
            string sName = CxMsObjectTypeManager::name(sTypeName, major);
            int iReceived = 0;
            if (sType != "Main")
            {
                iReceived = CxMsObjectDataManager::receivedObject(sTypeName, sName, major);
            }
            else
            {
                iReceived = CxMsObjectDataManager::receivedObject(sTypeName, sName, major, details);
            }
            if (iReceived > 0)
            {
                ++iObjectCount;
            }
        }
        else
        {
            dealCicsXml(sMsgNo, major, details);
        }
    }
    if (bWriteBegin)
    {
        if (eOperateType == DataOperateType_Write)
        {
            CxMsObjectDataManager::endReceived();
        }
    }

    if (iObjectCount > 0)
    {
        if (sTypeName == CS_MsObject_TypeName_UserInfo)
        {
            _syncUserInfoTime = CxTime::currentMsepoch();
        }
        else if (sTypeName == CS_MsObject_TypeName_RoleInfo)
        {
            _syncPowerTime = CxTime::currentMsepoch();
        }

    }

    outPromptInfo() << "explain TypeName=" << sTypeName << ", MsObject Count=" << iObjectCount << cxEndLine;

    return iObjectCount;
}

int CicsBusiness::dealCicsXml(const string &sMsgNo, const std::map<string, string> &major, const std::map<string, std::map<string, string> > &details)
{
    HuayanZaixianshuifenFlow::cicsReceivedXmlContent(sMsgNo, major, details);
}

int CicsBusiness::requestUserInfo()
{
    map<string, string> contents;

    createHead(contents);
    contents["Type"] = "2";

    string sParam = XmlMsObjectTemplateManager::toSingleTemplateString("00303", contents);
    int r = sendMessage("00303", sParam);
    if (r > 0)
    {
        _queryUserInfoTime = CxTime::currentMsepoch();
    }
    return r;
}

int CicsBusiness::requestPower()
{
    map<string, string> contents;
    createHead(contents);
    contents["Type"] = "3";

    string sParam = XmlMsObjectTemplateManager::toSingleTemplateString("00303", contents);
    int r = sendMessage("00303", sParam);
    if (r > 0)
    {
        _queryPowerTime = CxTime::currentMsepoch();
    }
    return r;
}

void CicsBusiness::createHead(std::map<string, string> &sHeads)
{
    //    <MsgId>[MsgId]</MsgId>
    //    <MsgRef>[MsgRef]</MsgRef>
    //    <TransDate>[TransDate]</TransDate>

    sHeads["MsgId"]  = CxString::format("%03d%s",4,CxTime::toString(CxTime::currentMsepoch(), (char)0, (char)0, (char)0).c_str());
    sHeads["MsgRef"] = "";
    sHeads["TransDate"] = CxTime::toString(CxTime::currentMsepoch(), '-', ' ', ':');
}

CicsBusiness::CicsBusiness()
{
    _queryInfoTm = new CxTimer();
    _queryInfoTm->setTimerTimeOut(this);
    _queryInfoTm->setInterval(1000);
    _syncUserInfoTime = 0;
    _queryInfoInterval = 0;
    _queryUserInfoTime = 0;
    _queryPowerTime = 0;
}

CicsBusiness::~CicsBusiness()
{
    delete _queryInfoTm;
}

void CicsBusiness::toContext(CxISetSkv &context) const
{
    PsmBusiness::toContext(context);

}

void CicsBusiness::fromContext(const CxIGetSkv &context)
{
    PsmBusiness::fromContext(context);
    _queryInfoInterval = context.getValue(CS_Entry_QueryInfo, _queryInfoInterval);

}

int CicsBusiness::sendSingleTemplateMessage(const string &sTemplate, const std::map<string, string> &sHeads)
{
    string sParam = XmlMsObjectTemplateManager::toSingleTemplateString(sTemplate, sHeads);
    return sendMessage(sTemplate, sParam);
}

void CicsBusiness::toReport(std::vector<string> &sReports) const
{
    PsmBusiness::toReport(sReports);
}

void CicsBusiness::businessAfterStart()
{
    PsmBusiness::businessAfterStart();

    if (_queryInfoInterval > 0)
    {
        _queryInfoTm->start();
    }
    requestUserInfo();
}

void CicsBusiness::businessAfterStop()
{
    PsmBusiness::businessAfterStop();

    _queryInfoTm->stop();
}


//******************************************************************************************************

void CicsBusiness::received_message_command(const PsmProtocol *oPsm, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach * oAttach)
{
    PsmBusiness::received_message_command(oPsm, pCommand, pParam, iParamLength, oAttach);

    if (! pParam || iParamLength <= 0)
    {
        return;
    }

    PsmProtocol * oPsmProtocol = (PsmProtocol *)oPsm;
    GString sCommand = pCommand;
    sCommand = CxString::trim(sCommand," ");
    GString sParam = string(pParam, iParamLength);

    GString sKey = CxString::trim(sCommand);
    if(sKey.empty())sKey = "00000";

    //以下是转成文件发出去的
    int nRet = 0; //以数字字符串作为命令码 XML格式文件
    CxString::fromString(sKey,nRet);
    if(nRet>=0)
    {
        GString sFilePath = CxFileSystem::mergeFilePath( GclConfig::tempPath(),"Recv");
        CxFileSystem::createDirMultiLevel(sFilePath);
        GString sFileName = CxFileSystem::mergeFilePath(sFilePath,CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0)+"_"+sKey+".xml");
        if(CxFile::save(sFileName, sParam))
        {
            GString sBuff = CxString::format("fc=%s\nfile=%s",sKey.data(),sFileName.data());
            oPsmProtocol->sendMsg(GCL_CMD_SYS_XML_CICS,0,sBuff.length(),(char*)sBuff.data());
            outBusinessDebug()<<"sendMsg:"<<sBuff<<cxEndLine;
        }
    }

    //以下解释成 msObject
    explain(pParam, iParamLength);
}

void CicsBusiness::received_file_write(const PsmProtocol *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach * oAttach)
{
    PsmBusiness::received_file_write(oPsm, pathInfo, fileData, oAttach);

    GString sPath = CxFileSystem::mergeFilePath(GclConfig::tempPath(),"Recv");
    CxFileSystem::createDirMultiLevel(sPath);
    GString fileName = CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0)+"_"+ pathInfo.fileName;
    GString sFilePath = CxFileSystem::mergeFilePath(sPath ,fileName );

    bool bSave = CxFile::save(sFilePath, fileData, std::string());

    if(bSave && CxFileSystem::extractFileSuffixName(sFilePath)==".xml")
    {
        GString sBuff = CxString::format("fc=00000\nfile=%s",sFilePath.data());
        PsmProtocol * oPsmProtocol = (PsmProtocol *)oPsm;
        if(oPsmProtocol)oPsmProtocol->sendMsg(GCL_CMD_SYS_XML_CICS,0,sBuff.length(),(char*)sBuff.data());
    }

    string sFileData = CxString::join(fileData, 0);
    explain(sFileData.data(), sFileData.size());
}

void CicsBusiness::received_realtime_data_request(const PsmProtocol *oPsm, const PsmAttach *oAttach)
{
    PsmBusiness::received_realtime_data_request(oPsm,oAttach);

}

void CicsBusiness::received_realtime_data_post(const PsmProtocol *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    PsmBusiness::received_realtime_data_post(oPsm,iParamType,pData,iDataLength,iParamCount,oAttach);
}

void CicsBusiness::msObjects_changed(const string &sTypeName, const std::vector<string> &sNames)
{
    PsmBusiness::msObjects_changed(sTypeName, sNames);

    DataOperateType_Enum eOperateType = localOperateType(sTypeName);
    if (eOperateType != DataOperateType_Read)
        return;

    string sMsgNo = localTypeName(sTypeName);
    if (sMsgNo.empty())
    {
        outPromptInfo() << "msObjects_changed - TypeName : " << sTypeName << " to MsgNo fail !!!" << cxEndLine;
    }

    map<string, string> sHeads;
    createHead(sHeads);

    CxMsObjectType::StructType_Enum eLocalStructType = localStructType(sTypeName);
    vector<const CxMsObjectData *> r = CxMsObjectDataManager::findObjects(sTypeName);
    for (size_t i = 0; i < r.size(); ++i)
    {
        const CxMsObjectData * oObjectData = r.at(i);
        string sParam;
        map<string, string> sMajor = CxContainer::merge(oObjectData->major(), sHeads);
        if (eLocalStructType == CxMsObjectType::StructType_Simple)
        {
            sParam = XmlMsObjectTemplateManager::toSingleTemplateString(sMsgNo, sMajor, & _mapingFieldNames);
        }
        else if (eLocalStructType == CxMsObjectType::StructType_Main)
        {
            sParam = XmlMsObjectTemplateManager::toMainTemplateString(sMsgNo, sMajor, oObjectData->details(), & _mapingFieldNames);
        }
        sendMessage(sMsgNo, sParam);
    }
}

void CicsBusiness::timer_timeOut(const CxTimer *oTimer)
{
    if (oTimer == _queryInfoTm)
    {
        if (CxTime::milliSecondDifferToNow(_syncUserInfoTime) > _queryInfoInterval)
        {
            if (CxTime::milliSecondDifferToNow(_queryUserInfoTime) > 5000)
            {
//                requestUserInfo();
            }
        }
        else
        {
            if (CxTime::milliSecondDifferToNow(_syncPowerTime) > _queryInfoInterval
                    && CxTime::milliSecondDifferToNow(_queryPowerTime) > 5000)
            {
//                requestPower();
            }
        }
    }
}

