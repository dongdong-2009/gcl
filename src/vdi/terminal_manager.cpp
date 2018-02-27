#include "terminal_manager.h"


#include "send_message_cases.h"
#include "send_file_cases.h"
#include "user_master.h"


using namespace std;


static std::vector<TerminalBase*> f_mTerminals;
static bool f_isStarted = false;


TerminalManager * fn_oTerminalManager()
{
    CxInterinfoIn::addObserver(TerminalManager::interinfoIn_line);
    CxInterinfoIn::addObserver(TerminalManager::interinfoIn_cmd);
    static TerminalManager m;
    return &m;
}

TerminalManager * f_oTerminalManager = fn_oTerminalManager();

/*

[terminal]
terminalName=Sample
protocol=CdtProtocol


[protocol]
channel=Serial
cases=ReceiveMonsbCases
cases=SendMonsbCases
cases=SendControlCases
cases=SendYcaddCases
Table=MonsbReceiveSendTable,YcaddReceiveSendTable

[channel]

[ReceiveMonsbCases]

[SendMonsbCases]

[SendControlCases]

[SendYcaddCases]

[MonsbReceiveSendTable]
TableFileName=Sample.MonsbReceiveSendTable.data

[YcaddReceiveSendTable]
TableFileName=Sample.YcaddReceiveSendTable.data

*/


std::string TerminalManager::protocolName(const std::string &sTerminalName)
{
    return sTerminalName + ".Protocol";
}

std::string TerminalManager::protocolName(const TerminalBase *oTerminal)
{
    if (oTerminal)
        return protocolName(oTerminal->terminalName());
    else
        return "";
}

std::string TerminalManager::businessName(const std::string &sTerminalName)
{
    return sTerminalName + ".Business";
}

std::string TerminalManager::businessName(const TerminalBase *oTerminal)
{
    if (oTerminal)
        return businessName(oTerminal->terminalName());
    else
        return "";
}

std::string TerminalManager::channelName(const std::string &sProtocolName)
{
    return sProtocolName + ".Channel";
}

std::string TerminalManager::channelName(TerminalBase *oTerminal)
{
    if (oTerminal && oTerminal->protocol())
        return channelName(oTerminal->protocol());
    else
        return "";
}

std::string TerminalManager::channelName(const ProtocolBase* oProtocol)
{
    if (oProtocol)
        return channelName(oProtocol->protocolName());
    else
        return "";
}

std::string TerminalManager::workerTableFileName(TerminalBase *oTerminal, const std::string &sTableName)
{
    if (oTerminal && oTerminal->protocol())
        return workerTableFileName(oTerminal->protocol()->protocolName(), sTableName);
    else
        return "";
}

std::string TerminalManager::workerTableFileName(const ProtocolBase *oProtocol, const std::string &sTableName)
{
    if (oProtocol)
        return workerTableFileName(oProtocol->protocolName(), sTableName);
    else
        return "";
}

std::string TerminalManager::workerTableFileName(const std::string &sProtocolName, const std::string &sTableName)
{
    return sProtocolName + "." + sTableName + ".txt";
}

TerminalBase * TerminalManager::createTerminal(const std::string &sTerminalName, const std::string &sTerminalTypeSimpleName, const std::string &sProtocolTypeSimpleName, const std::string& sBusinessTypeSimpleName, const std::string &sChannelTypeSimpleName)
{
    if (sTerminalName.empty() || sTerminalTypeSimpleName.empty() || sProtocolTypeSimpleName.empty() )
        return NULL;
    TerminalBase* oTerminal = terminal(sTerminalName);
    if (! oTerminal) {
        oTerminal = TerminalFactoryManager::createObject(sTerminalTypeSimpleName);
        if (oTerminal) {
            oTerminal->setTerminalName(sTerminalName);
            ProtocolBase* oProtocol = ProtocolFactoryManager::createObject(sProtocolTypeSimpleName);
            if (oProtocol)
            {
                std::string sProtocolName = protocolName(sTerminalName);
                oProtocol->setProtocolName(sProtocolName);
                oProtocol->setInfoTitle(sTerminalName + "." + oProtocol->typeSimpleName());
                CxChannelBase* oChannel  = CxChannelManager::createChannel(sChannelTypeSimpleName);
                if (oChannel)
                {
                    oChannel->setChannelName(channelName(sProtocolName));
                    oProtocol->setChannel(oChannel);
                }
                oTerminal->setProtocol(oProtocol);
                TBIBase* oBusiness = TBIFactoryManager::createObject(sBusinessTypeSimpleName);
                if (oBusiness)
                {
                    std::string sBusinessName = businessName(sTerminalName);
                    oBusiness->setBusinessName(sBusinessName);
                    oBusiness->setBusinessId(f_mTerminals.size()+1);
                    oTerminal->setBusiness(oBusiness);
                    oBusiness->setProtocol(oProtocol);
                }
                append( oTerminal );
                return oTerminal;
            }
            delete oTerminal;
        }
    }
    return NULL;
}

