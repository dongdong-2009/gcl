#ifndef PSM_PROTOCOL_H
#define PSM_PROTOCOL_H


#include <vdi/protocol_global.h>
#include <vdi/protocol_common.h>
#include <vdi/send_message_cases.h>
#include <vdi/send_file_cases.h>

#include <psm/psm_explain.h>


class PsmProtocol;


class IPsmProtocolNotify
{
protected:
    virtual void received_message_command(const PsmProtocol * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL) = 0;

    virtual void received_file_write(const PsmProtocol * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL) = 0;

    virtual void received_realtime_data_request(const PsmProtocol * oPsm, const PsmAttach * oAttach = NULL) = 0;

    virtual void received_realtime_data_post(const PsmProtocol * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL) = 0;

    friend class PsmProtocol;

};


class GM_GCL_VDI_API PsmProtocol : public ProtocolCommon, public IProtocolSendMessage, public IProtocolSendFile, public CxIChannelSend, public IPsmExplainNotify
{
public:
    enum DealReplyTypeEnum { DealReplyType_None = 0, DealReplyType_Ack, DealReplyType_Nack, DealReplyType_Define };

public:
    PsmProtocol();

    ~PsmProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Psm; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv & context);

    void describeSelf(CxDescribeUnit * oDescribeUnit) const;

    int postMessageCommand(int iTargetId, const char * sCommand, const char * sParam = 0, int iParamLength = 0 );

    int postMessageCommand(const char * sCommand, const char * sParam = 0, int iParamLength = 0, const PsmAttach * oAttach = NULL);

    int postFile( int iTargetId, const std::string & sFilePath );

    int postFile( const std::string & sFilePath, const PsmAttach * oAttach = NULL );

    int postRealtimeDataRequest(int iTargetId);

    int postRealtimeDataRequest(const PsmAttach * oAttach = NULL );

    int postRealtimeDataPost(int iTargetId, int iParamType, const char * pData, int iDataLength, int iParamCount = 1);

    int postRealtimeDataPost(int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

    inline void addObserver(IPsmProtocolNotify * oNotify) { if (! CxContainer::contain(_psmProtocolNotifies, oNotify)) _psmProtocolNotifies.push_back(oNotify); }

    inline void removeObserver(IPsmProtocolNotify * oNotify) { CxContainer::remove(_psmProtocolNotifies, oNotify); }

    int send_data(const char* pData, int iLength, int iTargetId = 0);

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

    void monsbsValueChanged(const std::map<const MonsbMeasure *, int> & monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag);

    void ycaddsValueChanged(const std::map<const YcaddMeasure *, double> & ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag);

    void checkProtocol();

    void doTask(GCCommon::GCDataElement dt);

    int post_message(const std::string &sCommand, const std::string &sParam);

    int post_file(const std::string &sFilePath);

    inline int received_packet(const PsmExplainer * oPsm, int oTag = 0) { return 0; }

    void received_message_command(const PsmExplainer * oPsm, const char * sCommand, const char * sParam, int iParamLength, const PsmAttach * oAttach = NULL);

    void received_file_write(const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach = NULL);

    void received_realtime_data_request(const PsmExplainer * oPsm, const PsmAttach * oAttach = NULL);

    void received_realtime_data_post(const PsmExplainer * oPsm, int iParamType, const char * pData, int iDataLength, int iParamCount = 1, const PsmAttach * oAttach = NULL);

    void result_file_write(const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const PsmAttach * oAttach = NULL);

public:
    int containerId(const char *sContainerCode);

    std::string containerCode(int iContainerId);

    std::string pathInfoToBuffer(const CxFileSystem::PathInfo &pathInfo);

    CxFileSystem::PathInfo pathInfoFromBuffer(const char * pData, size_t iLength);    

    //发送所有遥信
    int sendAllYC();
    //发送所有遥测
    int senddAllYX();
    //发送所有遥文
    int senddAllYb();

    //默认的 Attach
    inline const PsmAttach * defaultAttach() { return &_defaultAttach; }

public:


protected:
    int sendAllYcadd(YcaddTypeEnum eYcaddType, double *value, SendMeasureArgvs argvs);

    int sendAllMonsb(MonsbTypeEnum eMonsbType, int *value, SendMeasureArgvs argvs);

private:
    //config
    PsmExplainer _psmExplainer;
    std::vector<IPsmProtocolNotify *> _psmProtocolNotifies;

    char _sendBuffer[CI_PSM_PACKET_DATA];

    int _cfgSentReason;
    int _cfgSentContainerId;
    int _cfgSentSourceId;
    int _cfgSentTargetId;
    int _cfgSentTag;
    int _cfgHeartJumpInterval;
    int _cfgSendYxInterval;
    int _cfgSendYcInterval;
    int _cfgSendYwInterval;

    PsmAttach _defaultAttach;

    CxTimer* _heartJumpTimer;

//*business
private:
    int sendHeartJump();

private:
   void test();

public:
   int dealYK_01(const std::string & sParam);

   int dealYT_01(const std::string & sParam);

   int dealYS_01(const std::string & sParam);

   int dealYX_01(const std::string & sParam);

   int dealYC_01(const std::string & sParam);

   int dealYM_01(const std::string & sParam);

   int dealVTL_01(const std::string & sParam);

   int dealMessage_01(const std::string & sParam);

   int sendYX(uint32 id);

   int sendYC(uint32 id);

   int sendYK(uint32 id);

   int sendSC(uint32 id);

   //发送字符串值
   int sendSTR(GCCommon::GCDataElement &dt);


};

class GM_GCL_VDI_API PsmProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(PsmProtocolFactory)

public:
    std::string factoryName() {
        return CS_ProtocolTypeSimpleName_Psm;
    }

    ProtocolBase* createObject() {
        return new PsmProtocol();
    }

};

#endif // PSM_PROTOCOL_H
