#ifndef PSM_EXPLAINER_H
#define PSM_EXPLAINER_H


#include <psm_struct.h>
#include <psm_common.h>
#include <ccxx/cxchannel.h>
#include <ccxx/cxfile.h>
#include <ccxx/cxpacket.h>


class PsmExplainer;

typedef CxPacket5<int, short, short, psm_packet_body_t, short>::PacketData PsmPacketData;
typedef CxPacket5<int, short, short, psm_packet_body_t, short>::ReceivePacket PsmReceivePacket;
typedef CxPacket5<int, short, short, psm_packet_body_t, short>::SendPacket PsmSendPacket;


//*命令 - 用消息形式收发
typedef int ( * fn_psm_explain_received_message_command_t )( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );

typedef int ( * fn_psm_explain_received_file_write_t )( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach );

typedef int ( * fn_psm_explain_received_realtime_data_request_t )( const PsmExplainer * oPsm, const PsmAttach * oAttach );

typedef int ( * fn_psm_explain_received_realtime_data_post_t )( const PsmExplainer * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach );

typedef int ( * fn_psm_explain_result_file_write_t )( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const PsmAttach * oAttach );


class IPsmExplainNotify
{
protected:
    virtual int received_packet(const PsmExplainer * oPsm, int oTag = 0) = 0;

    virtual void received_message_command(const PsmExplainer * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL) = 0;

    virtual void received_file_write(const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL) = 0;

    virtual void received_realtime_data_request(const PsmExplainer * oPsm, const PsmAttach * oAttach = NULL) = 0;

    virtual void received_realtime_data_post(const PsmExplainer * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL) = 0;

    virtual void result_file_write(const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const PsmAttach * oAttach = NULL) = 0;

    friend class PsmExplainer;

};


struct PsmData
{
    PsmExplainer * psm;
    int tag;
    PsmAttach attach;

    PsmData() :
        psm(NULL),
        tag(0)
    {
    }

    PsmData(PsmExplainer * oPsm,
               int iTag,
               const PsmAttach & mAttach) :
        psm(oPsm), tag(iTag), attach(mAttach)
    {}
};

struct PsmMessage
{
    PsmExplainer * psm;
    std::string command;
    std::string param;
    PsmAttach attach;

    PsmMessage() :
        psm(NULL)
    {
    }

    PsmMessage(PsmExplainer * oPsm,
               const std::string & sCommand,
               const std::string & sParam,
               const PsmAttach & mAttach) :
        psm(oPsm), command(sCommand), param(sParam), attach(mAttach)
    {}
};

struct PsmFile
{
    PsmExplainer * psm;
    CxFilePathInfo filePathInfo;
    PsmAttach attach;

    PsmFile() :
        psm(NULL)
    {
    }

    PsmFile(PsmExplainer * oPsm,
               const CxFilePathInfo & mFilePathInfo,
               const PsmAttach & mAttach) :
        psm(oPsm), filePathInfo(mFilePathInfo), attach(mAttach)
    {}
};

struct PsmRealtimeData
{
    PsmExplainer * psm;
    int paramType;
    std::string data;
    int paramCount;
    PsmAttach attach;

    PsmRealtimeData() :
        psm(NULL)
    {
    }

    PsmRealtimeData(PsmExplainer * oPsm,
               int iParamType,
               const std::string & sData,
               int iParamCount,
               const PsmAttach & mAttach) :
        psm(oPsm), paramType(iParamType), data(sData), paramCount(iParamCount), attach(mAttach)
    {}
};


class PsmExplainer : public ICxTimerNotify
{
public:

    enum DealReplyTypeEnum { DealReplyType_None = 0, DealReplyType_Ack, DealReplyType_Nack, DealReplyType_Define };

public:
    PsmExplainer();

    ~PsmExplainer();

    inline int getSentSourceId() const { return _sentSourceId; }
    inline void setSentSourceId(int value) { _sentSourceId = value; }

    inline int getSentTargetId() const { return _sentTargetId; }
    inline void setSentTargetId(int value) { _sentTargetId = value; }

    int receivedData(const uchar* pData, int iLength);

    //注册Psm的解释通知
    inline void registerPsmExplainNotify(IPsmExplainNotify * oNotify) { _psmExplainNotify = oNotify; }

    inline void registerPsmExplainWrite(CxIChannelSend * oChannelSend) { _psmExplainWrite = oChannelSend; }

    int postMessageCommand( const char * sCommand, const char * sParam = 0, int iParamLength = 0, const PsmAttach * oAttach = NULL);

    int postFile(const std::string & sFilePath, const PsmAttach * oAttach = NULL);