const std::vector<TerminalBase *> *TerminalManager::terminals()
{
    return & f_mTerminals;
}

std::vector<TerminalBase *> TerminalManager::terminals(const std::vector<string> &sTerminalNames)
{
    std::vector<TerminalBase *> mTerminals;
    for (size_t i = 0; i < f_mTerminals.size(); ++i)
    {
        TerminalBase * oTerminal = f_mTerminals.at(i);
        if (CxContainer::contain(sTerminalNames, oTerminal->terminalName()))
        {
            mTerminals.push_back(oTerminal);
        }
    }
    return mTerminals;
}

std::vector<TerminalBase *> TerminalManager::terminals(const std::vector<int> &sTerminalIds)
{
    std::vector<TerminalBase *> mTerminals;
    for (size_t i = 0; i < f_mTerminals.size(); ++i)
    {
        TerminalBase * oTerminal = f_mTerminals.at(i);
        if (CxContainer::contain(sTerminalIds, oTerminal->terminalId()))
        {
            mTerminals.push_back(oTerminal);
        }
    }
    return mTerminals;
}

void TerminalManager::append(TerminalBase * oTerminal)
{
    f_mTerminals.push_back(oTerminal);
//    oTerminal->setTerminalId(f_mTerminals.size());
}

void TerminalManager::remove(TerminalBase * oTerminal)
{
    CxContainer::remove(f_mTerminals, oTerminal);
}

TerminalBase * TerminalManager::terminal(const std::string &sTerminalName, bool caseSensitive)
{
    if (caseSensitive) {
        std::string sName1 = CxString::toLower( CxString::trim( sTerminalName ) );
        for (size_t i = 0; i < f_mTerminals.size(); ++i) {
            TerminalBase* oTerminal = f_mTerminals.at(i);
            std::string sName2 = CxString::toLower( CxString::trim( oTerminal->terminalName() ) );
            if (sName1 == sName2)
                return oTerminal;
        }
        return NULL;
    } else {
        GM_LISTO_FIND(f_mTerminals, TerminalBase, terminalName(), sTerminalName)
    }
}

TerminalBase *TerminalManager::terminal(int iTerminalId)
{
    for (size_t i = 0; i < f_mTerminals.size(); ++i)
    {
        TerminalBase * oTerminal = f_mTerminals.at(i);
        if (oTerminal->terminalId() == iTerminalId)
        {
            return oTerminal;
        }
    }
    return NULL;
}

TerminalBase * TerminalManager::terminalByProtocol(int iProtocolId)
{
    for (size_t i = 0; i < f_mTerminals.size(); ++i)
    {
        TerminalBase* oTerminal = f_mTerminals.at(i);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol && oProtocol->protocolId() == iProtocolId)
        {
            return oTerminal;
        }
    }
    return NULL;
}

TerminalBase *TerminalManager::terminalByProtocol(const ProtocolBase * oProtocol)
{
    for (size_t i = 0; i < f_mTerminals.size(); ++i)
    {
        TerminalBase * oTerminal = f_mTerminals.at(i);
        if (oTerminal->protocol() == oProtocol)
            return oTerminal;
    }
    return NULL;
}

TerminalBase *TerminalManager::terminalByChannel(int iChannelId)
{
    for (size_t i = 0; i < f_mTerminals.size(); ++i)
    {
        TerminalBase * oTerminal = f_mTerminals.at(i);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol && oProtocol->channel() && oProtocol->channel()->channelId() == iChannelId)
        {
            return oTerminal;
        }
    }
    return NULL;
}

TerminalBase *TerminalManager::terminalByBusiness(int iBusinessId)
{
    for (size_t i = 0; i < f_mTerminals.size(); ++i)
    {
        TerminalBase * oTerminal = f_mTerminals.at(i);
        TBIBase* oBusiness= oTerminal->business();
        if (oBusiness  && oBusiness->businessId() == iBusinessId)
        {
            return oTerminal;
        }
    }
    return NULL;
}

TerminalBase *TerminalManager::terminalByRemoteTerminalId(int iRemoteTerminalId)
{
    for (size_t i = 0; i < f_mTerminals.size(); ++i)
    {
        TerminalBase * oTerminal = f_mTerminals.at(i);
        if (oTerminal->remoteTerminalId() == iRemoteTerminalId)
        {
            return oTerminal;
        }
    }
    return NULL;
}

