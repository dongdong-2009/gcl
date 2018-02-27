#include "ruixiang_business.h"

#include "huayan_zaixianshuifen_flow.h"


RuixiangBusinessFactory* fn_regRuixiangBusinessFactory()
{
    TBIFactoryManager::createAndRegister<RuixiangBusinessFactory>();
    return NULL;
}

RuixiangBusinessFactory* RuixiangBusinessFactory::s_instance = fn_regRuixiangBusinessFactory();


using namespace std;


static const string HYSBH = "HYSBH";
static const string TerminalNumber = "TerminalNumber";

RuixiangBusiness::RuixiangBusiness()
{
}

RuixiangBusiness::~RuixiangBusiness()
{
}

void RuixiangBusiness::toContext(CxISetSkv &context) const
{
    PsmBusiness::toContext(context);
//    context.setValue(CS_EntryPsmStationTypeSource, _cfgSourceType);

}

void RuixiangBusiness::fromContext(const CxIGetSkv &context)
{
    PsmBusiness::fromContext(context);

    string sRequestTypies;
    sRequestTypies = context.getValue(CS_Entry_RequestTypies, sRequestTypies);
    _requestTypies = CxString::splitToMap(sRequestTypies, '=', ',');

    string sKeyTsValues;
    sKeyTsValues = context.getValue(CS_Entry_KeyTsValues, sKeyTsValues);
    _keyTsValues = CxString::splitToMap(sKeyTsValues, '=', ',');

    string sTypeFileNames;
    sTypeFileNames = context.getValue(CS_Entry_TypeFileNames, sTypeFileNames);
    _typeFileNames = CxString::splitToMap(sTypeFileNames, '=', ',');

    _deviceCode = context.getValue(CS_Entry_DeviceCode, sTypeFileNames);
    _terminalCode = context.getValue(CS_Entry_TerminalCode, sTypeFileNames);

}

void RuixiangBusiness::toReport(std::vector<string> &sReports) const
{
    PsmBusiness::toReport(sReports);

}

void RuixiangBusiness::businessAfterStart()
{
    PsmBusiness::businessAfterStart();
}

void RuixiangBusiness::businessAfterStop()
{
    PsmBusiness::businessAfterStop();
}

//******************************************************************************************************

//user_check
//喻汉楼：04ED885B7D
//欧阳 ： 04842F286F 303438324632383646
//孔： 04C4C1B26E
void RuixiangBusiness::received_message_command(const PsmProtocol *oPsm, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach * oAttach)
{
    PsmBusiness::received_message_command(oPsm, pCommand, pParam, iParamLength, oAttach);

    std::string sCommand = pCommand;

    HuayanZaixianshuifenFlow::ruixianReceivedCommandMessage(sCommand, pParam, iParamLength);

    string sTypeName = typeName(sCommand);
    std::vector<string> sNames;
    DataOperateType_Enum eOperateType = localOperateType(sTypeName);
    if (eOperateType == DataOperateType_Read)
    {
        string sFileName = CxContainer::value(_typeFileNames, sTypeName);
        if (sFileName.size() > 0)
        {
            sendFileToRuiXiang(sTypeName, sNames, sFileName);
        }
        else
        {
            sendMessageToRuiXiang(sTypeName, sNames);
        }

    }
    else if (sCommand == "quest_user_info" )
    {
        sendFileToRuiXiang(CS_MsObject_TypeName_UserInfo, sNames, "userinfo.ini");
    }
    else if (sCommand == "quest_user_power")
    {
        sendFileToRuiXiang(CS_MsObject_TypeName_RoleInfo, sNames, "powers.ini");
    }
    else if (sTypeName == "Req_UserLogin")
    {
        std::string sParam(pParam, iParamLength);
        string sCardNumber = CxString::trim(CxString::token(sParam, '\n'));
        _cardNumber = sCardNumber;

        int iSource = (_protocol) ? _protocol->protocolId() : 0;
        CxMsObjectDataManager::beginReceived(sTypeName, iSource);

        std::map<std::string, std::string> major;
        std::map<std::string, std::map<std::string, std::string> > details;
        string sName = sCardNumber;
        major[HYSBH] = _deviceCode;
        major[TerminalNumber] = _terminalCode;
        major["CardNumber"] = sCardNumber;
        CxMsObjectDataManager::receivedObject(sTypeName, sName, major, details);
        CxMsObjectDataManager::endReceived();

        if (CxMsObjectDataManager::find(CS_MsObject_TypeName_UserInfo, "CardNumber", _cardNumber) != NULL)
        {
            sendMessage("reply_user_check", "1");
        }

        outPromptInfo() << sTypeName << " CardNumber : " << sCardNumber << cxEndLine;
    }
    else
    {
        if (sTypeName.size() > 0)
        {
            int iSource = (_protocol) ? _protocol->protocolId() : 0;
            CxMsObjectDataManager::beginReceived(sTypeName, iSource);

            std::map<std::string, std::string> major;
            std::map<std::string, std::map<std::string, std::string> > details;
            string sName = _deviceCode;
            if (CxContainer::contain(_requestTypies, sTypeName))
            {
                major["Type"] = CxContainer::value(_requestTypies, sTypeName);
            }
            major[HYSBH] = _deviceCode;
            major[TerminalNumber] = _terminalCode;
            CxMsObjectDataManager::receivedObject(sTypeName, sName, major, details);
            CxMsObjectDataManager::endReceived();
        }
        else
        {
            cxWarning() << "received message command, but " << sCommand << " no typeName!!!" << cxEndLine;
        }
    }

}

