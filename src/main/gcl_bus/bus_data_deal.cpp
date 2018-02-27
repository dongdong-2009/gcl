#include "bus_data_deal.h"

#include <psm_struct.h>

#include <vdi/measure_sharememory_all.h>
#include "bus_measure_control.h"
#include "bus_general_access.h"


using namespace std;

static map<int, map<string, vector<int> > > f_sourceMessagesTot;
static map<string, vector<int> > f_messagesTot;

static map<int, map<string, vector<int> > > f_sourceFileNamesTot;
static map<string, vector<int> > f_fileNamesTot;

static map<int, int> f_sourceRealtimeDataTot;
static vector<int> f_realtimeDataTot;


const vector<int> & MessageTransferStrategy::transferToTerminalIds(int iSourceTerminalId, const string &sMessage)
{
    if (iSourceTerminalId>0)
    {
        map<int, map<string, vector<int> > >::iterator it1 = f_sourceMessagesTot.find(iSourceTerminalId);
        if (it1 != f_sourceMessagesTot.end())
        {
            const map<string, vector<int> > & messagesTot = it1->second;
            for (map<string, vector<int> >::const_iterator it2 = messagesTot.begin(); it2 != messagesTot.end(); ++it2)
            {
                const string & msg = it2->first;
                if (sMessage.find(msg) != string::npos)
                {
                    return it2->second;
                }
            }
        }
        for (map<string, vector<int> >::const_iterator it = f_messagesTot.begin(); it != f_messagesTot.end(); ++it)
        {
            const string & msg = it->first;
            if (sMessage.find(msg) != string::npos)
            {
                return it->second;
            }
        }
    }
    static vector<int> empty;
    return empty;
}

void MessageTransferStrategy::update(const std::map<string, string> &row)
{
    int sourceTerminalId = 0;
    std::string sName;
    int targetTerminalId = 0;

    sName = CxContainer::valueTo(row, CS_Fetch_Message, sName);
    sourceTerminalId = CxContainer::valueTo(row, CS_SourceTerminalId, sourceTerminalId);
    targetTerminalId = CxContainer::valueTo(row, CS_TargetTerminalId, targetTerminalId);

    if (sName.size()>0)
    {
        update( sourceTerminalId, sName, targetTerminalId );
    }
}

void MessageTransferStrategy::update(int ssid, const string &sName, int ttid)
{
    GM_INVALID_RETURE(sName.size()>0);
    GM_INVALID_RETURE(ttid>0);
    if (ssid>0)
    {
        map<int, map<string, vector<int> > >::iterator it1 = f_sourceMessagesTot.find(ssid);
        if (it1 != f_sourceMessagesTot.end())
        {
            map<string, vector<int> > & messagesTot = it1->second;
            map<string, vector<int> >::iterator it2 = messagesTot.find(sName);
            if (it2 != messagesTot.end())
            {
                vector<int> & iTargetTerminalIds = it2->second;
                if (!CxContainer::contain(iTargetTerminalIds, ttid))
                {
                    iTargetTerminalIds.push_back(ttid);
                }
            }
            else
            {
                vector<int> iTargetTerminalIds;
                iTargetTerminalIds.push_back(ttid);
                messagesTot[sName] = iTargetTerminalIds;
            }
        }
        else
        {
            map<string, vector<int> > messagesTot;
            vector<int> iTargetTerminalIds;
            iTargetTerminalIds.push_back(ttid);
            messagesTot[sName] = iTargetTerminalIds;
            f_sourceMessagesTot[ssid] = messagesTot;
        }
    }
    else
    {
        map<string, vector<int> >::iterator it = f_messagesTot.find(sName);
        if (it != f_messagesTot.end())
        {
            vector<int> & iTargetTerminalIds = it->second;
            if (!CxContainer::contain(iTargetTerminalIds, ttid))
            {
                iTargetTerminalIds.push_back(ttid);
            }
        }
        else
        {
            vector<int> iTargetTerminalIds;
            iTargetTerminalIds.push_back(ttid);
            f_messagesTot[sName] = iTargetTerminalIds;
        }
    }
}