ProtocolBase *TerminalManager::findProtocol(const std::string &sProtocolTypeSimpleName)
{
    for (size_t i = 0; i < f_mTerminals.size(); ++i)
    {
        TerminalBase* oTerminal = f_mTerminals.at(i);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol && oProtocol->typeSimpleName() == sProtocolTypeSimpleName)
        {
            return oProtocol;
        }
    }
    return NULL;
}

void TerminalManager::start()
{
    if (f_isStarted) return;
    for (size_t i = 0; i < f_mTerminals.size(); ++i) {
        TerminalBase* oTerminal = f_mTerminals.at(i);
        TBIBase* oBusiness = oTerminal->business();
        if (oBusiness){
            oBusiness->start();
        }
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol){
            oProtocol->start();
        }
    }
    f_isStarted = true;
}

void TerminalManager::stop()
{
    if (! f_isStarted) return;
    for (size_t i = 0; i < f_mTerminals.size(); ++i) {
        TerminalBase* oTerminal = f_mTerminals.at(i);
        ProtocolBase* oProtocol = oTerminal->protocol();
        if (oProtocol){
            oProtocol->stop();
        }
        TBIBase* oBusiness = oTerminal->business();
        if (oBusiness){
            oBusiness->stop();
        }
    }
}

bool TerminalManager::isStarted()
{
    return f_isStarted;
}

/**
 * sdk server test commands :

system exit : Application::exit

system save config : Application::saveConfig()


terminal report all\terminalName : report terminal information

msobject report : report msobject information

measure report :

file send [fileName] : send file from directory [app_data/temp/fileName] to every client

yx send : send a test yx to every client

yc send : send a test yc to every client

message send [filePath\message_text] : send message command from directory [app_data/temp/fileName] to every client

channel enable packet.out : endable (CMD)command line packet output

channel disable packet.out : disable (CMD)command line packet output

interinfo enable\disable all\terminalName : enable\disable all\terminal(protocol and business and channel)

timer -message xxx -file xxx -interval:: start\stop timer timer to send message, if [app_data/temp/fileName] exist then send message from file

 */