void RuixiangBusiness::received_file_write(const PsmProtocol *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach)
{
    PsmBusiness::received_file_write(oPsm, pathInfo, fileData, oAttach);

    vector<string> sLines = CxString::split(fileData, cs_line_string);
    if (! explain(sLines, pathInfo.fileName))
    {
        cxWarning() << "explain fileData fail !" << cxEndLine;
    }
}

void RuixiangBusiness::msObjects_changed(const string &sTypeName, const std::vector<string> & sNames)
{
    PsmBusiness::msObjects_changed(sTypeName, sNames);

//写：表示写到内存对象库
//读：表示从内存对象库中读出来，发出去
    DataOperateType_Enum eOperateType = localOperateType(sTypeName);
    if (eOperateType != DataOperateType_Read)
        return;

    string sFileName = CxContainer::value(_typeFileNames, sTypeName);
    if (sFileName.size() > 0)
    {
        sendFileToRuiXiang(sTypeName, sNames, sFileName);
    }
    else
    {
        sendMessageToRuiXiang(sTypeName, sNames);
    }
}

bool RuixiangBusiness::saveToFile(const std::vector<const CxMsObjectData *> &msObjects, const string &sFilePath)
{
    vector<string> sLines;
    string sLine;
    for (size_t i = 0; i < msObjects.size(); ++i)
    {
        const CxMsObjectData * oMsObject = msObjects.at(i);
        const std::map<std::string, std::string> & major = oMsObject->major();
        const std::map<std::string, std::map<std::string, std::string> > details = oMsObject->details();

        if (oMsObject->name().size() > 0)
        {
            sLine = "[";
            sLine += oMsObject->name();
            sLine += "]";
            sLines.push_back(sLine);
            for (map<string, string>::const_iterator it = major.begin(); it != major.end(); ++it)
            {
                std::string sKeyName = it->first;
                sKeyName = localFieldName(sKeyName);
                sLine = sKeyName + "=" + it->second;
                sLines.push_back(sLine);
            }
            sLines.push_back("");
        }

        for (std::map<std::string, std::map<std::string, std::string> >::const_iterator it1 = details.begin(); it1 != details.end(); ++it1)
        {
            std::string sDetailName = it1->first;
            sDetailName = localFieldName(sDetailName);
            const std::map<std::string, std::string> & detail = it1->second;
            sLine = "[";
            sLine += sDetailName;
            sLine += "]";
            sLines.push_back(sLine);
            for (map<string, string>::const_iterator it2 = major.begin(); it2 != major.end(); ++it2)
            {
                sLine = it2->first + "=" + it2->second;
                sLines.push_back(sLine);
            }
            sLines.push_back("");
        }
    }
    return CxFile::save(sFilePath, CxEncoding::utf8ToGb2312(sLines));
}