std::vector<string> MessageTransferStrategy::reportSelf()
{
    vector<string> sReports;
    sReports.push_back("*** MessageTransferStrategy Report ***");
    sReports.push_back("Has SourceId Transfer Strategy : ");
    for (map<int, map<string, vector<int> > >::const_iterator it = f_sourceMessagesTot.begin(); it != f_sourceMessagesTot.end(); ++it)
    {
        const map<string, vector<int> > & messageTots = it->second;
        string sMessageTot = CxContainer::joinToString(messageTots, "=", " ", " ; ");
        string sReport = CxString::format("SourceId[%d] Transfer Strategy Count[%d] : %s", it->first, messageTots.size(), sMessageTot.c_str());
        sReports.push_back(sReport);
    }
    sReports.push_back("#");
    sReports.push_back("Not SourceId Transfer Strategy : ");
    string sMessageTot = CxContainer::joinToString(f_messagesTot, "=", " ", " ; ");
    sReports.push_back(CxString::format("Transfer Strategy Count[%d] : %s", f_messagesTot.size(), sMessageTot.c_str()));
    return sReports;
}

const std::vector<int> &  FileNameTransferStrategy::transferToTerminalIds(int iSourceTerminalId, const string &sFileName)
{
    if (iSourceTerminalId>0)
    {
        map<int, map<string, vector<int> > >::iterator it1 = f_sourceFileNamesTot.find(iSourceTerminalId);
        if (it1 != f_sourceFileNamesTot.end())
        {
            const map<string, vector<int> > & fileNamesTot = it1->second;
            for (map<string, vector<int> >::const_iterator it2 = fileNamesTot.begin(); it2 != fileNamesTot.end(); ++it2)
            {
                const string & fns = it2->first;
                if (sFileName.find(fns) != string::npos)
                {
                    return it2->second;
                }
            }
        }
        for (map<string, vector<int> >::const_iterator it = f_fileNamesTot.begin(); it != f_fileNamesTot.end(); ++it)
        {
            const string & fns = it->first;
            if (sFileName.find(fns) != string::npos)
            {
                return it->second;
            }
        }
    }
    static vector<int> empty;
    return empty;
}

void FileNameTransferStrategy::update(const std::map<string, string> &row)
{
    int sourceTerminalId = 0;
    std::string sName;
    int targetTerminalId = 0;

    sName = CxContainer::valueTo(row, CS_Fetch_Filename, sName);
    sourceTerminalId = CxContainer::valueTo(row, CS_SourceTerminalId, sourceTerminalId);
    targetTerminalId = CxContainer::valueTo(row, CS_TargetTerminalId, targetTerminalId);

    if (sName.size() > 0)
    {
        update( sourceTerminalId, sName, targetTerminalId);
    }
}

void FileNameTransferStrategy::update(int ssid, const string &sName, int ttid)
{
    GM_INVALID_RETURE(sName.size()>0);
    GM_INVALID_RETURE(ttid>0);
    if (ssid>0)
    {
        map<int, map<string, vector<int> > >::iterator it1 = f_sourceFileNamesTot.find(ssid);
        if (it1 != f_sourceFileNamesTot.end())
        {
            map<string, vector<int> > & fileNamesTot = it1->second;
            map<string, vector<int> >::iterator it2 = fileNamesTot.find(sName);
            if (it2 != fileNamesTot.end())
            {
                vector<int> & iTargetTerminalIds = it2->second;
                if (!CxContainer::contain(iTargetTerminalIds, ttid))
                {
                    iTargetTerminalIds.push_back(ttid);
                }
            }
            else
            {
                vector<int> iTargetTerminalIds;
                iTargetTerminalIds.push_back(ttid);
                fileNamesTot[sName] = iTargetTerminalIds;
            }
        }
        else
        {
            map<string, vector<int> > fileNamesTot;
            vector<int> iTargetTerminalIds;
            iTargetTerminalIds.push_back(ttid);
            fileNamesTot[sName] = iTargetTerminalIds;
            f_sourceFileNamesTot[ssid] = fileNamesTot;
        }
    }
    else
    {
        map<string, vector<int> >::iterator it = f_fileNamesTot.find(sName);
        if (it != f_fileNamesTot.end())
        {
            vector<int> & iTargetTerminalIds = it->second;
            if (!CxContainer::contain(iTargetTerminalIds, ttid))
            {
                iTargetTerminalIds.push_back(ttid);
            }
        }
        else
        {
            vector<int> iTargetTerminalIds;
            iTargetTerminalIds.push_back(ttid);
            f_fileNamesTot[sName] = iTargetTerminalIds;
        }
    }
}