int TerminalManager::interinfoIn_line(const std::string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

//#include <vdi/monsb_address_worker.h>
//#include <vdi/ycadd_address_worker.h>
int TerminalManager::interinfoIn_cmd(const std::string &sCommand, const std::map<std::string, std::string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    if (sCommand == "system")
    {
        if (CxContainer::contain(sParams, std::string("exit")))
        {
            CxApplication::raiseExit();
        }
        else if (CxContainer::contain(sParams, std::string("save")))
        {
            if (CxContainer::contain(sParams, std::string("save")))
            {
                CxApplication::saveConfig();
            }
        }
        return CxInterinfo::InDeal_Ed;
    }
    else if (sCommand == "terminal")
    {
        if (CxContainer::contain(sParams, std::string("report")))
        {
            vector<string> sTerminalNames = CxString::split(CxContainer::value(sParams, std::string("names")), ',');
            vector<string> sTerminalIds = CxString::split(CxContainer::value(sParams, std::string("ids")), ',');
            std::string sReportValue = CxContainer::value(sParams, std::string("report"));
            vector<string> sReports = reportSelf(sTerminalNames, sTerminalIds);
            if (sReportValue == "cmd")
            {
                cxPrompt().lf() << sReports;
            }
            else
            {
                if (CxFile::save(GclConfig::reportDefaultFilePath(), sReports))
                {
                    cxPrompt() << "report save to file : " << GclConfig::reportDefaultFilePath();
                }
            }
        }
        else if (CxContainer::contain(sParams, std::string("stop")))
        {
            CxApplication::raiseExit();
        }
        return CxInterinfo::InDeal_Ed;
    }
    else if (sCommand == "msobject")
    {
        if (CxContainer::contain(sParams, std::string("report")))
        {
            std::string sReportValue = CxContainer::value(sParams, std::string("report"));
            if (sReportValue.empty() || sReportValue == "all")
            {
                std::string sFilePath = CxFileSystem::mergeFilePath( GclConfig::tempPath() , "msObjects.txt" );
                int iCount = CxMsObjectDataManager::saveMsObjects(sFilePath);
                cxPrompt() << "save MsObjects to " << sFilePath << " , file line count " << iCount << cxEndLine;
            }
        }
        return CxInterinfo::InDeal_Ed;
    }
    else if (sCommand == "interinfo")
    {
        if (CxContainer::contain(sParams, std::string("terminal")))
        {
            std::string sTerminal = CxContainer::value(sParams, std::string("terminal"));
            TerminalBase * oTerminal = TerminalManager::terminal(sTerminal, true);
            if (! oTerminal)
            {
                int iTerminalId = CxString::toInt32(sTerminal);
                if (iTerminalId > 0)
                    oTerminal = TerminalManager::terminal(iTerminalId);
            }
            if (oTerminal)
            {
                TerminalManager::disableInterinfoOut(oTerminal);
            }
        }

        if (CxContainer::contain(sParams, std::string("enable")) || CxContainer::contain(sParams, std::string("diable")))
        {
            CxInterinfoOut::update(sParams);
        }
        //-dir    in / *out
        //-platform    * ui / log / net
        //-type    * system / terminal / protocol / channel
        //-sourceId  terminalId / protocolId
        //-reason
        //-terminalName
        //-targetId
        return CxInterinfo::InDeal_Ed;
    }
    else if (sCommand == "channel")
    {
        if (CxContainer::contain(sParams, std::string("packet")))
        {
            std::string sPacketValue = CxContainer::value(sParams, std::string("packet"));
            if (sPacketValue == "enable")
                CxChannelManager::setPacketInterinfoOut(true);
            else if (sPacketValue == "disable")
                CxChannelManager::setPacketInterinfoOut(false);
        }
        return CxInterinfo::InDeal_Ed;
    }

    return 0;
}

std::vector<string> TerminalManager::reportSelf(const std::vector<string> &sTerminalNames, const std::vector<string> &sTerminalIds)
{
    std::vector<std::string> sReports;
    std::vector<TerminalBase*> mTerminals = f_mTerminals;
    if (sTerminalNames.size()>0)
    {
        mTerminals = terminals(sTerminalNames);
    }
    else if (sTerminalIds.size()>0)
    {
        mTerminals = terminals(CxContainer::valuesTo(sTerminalIds, (int)0));
    }
    for (size_t i = 0; i < mTerminals.size(); ++i)
    {
        sReports.push_back(CxString::format("%06d :", i+1));
        TerminalBase* oTerminal = mTerminals.at(i);
        CxContainer::append(sReports, oTerminal->reportSelf());
        sReports.push_back(".");
    }
    return sReports;
}

std::vector<std::string> TerminalManager::terminalNames()
{
    std::vector<std::string> sTerminalNames;
    for (size_t i = 0; i < f_mTerminals.size(); ++i)
    {
        const TerminalBase* oTerminal = f_mTerminals.at(i);
        sTerminalNames.push_back(oTerminal->terminalName());
    }
    return sTerminalNames;
}

bool TerminalManager::deleteTerminal(TerminalBase *oTerminal)
{
    if (oTerminal){
        remove(oTerminal);
        delete oTerminal;
    }
    return true;
}

void TerminalManager::enableInterinfoOut(TerminalBase * oTerminal, bool bOnly)
{
    if (oTerminal)
    {
        if (bOnly)
        {
            CxInterinfoOut::enableAll();
        }
        CxInterinfoOut::enableType(CxInterinfo::Type_Terminal, oTerminal->terminalId());
        if (oTerminal->protocol())
        {
            CxInterinfoOut::enableType(CxInterinfo::Type_Protocol, oTerminal->protocol()->protocolId());
        }
        if (oTerminal->business())
        {
            CxInterinfoOut::enableType(CxInterinfo::Type_Business, oTerminal->business()->businessId());
        }
        if (oTerminal->protocol() && oTerminal->protocol()->channel())
        {
            CxInterinfoOut::enableType(CxInterinfo::Type_IO_Channel, oTerminal->protocol()->channel()->channelId());
        }
    }
}

void TerminalManager::disableInterinfoOut(TerminalBase *oTerminal)
{
    if (oTerminal)
    {
        CxInterinfoOut::disableType(CxInterinfo::Type_Terminal, oTerminal->terminalId());
        if (oTerminal->protocol())
        {
            CxInterinfoOut::disableType(CxInterinfo::Type_Protocol, oTerminal->protocol()->protocolId());
        }
        if (oTerminal->business())
        {
            CxInterinfoOut::disableType(CxInterinfo::Type_Business, oTerminal->business()->businessId());
        }
        if (oTerminal->protocol() && oTerminal->protocol()->channel())
        {
            CxInterinfoOut::disableType(CxInterinfo::Type_IO_Channel, oTerminal->protocol()->channel()->channelId());
        }
    }
}

