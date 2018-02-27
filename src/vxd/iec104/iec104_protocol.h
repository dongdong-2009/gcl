#ifndef IEC104_PROTOCOL_H
#define IEC104_PROTOCOL_H

/*
todo:
1、多站召唤“Callup”还没做的。
2、响应召唤而发送的遥信目前实现了地址是”连续“的方式，还有“不连续”的没有实现
3、响应召唤而发送的遥测目前实现了地址是”连续“的方式，还有“不连续”的没有实现
4、遥控没有支持多站
5、通道检测没有做好，要做成有数据收时就不用检测通道了

APDU：
APCI + ASDU:

启动字符 + APDU的长度 + 控制域 +
类型标识 + 可变结构限定词 + 传送原因 + 应用服务数据单元公共地址 +
信息对象数据

1 + 1 + 4 +
1 + 1 + 2 + 2 +
x * N

遥信单点、双点的数据结构
( 3 + N )              连续 不带时标
( [3 + 1] * N )        不连续 不带时标
( 3 + [1 + 7] * N )    连续 带时标
( [3 + 1 + 7] * N )    不连续 带时标

遥测规一化、标度化的数据结构
( 3 + [3] * N )        连续 不带时标
( [3 + 3] * N )        不连续 不带时标
( 3 + [3 + 7] * N )    连续 带时标
( [3 + 3 + 7] * N )    不连续 带时标

遥测短浮点的数据结构
( 3 + [5] * N )        连续 不带时标
( [3 + 5] * N )        不连续 不带时标
( 3 + [5 + 7] * N )    连续 带时标
( [3 + 5 + 7] * N )    不连续 带时标
*/


#include <vdi/protocol_common.h>

const int C104BUFLEN = 255;


class IEC104Protocol : public ProtocolCommon
{

public:

    IEC104Protocol();

    ~IEC104Protocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_IEC104; }

    //control
    int sendControl(const MonsbMeasure* oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs());

    bool provideSendControl(int iMeasureSonType, const MonsbMeasure* oMonsb = NULL);

    int sendYK(uchar funcode, ushort reason, ushort stationId, int address, bool gotoHe, bool isPerform = true);
    //custom data
    int sendCustomData(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData);

protected:
    void protocolTimerTimeOut(const CxTimer* oTimer);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

    void doTask(GCCommon::GCDataElement dt);

    void clock();
private:
    std::vector<int> _stationIdAll;

    int _stationIndex;

    int FRecBufState;
    int FRecBufLen;
    uchar FRecBuf[C104BUFLEN];
    int FRecBufIndex;
    uint FCalcerS;
    uint FCalcerR;
    uchar FRecTypeFlag;

    uchar _sendBuf[GCL_LEN_DT];
    int   _sendLen;
    //method
private:
    void callUp();

    void callUp(ushort stationId);

    int sendCallUp(ushort reason, ushort stationId, int address = 0);

    int sendFrameI(uchar *data, int length);

    int sendFrameI2(uchar *data, int length);

    void dealData();

    void dealFrameI();

    void resetCalcer();

    void dealYX();

    void dealYC();

    void dealYw();

    void dealYK();

    void dealYK(int stationId, int address, uchar ation);

    void dealCallUp(ushort wStationId);

    void sendSureFrameI();

    void sendStart();

    void sendChannelCheckConfirmed();

    void sendStartConfirmed();

    void sendStopConfirmed();

    //发送遥调
    int sendYT(uchar funcode, ushort reason, ushort stationId, int address,char *value);

    //发送字符串
    int sendSTR(uchar funcode, ushort reason, ushort stationId, int address, char *value, int len);

private:
    friend class IEC104MasterProtocol;
    friend class IEC104SlaveProtocol;

private:
    double _fVal;
    bool  _bVal;

};


class IEC104MasterProtocol : public IEC104Protocol
{
public:
    IEC104MasterProtocol();

    ~IEC104MasterProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_IEC104_Master; }

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

};

class IEC104SlaveProtocol : public IEC104Protocol
{
public:
    IEC104SlaveProtocol();

    ~IEC104SlaveProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_IEC104_Slave; }

};

class IEC104MasterProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(IEC104MasterProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_IEC104_Master;
    }

    ProtocolBase* createObject(){
        return new IEC104MasterProtocol();
    }

};

class IEC104SlaveProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(IEC104SlaveProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_IEC104_Slave;
    }

    ProtocolBase* createObject(){
        return new IEC104SlaveProtocol();
    }

};
#endif // IEC104_PROTOCOL_H