std::vector<string> FileNameTransferStrategy::reportSelf()
{
    vector<string> sReports;
    sReports.push_back("*** FileNameTransferStrategy Report ***");
    sReports.push_back("Has SourceId Transfer Strategy : ");
    for (map<int, map<string, vector<int> > >::const_iterator it = f_sourceFileNamesTot.begin(); it != f_sourceFileNamesTot.end(); ++it)
    {
        const map<string, vector<int> > & fileNameTots = it->second;
        string sFileNameTot = CxContainer::joinToString(fileNameTots, "=", " ", " ; ");
        string sReport = CxString::format("SourceId[%d] Transfer Strategy Count[%d] : %s", it->first, fileNameTots.size(), sFileNameTot.c_str());
        sReports.push_back(sReport);
    }
    sReports.push_back("#");
    sReports.push_back("Not SourceId Transfer Strategy : ");
    string sFileNameTot = CxContainer::joinToString(f_fileNamesTot, "=", " ", " ; ");
    sReports.push_back(CxString::format("Transfer Strategy Count[%d] : %s", f_fileNamesTot.size(), sFileNameTot.c_str()));
    return sReports;
}
















//实时数据转发
int RealdataTransferStrategy::transferToTerminalId(int iSourceTerminalId)
{
    if (iSourceTerminalId>0)
    {
        for (map<int, int>::const_iterator it = f_sourceRealtimeDataTot.begin(); it != f_sourceRealtimeDataTot.end(); ++it)
        {
            if (it->second == iSourceTerminalId)
                return it->first;
        }
    }
    return 0;
}

void RealdataTransferStrategy::update(const std::map<string, string> &row)
{
    int sourceTerminalId = 0;
     std::string sType;
    int targetTerminalId = 0;

    sType = CxContainer::valueTo(row, CS_Fetch_CmdType, sType);
    if (sType != CS_Fetch_RealtimeData) return;

    sourceTerminalId = CxContainer::valueTo(row, CS_SourceTerminalId, sourceTerminalId);
    targetTerminalId = CxContainer::valueTo(row, CS_TargetTerminalId, targetTerminalId);

    update( sourceTerminalId,targetTerminalId );
}

void RealdataTransferStrategy::update(int ssid, int ttid)
{
    GM_INVALID_RETURE(ttid>0);
    if (ssid>0)
    {
        f_sourceRealtimeDataTot[ssid] = ttid;
    }
    else
    {
        f_realtimeDataTot.push_back(ttid);
    }
}

std::vector<string> RealdataTransferStrategy::reportSelf()
{
    vector<string> sReports;
    sReports.push_back("*** RealdataTransferStrategy Report ***");
    string sTot = CxContainer::joinToString(f_sourceRealtimeDataTot, "=", "; ");
    sReports.push_back(CxString::format("Transfer Strategy Count[%d] : %s", f_sourceRealtimeDataTot.size(), sTot.c_str()));
    return sReports;
}














