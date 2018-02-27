#include "iec104.h"

#include <terminal_exe.h>
#include <terminal_manager.h>

using namespace std;

static IEC_104 f_main;

int IEC_104::start(int argc, char *argv[])
{
    CxApplication::init(argc, argv);

//    CxConsoleInterinfo::addObserver(&f_main);

    TerminalExe::load(argc, argv);

    reg();

    TerminalExe::start();

    atexit(IEC_104::exitServer);

    int iResult = CxApplication::exec(1);

    TerminalExe::stop();

    return iResult;
}

int IEC_104::reg()
{
    int iResult = 0;
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase* oTerminal = oTerminals->at(i);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol->typeSimpleName() == CS_ProtocolTypeSimpleName_IEC104_Master)
        {
            IEC104Protocol* iProtocol = (IEC104Protocol*)oProtocol;
//            iProtocol->registerReceivedMessageNotify(& f_sdkServer);
//            iProtocol->registerReceivedFileNotify(& f_sdkServer);
            ++iResult;
        }
    }
    return iResult;
}
void IEC_104::exitServer()
{
    CxApplication::exit();
}
//int SdkServer::testSendMessage(const string &sMsg, const string &sParam)
//{
//    int iSent = 0;
//    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
//    for (size_t i = 0; i < oTerminals->size(); ++i)
//    {
//        TerminalBase* oTerminal = oTerminals->at(i);
//        ProtocolBase* oProtocol = oTerminal->protocol();
//        if (oProtocol->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
//        {
//            PsmProtocol* oPsmProtocol = (PsmProtocol*)oProtocol;
//            if (oPsmProtocol->channel() && oPsmProtocol->channel()->connected())
//            {
//                iSent += oPsmProtocol->sendMessageCommand(sMsg.data(), sParam.c_str(), sParam.size());
//            }
//        }
//    }
//    return iSent;
//}



//void SdkServer::received_message_command(const PsmProtocol *oPsm, const char *sCommand, const char *sParam, int iParamLength, const PsmAttach &attach)
//{
//    assert(oPsm != NULL);

//    PsmProtocol * oPsmProtocol = (PsmProtocol *)oPsm;
//    string sCommand1 = sCommand;
//    string sParam1(sParam, iParamLength);
//    vector<string> sParams1 = CxString::split(sParam1, '\n');

//    if (sCommand1 == "quest_test_result_commit_status")
//    {
//        string sCommand2 = "reply_test_result_commit_status";
//        string sParam2 = "0\n0";
//        if (sParams1.size() > 1)
//        {
//            sParam2 = sParams1[0]+"\n"+sParams1[1]+"\n1";
//        }
//        oPsmProtocol->sendMessageCommand(sCommand2.c_str(), sParam2.c_str(), sParam2.size());
//    }
//    else if (sCommand1 == "quest_user_check")
//    {
//        string sCommand2 = "reply_user_check";
//        string sParam2 = "1";
//        oPsmProtocol->sendMessageCommand(sCommand2.c_str(), sParam2.c_str(), sParam2.size());
//    }
//    else if (sCommand1 == "quest_user_info")
//    {
//        string sFilePath = CxFileSystem::mergeFilePath( GclConfig::tempPath() , "user_info.ini" );
//        oPsmProtocol->sendFilePath(sFilePath);
//    }
//    else if (sCommand1 == "quest_user_power")
//    {
//        string sFilePath = CxFileSystem::mergeFilePath( GclConfig::tempPath() , "user_power.ini" );
//        oPsmProtocol->sendFilePath(sFilePath);
//    }

//    cxPrompt() << "SdkServer - received_message_command : " << sCommand << cxEndLine;
//}

//void SdkServer::received_file(const PsmProtocol *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach &attach)
//{
//    string sFilePath = CxFileSystem::mergeFilePath( GclConfig::tempPath() , pathInfo.fileName );
//    bool bSave = CxFile::save(sFilePath, fileData, std::string());
//    cxPrompt() << "SdkServer - received_file - save as : " << sFilePath << " --> save " << ((bSave)?"success":"fail") << cxEndLine;
//}

//void SdkServer::interinfo_in_line(const string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
//{
//    vector<string> sArgvs = CxString::split(sInfo, ' ');
//    if (sArgvs.size() < 2) return;
//    const string & sCommand = sArgvs[0];
//    const string & sPurpose = sArgvs[1];
//    if (sCommand == "system")
//    {
//        if (sPurpose == "exit")
//        {
//            CxApplication::exit();
//        }
//    }
//    else if (sCommand == "query")
//    {
//        TerminalBase* oTerminal = TerminalManager::terminal(sPurpose);
//        if (oTerminal)
//        {
//            std::vector<std::string> sReports = oTerminal->reportSelf();
//            cxPrompt() << sReports << cxEndLine;
//        }
//        cxPrompt() << "SdkServer : " << sInfo << cxEndLine;
//    }
//    else if (sCommand == "send")
//    {
//        string sParam;
//        if (sArgvs.size() > 2)
//            sParam = sArgvs[2];
//        testSendMessage(sPurpose, sParam);
//    }
//    else if (sCommand == "stop")
//    {
//        if (sPurpose == "interinfo.packet")
//        {
//            CxChannelManager::setPacketInterinfoOut(false);
//        }
//    }
//    else if (sCommand == "start")
//    {
//        if (sPurpose == "interinfo.packet")
//        {
//            CxChannelManager::setPacketInterinfoOut(false);
//        }
//    }
//}


