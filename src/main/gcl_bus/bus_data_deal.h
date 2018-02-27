#ifndef BUS_DATA_DEAL_H
#define BUS_DATA_DEAL_H

#include "bus_terminal.h"

//message fileName转发机制
//sourceId message/fileName == target
//message/fileName == target
//是包括还是等于

struct TransferStrategy
{
public:
    int sourceTerminalId;
    std::string name;
    int targetTerminalId;

    TransferStrategy() :
        sourceTerminalId(0),
        name(""),
        targetTerminalId(0)
    {}

    TransferStrategy( int ssid, const std::string & sName, int ttid ) :
        sourceTerminalId(ssid),
        name(sName),
        targetTerminalId(ttid)
    {}

    TransferStrategy &operator=(const TransferStrategy &other)
    {
        sourceTerminalId = other.sourceTerminalId; name=other.name; targetTerminalId=other.targetTerminalId; return *this;
    }

    TransferStrategy(const TransferStrategy &other) :
        sourceTerminalId(other.sourceTerminalId), name(other.name), targetTerminalId(other.targetTerminalId)
    {}

};

inline bool operator==(const TransferStrategy &p1, const TransferStrategy &p2)
{ return p1.sourceTerminalId == p2.sourceTerminalId && p1.name == p2.name && p1.targetTerminalId == p2.targetTerminalId; }

inline bool operator!=(const TransferStrategy &p1, const TransferStrategy &p2)
{ return !(p1 == p2); }


class MessageTransferStrategy
{
public:
    static const std::vector<int> & transferToTerminalIds(int iSourceTerminalId, const std::string & sMessage);

    static const std::string & getTableName() {
        return CS_TableName_Bus;
    }

    //<!-- Bus Transfer Message Strategy : ssid == Source Terminal Id(if less of 0, whill all message); -->
    //xml :message=ticket stid=0x01830101 ttid=0x01830101
    static void update(const std::map<std::string, std::string> & row);

    static void update( int ssid, const std::string & sName, int ttid );

    static std::vector<std::string> reportSelf();

};

class FileNameTransferStrategy
{
public:
    static const std::vector<int> & transferToTerminalIds(int iSourceTerminalId, const std::string & sFileName);

    static const std::string & getTableName() {
        return CS_TableName_Bus;
    }

    //<!-- Bus Transfer FileName Strategy : ssid == Source Terminal Id(if less of 0, whill all message); -->
    //xml :filename=ticket stid=0x01830101 ttid=0x01830101
    static void update(const std::map<std::string, std::string> & row);

    static void update( int ssid, const std::string & sName, int ttid );

    static std::vector<std::string> reportSelf();
};

class RealdataTransferStrategy
{
public:
    static int transferToTerminalId(int iSourceTerminalId);

    static const std::string & getTableName() {
        return CS_TableName_Bus;
    }

    //xml : stid=0x01830101 ttid=0x01830101
    static void update(const std::map<std::string, std::string> & row);

    static void update(int ssid, int ttid );

    static std::vector<std::string> reportSelf();

};


class BusDataDeal
{
public:
    static void dealMessageCommand(int iSourceTerminalId, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach *oAttach);

    static void dealFile(int iSourceTerminalId, const CxFileSystem::PathInfo &pathInfo, const std::vector<std::string> &fileData, const PsmAttach *oAttach);

    static void dealRealtimeDataRequest(int iSourceTerminalId, const PsmAttach *oAttach);

    static void dealRealtimeDataPost(int iSourceTerminalId, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach);

    //定时 1 秒
    static void timerDeal(int iInterval);

};

#endif // BUS_DATA_DEAL_H