void BusDataDeal::dealMessageCommand(int iSourceTerminalId, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach *oAttach)
{
    string sCommand = pCommand;sCommand.push_back(0);

//    int iTerminalId = oBusTerminal->terminalId();
    const vector<int> & iTargetTerminalIds = MessageTransferStrategy::transferToTerminalIds(iSourceTerminalId, sCommand);

    if (iTargetTerminalIds.size()>0)
    {
        for (size_t i = 0; i < iTargetTerminalIds.size(); ++i)
        {
            int iTargetTerminalId = iTargetTerminalIds.at(i);
            if (iSourceTerminalId == iTargetTerminalId)
            {
                cxDebug() << "bus recv message [ " << sCommand << " ] from " << CxString::format("0x%08x",iSourceTerminalId) << ", but transferToTerminalId same to source !!!" << cxEndLine;
                continue;
            }
            else if (iTargetTerminalId > 0)
            {
                BusManager::transferMessage(iSourceTerminalId, pCommand, pParam, iParamLength, iTargetTerminalId);
//                cxDebug() << "bus recv message [ " << sCommand << " ] from " << CxString::format("0x%08x",iSourceTerminalId) <<" to "<<CxString::format("0x%08x",iTargetTerminalId) << cxEndLine;
            }
            else
            {
                cxDebug() << "bus recv message [ " << sCommand << " ] from " << CxString::format("0x%08x",iSourceTerminalId) << ", but transferToTerminalId less 0 !!!" << cxEndLine;
                continue;
            }
        }
    }
    else
    {
        if (sCommand.find("send.yk") != string::npos || sCommand.find("send.yt") != string::npos || sCommand.find("send.ys") != string::npos )
        {
            BusMeasureControl::dealPsmMeasureControl(sCommand, pParam, iParamLength, oAttach);
        }
        else
        {
            cxDebug() << "bus recv message [ " << sCommand << " ] from " << CxString::format("0x%08x",iSourceTerminalId) << " but do not target!!!" << cxEndLine;
        }
    }
}

void BusDataDeal::dealFile(int iSourceTerminalId, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    if (oAttach->targetId > 0)
    {
        if (iSourceTerminalId == oAttach->targetId)
        {
            cxDebug() << CxString::format("bus recv file [%s %d]", pathInfo.fileName.c_str(), pathInfo.fileSize)  << " from " << CxString::format("0x%08x",oAttach->sourceId) << ", but transferToTerminalId same to source !!!";
            return;
        }

        BusManager::transferFileStream(oAttach->sourceId, pathInfo, fileData, oAttach->targetId);

        cxDebug() << CxString::format("bus recv file [%s %d]", pathInfo.fileName.c_str(), pathInfo.fileSize)  << " from " << CxString::format("0x%08x",oAttach->sourceId) <<" to "<<CxString::format("0x%08x",oAttach->targetId) << cxEndLine;
    }
    else
    {
        //*004
        string sFileName = pathInfo.fileName;
        const vector<int> & iTargetTerminalIds = FileNameTransferStrategy::transferToTerminalIds(iSourceTerminalId, sFileName);
        if (iTargetTerminalIds.size()>0)
        {
            for (size_t i = 0; i < iTargetTerminalIds.size(); ++i)
            {
                int iTargetTerminalId = iTargetTerminalIds.at(i);
                if (iSourceTerminalId == iTargetTerminalId)
                {
                    cxDebug() << CxString::format("bus recv file [%s %d]", pathInfo.fileName.c_str(), pathInfo.fileSize)  << " from " << CxString::format("0x%08x",oAttach->sourceId) << ", but transferToTerminalId same to source !!!";
                    continue;
                }
                else if (iTargetTerminalId > 0)
                {
                    if (iTargetTerminalId == BusGeneralAccess::getSourceId())
                        BusGeneralAccess::postFile(pathInfo, fileData);
                    else
                        BusManager::transferFileStream(iSourceTerminalId, pathInfo, fileData, iTargetTerminalId);
                    cxDebug() << CxString::format("bus recv file [%s %d]", sFileName.c_str(),pathInfo.fileSize)  << " from " << CxString::format("0x%08x",iSourceTerminalId) <<" to "<<CxString::format("0x%08x",iTargetTerminalId) << cxEndLine;
                }
                else
                {
                    cxDebug() << CxString::format("bus recv file [%s %d]", sFileName.c_str(),pathInfo.fileSize)  << " from "  << CxString::format("0x%08x",iSourceTerminalId) << " but transferToTerminalId less 0 !!!" << cxEndLine;
                }
            }
        }
    }
}

void BusDataDeal::dealRealtimeDataRequest(int iSourceTerminalId, const PsmAttach *oAttach)
{
    cxDebug() << "bus recv realtime data request but do not surport!!!";
}

