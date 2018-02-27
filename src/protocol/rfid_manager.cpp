#include "rfid_manager.h"

#include <vdi/terminal_manager.h>
#include "rfid_protocol.h"
#include "plc_manager.h"

msepoch_t RfidManager::query()
{
    RfidProtocol * oRfidProtocol = (RfidProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Rfid);
    if (oRfidProtocol)
    {
        return oRfidProtocol->query();
    }
    return 0;
}

msepoch_t RfidManager::lastTimeUpdateRfid()
{
    RfidProtocol * oRfidProtocol = (RfidProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Rfid);
    if (oRfidProtocol)
    {
        return oRfidProtocol->lastTimeUpdateRfid();
    }
    return 0;
}
bool RfidManager::isSucced()
{
    RfidProtocol * oRfidProtocol = (RfidProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Rfid);
    if (oRfidProtocol)
    {
        return oRfidProtocol->isSucced();
    }
    return false;
}

std::string RfidManager::lastRfid()
{
    RfidProtocol * oRfidProtocol = (RfidProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Rfid);
    if (oRfidProtocol)
    {
        return oRfidProtocol->lastRfid();
    }
    return "";
}

std::string RfidManager::lastBarcode()
{
    RfidProtocol * oRfidProtocol = (RfidProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Rfid);
    if (oRfidProtocol)
    {
        return oRfidProtocol->lastBarcode();
    }
    return "";
}

std::string RfidManager::lastResponse()
{
    RfidProtocol * oRfidProtocol = (RfidProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Rfid);
    if (oRfidProtocol)
    {
        return oRfidProtocol->lastResponse();
    }
    return "";
}

bool RfidManager::waitRfid(std::string &sRfid, std::string &barcode, msepoch_t iTimeOut)
{
    msepoch_t dtNow = query();
    bool bRet = false;
    if (dtNow > 0)
    {
        msepoch_t iG = CxTime::currentSystemTime() - dtNow;
        while (iG < iTimeOut)
        {
            CxApplication::processEvents();
            iG = CxTime::currentSystemTime() - dtNow;
            if(isSucced())
            {
                sRfid = lastRfid();
                barcode = lastBarcode();
                bRet = true;
                break;
            }
        }
    }
    return bRet;
}

bool RfidManager::waitRfidEx(std::string &sRfid, std::string &barcode, msepoch_t iTimeOut)
{
//    if(waitRfid(sRfid, barcode, iTimeOut) == false) ????
//    {
//        return waitRfid(sRfid, barcode, iTimeOut);
//    }

//    return false;

    return waitRfid(sRfid, barcode, iTimeOut);
}