int RuixiangBusiness::explain(const std::vector<std::string> &  sLines, const std::string & sFileName)
{
    const CxMsObjectType * oObjectType = NULL;
    std::string sTypeName;
    std::string sSection;
    std::string sEntry;
    std::string sValue;

    std::map<std::string, std::string> major;
    std::map<std::string, std::map<std::string, std::string> > details;
    std::map<std::string, std::string> entrys;
    for (size_t i = 0; i < sLines.size(); ++i)
    {
        string sLine =  CxString::trim(sLines.at(i));
        if (sLine.empty()) continue;
        switch (sLine[0])
        {
        case '[':
        {
            if (entrys.size() > 0 && sSection.size() > 0)
            {
                if (CxContainer::containValue(_mapingTypeNames, sSection))
                    major = entrys;
                else
                    details[sSection] = entrys;
            }
            sSection = CxString::unquote(sLine, '[', ']');
            entrys = map<string, string>();
            //收到的section转成
            if (CxContainer::contain(_keyTsValues, sSection))
            {
                entrys[string("ZBMC")] = sSection;
                sSection = "Stad";
                sTypeName = typeName(sSection);
                oObjectType = CxMsObjectTypeManager::find(sTypeName);
            }
        }
            break;
        case '#':
        case ';':
        {
        }
            break;
        default:
        {
            bool bHas;
            sValue = CxString::tokenLeft(sLine, '=', &bHas);
            if (! bHas)
            {
                break;
            }
            sValue = CxString::trim(sValue);
            sEntry = CxString::trim(sLine);
            if (sSection.size() > 0 && sEntry.size() > 0)
            {
                if (CxContainer::contain(_keyTsValues, sEntry))
                {
                    entrys[CxContainer::value(_keyTsValues, sEntry)] = sEntry;
                    entrys["ZBZ"] = sValue;
                }
                else
                {
                    entrys[sEntry] = sValue;
                }
            }
        }
            break;
        }
    }
    if (entrys.size() > 0 && sSection.size() > 0)
    {
        if (CxContainer::containValue(_mapingTypeNames, sSection))
            major = entrys;
        else
            details[sSection] = entrys;
    }
    if (! oObjectType)
        return false;
    int iSource = (_protocol) ? _protocol->protocolId() : 0;
    CxMsObjectDataManager::beginReceived(sTypeName, iSource);
    for (std::map<std::string, std::map<std::string, std::string> >::const_iterator it1 = details.begin(); it1 != details.end(); ++it1)
    {
        //sTypeName
        string sNameValue;
        map<std::string, std::string> majorNew = major;
//        const std::string & sDetailName = it1->first;
        const std::map<std::string, std::string> & detail = it1->second;
        for (map<string, string>::const_iterator it2 = detail.begin(); it2 != detail.end(); ++it2)
        {
            const string & sKey = it2->first;
            const string & sValue = it2->second;
            majorNew[sKey] = sValue;
            if (oObjectType->detailKey() == sKey)
            {
                sNameValue = sValue;
            }
        }
        if (sNameValue.size() > 0)
        {
            CxMsObjectDataManager::receivedObject(sTypeName, sNameValue, majorNew);
        }
    }
    CxMsObjectDataManager::endReceived();
    return true;
}

bool RuixiangBusiness::sendFileToRuiXiang(const string &sTypeName, const std::vector<string> &sNames, const string &sFileName)
{
    int r = false;
    vector<const CxMsObjectData *> oMsObjects;
    if (sNames.size() > 0)
        oMsObjects = CxMsObjectDataManager::findObjects(sTypeName, sNames);
    else
        oMsObjects = CxMsObjectDataManager::findObjects(sTypeName);
    if (oMsObjects.size() > 0)
    {
        string sFilePath = CxFileSystem::mergeFilePath( GclConfig::tempPath(), sFileName );
        if ( saveToFile(oMsObjects, sFilePath) )
        {
            r = sendFile(sFilePath);
        }
        else
        {
            cxWarning() << "can save file " << sFilePath << cxEndLine;
        }
    }
    else
    {
        cxWarning() << "can not find " << sTypeName << cxEndLine;
    }
    return r;
}

bool RuixiangBusiness::sendMessageToRuiXiang(const string &sTypeName, const std::vector<string> &sNames)
{
    int r = false;
    vector<const CxMsObjectData *> msObjects = CxMsObjectDataManager::findObjects(sTypeName, sNames);
    if (msObjects.size() > 0)
    {
        for (size_t i = 0; i < msObjects.size(); ++i)
        {
            const CxMsObjectData * msObject = msObjects.at(i);

            if (msObject->typeName() == "Resp_UserLogin")
            {
                string sTerminalNumber = CxContainer::value(msObject->major(), string("TerminalNumber"));
                if (CxString::equalIgnoreAll(_terminalCode, sTerminalNumber))
                {
                    if (CxMsObjectDataManager::find(CS_MsObject_TypeName_UserInfo, "CardNumber", _cardNumber) != NULL)
                    {
                        r = sendMessage("reply_user_check", "1");
                    }
                    else
                    {
                        r = sendMessage("reply_user_check", "0");
                    }

                    string sUserToken = CxContainer::value(msObject->major(), string("UserToken"));
                    sUserToken.append("\n0");
                    r = sendMessage("post_token", sUserToken);
                }
                else
                {
                    outPromptInfo() << "Resp_UserLogin , but TerminalNumber not equal!!! " << cxEndLine;
                }
            }
            else if (msObject->typeName() == "Resp_HuayanResult_Exist")
            {
                string sHYSBH = CxContainer::value(msObject->major(), string("HYSBH"));
                string sState = CxContainer::value(msObject->major(), string("State"));
                string sSBHYID = CxContainer::value(msObject->major(), string("SBHYID"));
                r = sendMessage("reply_test_result_commit_status", sHYSBH + "\n" + sSBHYID + "\n" + sState);
            }
        }
    }
    else
    {
        cxWarning() << "can not find " << sTypeName << cxEndLine;
    }
    return r;
}