void BusDataDeal::dealRealtimeDataPost(int iSourceTerminalId, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
//    int iSourceTerminalId = oBusTerminal->terminalId();
    int iTargetTerminalId = RealdataTransferStrategy::transferToTerminalId(iSourceTerminalId);
    if (iSourceTerminalId == iTargetTerminalId)
    {
        if (iTargetTerminalId == BusGeneralAccess::getSourceId())
            MeasureShareMemoryAll::dealRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, oAttach);
//            cxDebug() << "bus recv rtdata [ " << iDataLength << iParamCount << " ] from " << CxString::format("0x%08x",iSourceTerminalId) << ", write to sharememory." << cxEndLine;

//        cxDebug() << "bus recv rtdata [ " << iDataLength << iParamCount << " ] from " << CxString::format("0x%08x",iSourceTerminalId) << ", but transferToTerminalId same to source !!!";
        return;
    }

    if (iTargetTerminalId > 0)
    {
        if (iTargetTerminalId == BusGeneralAccess::getSourceId())
            BusGeneralAccess::postRealtimeDataPost(iParamType, pData, iDataLength,iParamCount);
        else
            BusManager::transferRealtimeDataPost(iSourceTerminalId, iParamType, pData, iDataLength, iParamCount, iTargetTerminalId);
//        cxDebug() << "bus recv rtdata [ " << iDataLength << iParamCount << " ] from " << CxString::format("0x%08x",iSourceTerminalId) <<" to "<<CxString::format("0x%08x",iTargetTerminalId) << cxEndLine;
    }
    else
    {
        if (f_realtimeDataTot.size()>0)
        {
            for(size_t i=0; i < f_realtimeDataTot.size(); i++)
            {
                iTargetTerminalId = f_realtimeDataTot[i];
                if (iTargetTerminalId == BusGeneralAccess::getSourceId())
                    BusGeneralAccess::postRealtimeDataPost(iParamType, pData, iDataLength,iParamCount);
                else
                    BusManager::transferRealtimeDataPost(iSourceTerminalId, iParamType, pData, iDataLength, iParamCount, iTargetTerminalId);
//                cxDebug() << "bus recv rtdata [ " << iDataLength << iParamCount << " ] from " << CxString::format("0x%08x",iSourceTerminalId) <<" to "<<CxString::format("0x%08x",iTargetTerminalId) << cxEndLine;
            }
        }
        else
        {
            MeasureShareMemoryAll::dealRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, oAttach);
//            cxDebug() << "bus recv rtdata [ " << iDataLength << iParamCount << " ] from " << CxString::format("0x%08x",iSourceTerminalId) << ", write to sharememory." << cxEndLine;
        }
    }
}

void BusDataDeal::timerDeal(int iInterval)
{
    static int iTimes = 0;
    ++iTimes;

    if (iTimes % 10) return;

    static msepoch_t dtEncryptInvalid = 0;

    if (dtEncryptInvalid != 0 && CxTime::milliSecondDifferToNow(dtEncryptInvalid) > 1000 * 60 * 60 * 2)
    {
        cxWarning() << "EncryptDog EncryptDog EncryptDog Invalid !!! BUS Long Sleep or enter 'q' to exit !!! ";
        cout        << "EncryptDog EncryptDog EncryptDog Invalid !!! BUS Long Sleep or enter 'q' to exit !!! " << endl;
        while (1)
        {
            string s;
            std::cin >> s;
            if (s == "q")
            {
                break;
            }
            else
            {
                std::cout << s << endl;
            }
        }
        CxApplication::raiseExit();
    }

    StrawShareMemory::MeasureData * oMeasureData = StrawShareMemory::memoryManager()->measureData(0x03000008);
    if (oMeasureData)
    {
        size_t iSize = strlen(oMeasureData->value.value);
        if (iSize == 1 && dtEncryptInvalid == 0)
        {
            dtEncryptInvalid = CxTime::currentMsepoch();
            cxPrompt() << "EncryptDog EncryptDog EncryptDog invalid Now!!!";
        }
        else if (dtEncryptInvalid != 0)
        {
            dtEncryptInvalid = 0;
            cxPrompt() << "EncryptDog EncryptDog EncryptDog Valid!!!";
        }
    }
}
