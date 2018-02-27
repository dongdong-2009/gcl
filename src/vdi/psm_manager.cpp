#include "psm_manager.h"

#include "terminal_exe.h"
#include "terminal_manager.h"
#include "user_master.h"

using namespace std;


PsmManager * fn_oPsmManager()
{
    TerminalExe::registAfterLoad(PsmManager::registerPsmProtocolNotify);
    CxInterinfoIn::addObserver(PsmManager::interinfoIn_line);
    CxInterinfoIn::addObserver(PsmManager::interinfoIn_cmd);
    static PsmManager m;
    return &m;
}

PsmManager * f_oPsmManager = fn_oPsmManager();


static map<int, int> f_remoteTerminalUserIds;
static map<int, int> f_localTerminalUserIds;


/**
 * sdk server test commands :


file send [fileName] : send file from directory [app_data/temp/fileName] to every client

message send [filePath\message_text] : send message command from directory [app_data/temp/fileName] to every client

timer -message xxx -file xxx -interval:: start\stop timer timer to send message, if [app_data/temp/fileName] exist then send message from file

yx send : send a test yx to every client

yc send : send a test yc to every client

 */

static std::string f_timerFilePath;
static std::string f_timerMessage;
static std::string f_timerParam;

int PsmManager::interinfoIn_line(const string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

int PsmManager::interinfoIn_cmd(const string &sCommand, const std::map<string, string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    if (sCommand == "file")
    {
        if (CxContainer::contain(sParams, std::string("send")))
        {
            std::string sSendValue = CxContainer::value(sParams, std::string("send"));
            std::string sFilePath;
//            sFilePath = "c:\\3.txt";
//            sendFile(sFilePath);
//            sFilePath = "c:\\2.txt";
//            sendFile(sFilePath);
//            sFilePath = "c:\\1.txt";
//            sendFile(sFilePath);
            sFilePath = "c:\\0.txt";
            if (CxFileSystem::isExist(sSendValue))
                sFilePath = sSendValue;
            postFile(sFilePath);
        }
        return CxInterinfo::InDeal_Ed;
    }
    else if (sCommand == "message")
    {
        if (CxContainer::contain(sParams, std::string("send")))
        {
            std::string sSendValue = CxContainer::value(sParams, std::string("send"));
            if (sSendValue.size() > 0)
            {
                std::string sFilePath = CxFileSystem::mergeFilePath( GclConfig::tempPath() , sSendValue );
                if (CxFileSystem::isExist(sFilePath))
                {
                    std::vector<std::string> sLines;
                    CxFile::load(sFilePath, sLines);
                    if (sLines.size()>0)
                    {
                        std::string rCommand = sLines.at(0);
                        sLines.erase(sLines.begin());
                        std::string rParam = CxString::join(sLines, cs_line_string);
                        postMessage(rCommand, rParam);
                    }
                }
                else
                {
                    postMessage(sSendValue, "");
                }
            }
        }
        return CxInterinfo::InDeal_Ed;
    }
    else if (sCommand == "timer")
    {
        std::string sMessage = CxContainer::value(sParams, std::string("message"));
        std::string sParam = CxContainer::value(sParams, std::string("param"));
        std::string sFile = CxContainer::value(sParams, std::string("file"));
        int iInterval = 0;
        iInterval = CxContainer::valueTo(sParams, std::string("interval"), iInterval);

        if (iInterval > 10)
        {
            if (sFile.size() > 0)
            {
                if (CxFileSystem::isExist(sFile))
                {
                    f_timerFilePath = sFile;
                }
                else if (CxFileSystem::isExist(CxFileSystem::mergeFilePath( GclConfig::tempPath() , sFile )))
                {
                    f_timerFilePath = CxFileSystem::mergeFilePath( GclConfig::tempPath() , sFile );
                }
            }
            else
            {
                f_timerFilePath = "";
            }
            f_timerMessage = sMessage;
            f_timerParam = sParam;
            CxTimerManager::stopTimer(PsmManager::timeOutTest);
            CxTimerManager::startTimer(PsmManager::timeOutTest, iInterval);
        }
        else
        {
            CxTimerManager::stopTimer(PsmManager::timeOutTest);
        }
        return CxInterinfo::InDeal_Ed;
    }
    return 0;
}

PsmProtocol *PsmManager::getPsmProtocol(int iLocalTerminalId)
{
    TerminalBase * oTerminal = TerminalManager::terminal(iLocalTerminalId);
    if (oTerminal && oTerminal->protocol() && oTerminal->protocol()->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
    {
        return (PsmProtocol*)oTerminal->protocol();
    }
    return NULL;
}

int PsmManager::postMessageByTerminalId(int iLocalTerminalId, int iRemoteTerminalId, const string &sCommand, const string &sParam)
{
    PsmProtocol* oPsmProtocol = getPsmProtocol(iLocalTerminalId);
    cxPromptCheck(oPsmProtocol, return -1);
    return oPsmProtocol->postMessageCommand(iRemoteTerminalId, sCommand.c_str(), sParam.data(), sParam.size());
}

int PsmManager::postMessage(const std::string &sCommand, const std::string &sParam, const PsmAttach *oAttach)
{
    int iSent = 0;
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase* oTerminal = oTerminals->at(i);
        if (oTerminal && oTerminal->protocol() && oTerminal->protocol()->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
        {
            PsmProtocol* oPsmProtocol = (PsmProtocol*)oTerminal->protocol();
            iSent += oPsmProtocol->postMessageCommand(sCommand.c_str(), sParam.data(), sParam.size(), oAttach);
        }
    }
    return iSent;
}

int PsmManager::postFileByTerminalId(int iLocalTerminalId, int iRemoteTerminalId, const string &sFilePath)
{
    PsmProtocol* oPsmProtocol = getPsmProtocol(iLocalTerminalId);
    cxPromptCheck(oPsmProtocol, return -1);
    return oPsmProtocol->postFile(iRemoteTerminalId, sFilePath);
}

int PsmManager::postFile(const std::string &sFilePath, const PsmAttach * oAttach)
{
    int iSent = 0;
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase* oTerminal = oTerminals->at(i);
        if (oTerminal && oTerminal->protocol() && oTerminal->protocol()->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
        {
            PsmProtocol* oPsmProtocol = (PsmProtocol*)oTerminal->protocol();
            iSent += oPsmProtocol->postFile(sFilePath, oAttach);
        }
    }
    return iSent;
}

int PsmManager::postRealtimeDataRequestByTerminalId(int iLocalTerminalId, int iRemoteTerminalId)
{
    PsmProtocol* oPsmProtocol = getPsmProtocol(iLocalTerminalId);
    cxPromptCheck(oPsmProtocol, return -1);
    return oPsmProtocol->postRealtimeDataRequest(iRemoteTerminalId);
}

int PsmManager::postRealtimeDataRequest(const PsmAttach * oAttach)
{
    int iSent = 0;
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase* oTerminal = oTerminals->at(i);
        if (oTerminal && oTerminal->protocol() && oTerminal->protocol()->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
        {
            PsmProtocol* oPsmProtocol = (PsmProtocol*)oTerminal->protocol();
            iSent += oPsmProtocol->postRealtimeDataRequest(oAttach);
        }
    }
    return iSent;
}

int PsmManager::postRealtimeDataPostByTerminalId(int iLocalTerminalId, int iRemoteTerminalId, int iParamType, const char *pData, int iDataLength, int iParamCount)
{
    PsmProtocol* oPsmProtocol = getPsmProtocol(iLocalTerminalId);
    cxPromptCheck(oPsmProtocol, return -1);
    return oPsmProtocol->postRealtimeDataPost(iRemoteTerminalId, iParamType, pData, iDataLength, iParamCount);
}

int PsmManager::postRealtimeDataPost(int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach)
{
    int iSent = 0;
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase* oTerminal = oTerminals->at(i);
        if (oTerminal && oTerminal->protocol() && oTerminal->protocol()->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
        {
            PsmProtocol* oPsmProtocol = (PsmProtocol*)oTerminal->protocol();
            iSent += oPsmProtocol->postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, oAttach);
        }
    }
    return iSent;
}

void PsmManager::timeOutTest(int iInterval)
{
    if (f_timerFilePath.size() > 0)
    {
        if (CxFileSystem::isExist(f_timerFilePath))
        {
            std::vector<std::string> sLines;
            CxFile::load(f_timerFilePath, sLines);
            if (sLines.size()>0)
            {
                std::string rCommand = sLines.at(0);
                sLines.erase(sLines.begin());
                std::string rParam = CxString::join(sLines, cs_line_string);
                postMessage(rCommand, rParam);
            }
        }
    }
    else if (f_timerMessage.size() > 0)
    {
        postMessage(f_timerMessage, f_timerParam);
    }
}

void PsmManager::registerPsmProtocolNotify()
{
    addObserver(f_oPsmManager);
}

void PsmManager::addObserver(IPsmProtocolNotify *oNotify)
{
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase* oTerminal = oTerminals->at(i);
        if (oTerminal && oTerminal->protocol() && oTerminal->protocol()->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
        {
            PsmProtocol* oPsmProtocol = (PsmProtocol*)oTerminal->protocol();
            oPsmProtocol->addObserver(oNotify);
        }
    }
}

void PsmManager::removeObserver(IPsmProtocolNotify *oNotify)
{
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase* oTerminal = oTerminals->at(i);
        if (oTerminal && oTerminal->protocol() && oTerminal->protocol()->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
        {
            PsmProtocol* oPsmProtocol = (PsmProtocol*)oTerminal->protocol();
            oPsmProtocol->removeObserver(oNotify);
        }
    }
}

volatile bool f_bLogining = false;
int f_iLoginingLocalTerminalId = 0;


int fn_waitLocalTerminalUserId()
{
    int iUserId = f_localTerminalUserIds[f_iLoginingLocalTerminalId];
    return iUserId != 0;
}

int PsmManager::userLoginServer(int iLocalTerminalId, int iRemoteTerminalId, const string &sUserName, const string &sPassword, int iWaittime)
{
    if (f_bLogining) return 0;
    f_bLogining = true;
    string sParam = CxString::format("user_name=%s;password=%s", sUserName.c_str(), sPassword.c_str());
    int iResult = postMessageByTerminalId(iLocalTerminalId, iRemoteTerminalId, GM_req_sys_login_1, sParam);
    if (iResult > 0)
    {
        //设本地去登录的 terminalId
        f_iLoginingLocalTerminalId = iLocalTerminalId;
        f_localTerminalUserIds[f_iLoginingLocalTerminalId] = 0;
        CxApplication::waiting(iWaittime, fn_waitLocalTerminalUserId);
    }
    f_bLogining = false;
    return f_localTerminalUserIds[f_iLoginingLocalTerminalId];
}

int PsmManager::userLoginServer(int iLocalTerminalId, int iRemoteTerminalId, const string &sIccardCode, int iWaittime)
{
    if (f_bLogining) return 0;
    f_bLogining = true;
    string sParam = CxString::format("card_code=%s", sIccardCode.c_str());
    int iResult = postMessageByTerminalId(iLocalTerminalId, iRemoteTerminalId, GM_req_sys_login_1, sParam);
    if (iResult > 0)
    {
        //设本地去登录的 terminalId
        f_iLoginingLocalTerminalId = iLocalTerminalId;
        f_localTerminalUserIds[f_iLoginingLocalTerminalId] = 0;
        CxApplication::waiting(iWaittime, fn_waitLocalTerminalUserId);
    }
    f_bLogining = false;
    return f_localTerminalUserIds[f_iLoginingLocalTerminalId];
}

void PsmManager::received_message_command(const PsmProtocol * oPsm, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach * oAttach)
{
    //req.sys.login.1\0
    //card_code=
    //user_name=
    //password=
    string sCommand(pCommand);
    if (sCommand == GM_req_sys_login_1)
    {
        cxPromptCheck(oAttach,return);
        int iLocalTerminalId = oAttach->targetId;
        int iRemoteTerminalId = oAttach->sourceId;
        PsmProtocol * oPsmProtocol = getPsmProtocol(iLocalTerminalId);
        cxPromptCheck(oPsmProtocol,return);

        map<string, string> sParams = CxString::splitToMap(string(pParam, iParamLength), '=', ';');
        string sIccardCode = CxContainer::value(sParams, string("card_code"));
        string sUserName = CxContainer::value(sParams, string("user_name"));
        string sPassword = CxContainer::value(sParams, string("password"));

        string sSql;
        if (sIccardCode.size() > 0)
        {
            sSql = CxString::format("select user_id as counter from user where cardcode = '%s'", sIccardCode.c_str());
        }
        else
        {
            sSql = CxString::format("select user_id as counter from user where user_name = '%s' and password = '%s'", sUserName.c_str(), sPassword.c_str());
        }
        CxDatabase * oDb = CxDatabase::getDefaultDb();
        string sUserId = oDb ? oDb->queryValue(sSql) : string();
        int iUserId = -1;
        if (sUserId.size() > 0)
        {
            iUserId = CxString::toInt32(sUserId);
            f_remoteTerminalUserIds[iRemoteTerminalId] = iUserId;
        }
        string sParam = CxString::format("user_id=%d", iUserId);
        oPsmProtocol->postMessageCommand(iRemoteTerminalId, GM_resp_sys_login_1.c_str(), sParam.data(), sParam.size());
    }
    else if (sCommand == GM_resp_sys_login_1)
    {
//        cxPromptCheck(oAttach);
        int iLocalTerminalId = oAttach->targetId;
//        int iRemoteTerminalId = oAttach->sourceId;
        TerminalBase * oTerminal = TerminalManager::terminal(iLocalTerminalId);
//        cxPromptCheck(oTerminal);

        map<string, string> sParams = CxString::splitToMap(string(pParam, iParamLength), '=', ';');
        string sUserId = CxContainer::value(sParams, string("user_id"));
        if (sUserId.size() > 0)
        {
            int iUserId = CxString::toInt32(sUserId);
            f_localTerminalUserIds[iLocalTerminalId] = iUserId;
        }
    }
}

void PsmManager::received_file_write(const PsmProtocol * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach)
{
}

void PsmManager::received_realtime_data_request(const PsmProtocol *oPsm, const PsmAttach * oAttach)
{
}

void PsmManager::received_realtime_data_post(const PsmProtocol *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach * oAttach)
{



}
