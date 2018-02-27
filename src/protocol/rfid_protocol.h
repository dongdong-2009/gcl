#ifndef RFID_PROTOCOL_H
#define RFID_PROTOCOL_H


#include <vdi/protocol_base.h>

#include "rfid_ywg_frame.h"


class RfidProtocol : public ProtocolBase
{

public:

    RfidProtocol();

    ~RfidProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Rfid; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

public:
    //返回查询时间
    long long query();
    //返回最后获取到RFID号的时间
    inline long long lastTimeUpdateRfid() const { return _lastTimeUpdateRfid; }
    //返回最后获取到的RFID号
    inline std::string lastRfid() const { return _lastRfid; }

    inline std::string lastBarcode() const { return _lastBarcode; }

    inline std::string lastResponse() const { return _lastResponse; }

    inline bool isSucced(){return _bSucced;}
private:
    //初始化
    int initRFID();
    //处理数据
    void dealDataEx();
    //处理数据
    void dealData2();
    //功率处理
    void dealPower();
    //通用读解析
    void dealReadGM();
    //发送通用读
    int sendReadGM();
    //发送功率查询
    int sendQueryPower();
    //关闭
    int sendPoweroff();
    //组帧
    int setUpFrame();
    //寻找头部
    int FindHeader(uchar *pData, int iLength);
    //帧检查
    bool CheckFrame(uchar *pData, int iLength,int &nFrmLen);
private:
    uchar _rfidBuffer[RFID_LEN_BUF];
    int _rfidLen;

    uchar _rfidTmp[RFID_LEN_BUF];
    int _tmpLen;

    uchar _rfidTmp2[RFID_LEN_BUF];
    int _tmpLen2;

    uchar _sendBuf[RFID_LEN_BUF];
    uint16 _sendLen;

    int _rfidMode;
    int _extraLength;
    int _scanerInterval;
//    Timer *_timeScaner;
    int _started;

    std::string _lastRfid, _lastBarcode, _lastResponse,_lastEPC;
    long long _lastTimeUpdateRfid;

    bool _bSucced;

private:
    RfidYWGFrame _recvFrm;
    RfidYWGFrame _sendFrm;
};

class RfidProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(RfidProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Rfid;
    }

    ProtocolBase* createObject(){
        return new RfidProtocol();
    }
};


#endif // RFID_PROTOCOL_H