    int postFile(const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> &fileData, const PsmAttach * oAttach = NULL);

    int postRealtimeDataRequest( const PsmAttach * oAttach = NULL );

    int postRealtimeDataPost( int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

    int postHeartJump( const PsmAttach * oAttach = NULL );

    int postData( int iCommand, int iParamType = 0, const char * pData = 0, int iDataLength = 0, int iParamCount = 1, const PsmAttach * oAttach = NULL );

    int responseAck( int iCommand, const PsmAttach * oAttach = NULL);

    int responseNack( int iCommand , int iErrorid, const PsmAttach * oAttach = NULL );

    int postPacketData(const psm_packet_body_t & body, const char * pData, int iDataLength);

    void dealPacketData( const PsmPacketData & packet );

    void start();

    void stop();

    inline const PsmReceivePacket & receivePacket() const { return _receivePacket; }

    inline msepoch_t lastReceivedDataTime() const { return _lastReceivedDataTime; }

    //attention : multiRoad and processQueue can not all ture
    inline bool isProcessQueue() { return _isProcessQueue; }
    inline void setIsProcessQueue(bool value) { _isProcessQueue = value; }

    std::string reportSelf();

protected:
    void timer_timeOut(const CxTimer* oTimer);

    void checkProtocol();

    virtual int sendData(const char* pData, int iLength, int iTargetId);

public:
    std::string pathInfoToBuffer(const CxFileSystem::PathInfo &pathInfo);

    CxFileSystem::PathInfo pathInfoFromBuffer(const char * pData, size_t iLength);    

private:
    IPsmExplainNotify * _psmExplainNotify;
    CxIChannelSend * _psmExplainWrite;

    PsmReceivePacket _receivePacket;
    PsmSendPacket _sendPacket;

    CxFileSystem::PathInfo _sendPathInfo;
    PsmAttach _sendPathInfoAttach;
    std::vector<std::string> _sendFileTexts;
    msepoch_t _sendFileTime;
    int _sendFileCurrentIndex;
    CxFileSystem::PathInfo _receivePathInfo;
    std::vector<std::string> _receiveFileTexts;
    int _receivedFileSize;

    CxTimer* _checkProtocolTm;

    std::map<std::string, PsmAttach> _sendFilePathes;
    std::queue<CxFileSystem::PathInfo> _sendFileSreamInfos;
    std::queue<std::vector<std::string> > _sendFileSreamDatas;
    std::queue<PsmAttach> _sendFileSreamAttach;
    msepoch_t _sendingFileTime;

    char _sendBuffer[CI_PSM_PACKET_DATA];

    msepoch_t _lastReceivedDataTime;
    int _sentSourceId;
    int _sentTargetId;

    int _receiveFileInfoTargetId;

    bool _isProcessQueue;
    //*lock
    std::queue<PsmMessage> _processMessages;
    std::queue<PsmFile> _processFiles;
    std::queue<PsmData> _processRealtimeDataRequests;
    std::queue<PsmRealtimeData> _processRealtimeDatas;
    std::queue<PsmFile> _processResultFiles;
    std::string _fileSavePath;
    //#lock
    CxMutex _processLock;

private:
    void dealData();

    int sendFileInfo(const std::string &sFilePath, const PsmAttach * oAttach = NULL);

    int sendFileInfo(const CxFileSystem::PathInfo & mPathInfo, const PsmAttach * oAttach = NULL);

    int sendFileData(const std::string & sFileData, const PsmAttach * oAttach = NULL);

    int sendNextFilePath();

    void sendFileComplete();

    int dealMessageCommand(const PsmPacketData &packet);

    int dealFileInformationWrite(const PsmPacketData &packet);

    int dealFileDataWrite(const PsmPacketData &packet);

    int dealNack(const PsmPacketData &packet);

    int dealRealtimeRequest(const PsmPacketData &packet);

    int dealRealtimePost(const PsmPacketData &packet);

    void receivedMessageCommand(const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL);

    void receivedFileWrite(const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL);

    void receivedRealtimeDataRequest(const PsmAttach * oAttach = NULL);

    void receivedRealtimeDataPost(int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

    //processer thread notify main thread
    static void threadEventNotify(PsmExplainer * oPsm, int iMsg, int iTag = 0, const void * pData = 0, int iLength = 0, void * oSource = 0);

    //main thread CallUp
    static void processThreadNofity(int, int, const void *, int, void *, void *);

    //main thread CallUp
    void processThreadNofityImpl(int iMsg, int iTag, const void * pData, int iLength, void * oSource);

};



#endif // PSM_EXPLAINER_H
