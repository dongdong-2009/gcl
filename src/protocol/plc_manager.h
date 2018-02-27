#ifndef PLC_MANAGER_H
#define PLC_MANAGER_H


#include <vdi/gccommon.h>
#include <vdi/terminal_manager.h>

#include "plc_fins_global.h"
#include "plc_protocol.h"

#include "rfid_manager.h"

class PlcManager
{
public:
    static inline PlcProtocol * getPlcProtocol() {
        static PlcProtocol * s_oPlcProtocol=NULL;
        if (! s_oPlcProtocol)
            s_oPlcProtocol = (PlcProtocol *)TerminalManager::findProtocol(CS_ProtocolTypeSimpleName_Plc);
        return s_oPlcProtocol;
    }

    //设置遥信值
    static bool setYxCommand(const std::string &sCommand, int iValue, bool vfy=false);

    static bool setYxCommand(GCCommon::GCCommonObj &obj, bool vfy=true);

    static bool setYxCommand(GCCommon::GCComObjVector &vObj, bool vfy=true);

    //设置遥测值
    static bool setYcCommand(const std::string &sCommand, int iValue, bool vfy=false);

    static bool setYcCommand(GCCommon::GCCommonObj &obj, bool vfy=true);

    static bool setYcCommand(GCCommon::GCComObjVector &vObj, bool vfy=true);

    //获取遥信值
    static bool getYxCommand(const std::string &sCommand, int iValue, bool bReq = true);

    static bool getYxCommand(GCCommon::GCCommonObj &obj, bool bReq = true);

    static bool getYxCommand(GCCommon::GCComObjVector &vObj, int &index, bool bReq = true);
    //获取遥测值
    static bool getYcCommand(const std::string &sCommand, int iValue, bool bReq = true);

    static bool getYcCommand(GCCommon::GCCommonObj &obj, bool bReq = true);

    static bool getYcCommand(GCCommon::GCComObjVector &vObj, int &index, bool bReq = true);

    //超时等待遥信值
    static bool waitYxEvent(const std::string &sCommand, int iValue, uint32 iTimeOut, bool bCanStop = false);

    static bool waitYxEvent(GCCommon::GCCommonObj &obj, uint32 iTimeOut, bool bCanStop);

    static bool waitYxEvent(GCCommon::GCComObjVector &vObj, uint32 iTimeOut, int * pIndex = NULL, bool bCanStop = false);

    //超时等待遥测值
    static bool waitYcEvent(const std::string &sCommand, int iValue, uint32 iTimeOut);

    static bool waitYcEvent(GCCommon::GCCommonObj &obj, uint32 iTimeOut);

    static bool waitYcEvent(GCCommon::GCComObjVector &vObj, uint32 iTimeOut);

    static bool waitYcEvent(GCCommon::GCComObjVector &vObj, uint32 iTimeOut, int &index);

    static bool getCancelWait();

    static void setCancelWait(bool value);

    static bool callUp();

    static bool isOnline();

};

#endif // PLC_MANAGER_H
