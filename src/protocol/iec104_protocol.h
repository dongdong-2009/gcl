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



#include <vdi/protocol_base.h>
#include <vdi/monsb_address_worker.h>
#include <vdi/ycadd_address_worker.h>
#include <vdi/send_monsb_control_cases.h>


const std::string CS_EntryIEC104CallUpInterval = "CallUpInterval";


const int C104BUFLEN = 255;


class IEC104Protocol : public ProtocolBase, public IProtocolSendMonsbControl
{

public:

    IEC104Protocol();

    ~IEC104Protocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_IEC104; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    //control
    int sendControl(const MonsbMeasure* oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs());

    int sendAllControl(int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs());

    bool provideSendControl(int iMeasureSonType, const MonsbMeasure* oMonsb = NULL);

    //receive
    //YX
    inline const MonsbAddressWorkerTable* YxRecTable() const            { return _yxRecTable            ; }
    //YK
    inline const MonsbAddressWorkerTable* YkRecTable() const            { return _ykRecTable            ; }
    //YC
    inline const YcaddAddressWorkerTable* YcRecTable() const            { return _ycRecTable            ; }

    //send
    //YX
    inline const MonsbAddressWorkerTable* YxSendTable() const           { return _yxSendTable           ; }
    //YK
    inline const MonsbAddressWorkerTable* YkSendTable() const           { return _ykSendTable           ; }
    //YC
    inline const YcaddAddressWorkerTable* YcSendTable() const           { return _ycSendTable           ; }

    int SendYK(uchar funcode, ushort reason, ushort stationId, int address, bool gotoHe, bool isPerform = true);

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void channelAfterOpen(const CxChannelBase* oChannel);

    void channelAfterClose(const CxChannelBase* oChannel);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

    void monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag);

    void ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag);

    //data / field
private:
    MonsbAddressWorkerTable* _yxRecTable;
    MonsbAddressWorkerTable* _ykRecTable;
    YcaddAddressWorkerTable* _ycRecTable;

    MonsbAddressWorkerTable* _yxSendTable;
    MonsbAddressWorkerTable* _ykSendTable;
    YcaddAddressWorkerTable* _ycSendTable;

    std::vector<int> _stationIdAll;

    SendMonsbControlCases* _sendMonsbControl;

    CxTimer* _timeCallUp;

    //config
    int _callUpInterval;

    int _stationIndex;

    int FRecBufState;
    int FRecBufLen;
    uchar FRecBuf[C104BUFLEN];
    int FRecBufIndex;
    uint FCalcerS;
    uint FCalcerR;
    uchar FRecTypeFlag;

    bool _started;
    bool _ykInitiative;

    //method
private:
    void CallUp();

    void CallUp(ushort stationId);

    int SendCallUp(ushort reason, ushort stationId, int address = 0);

    int SendFrameI(uchar *data, int length);

    void DealData();

    void DealFrameI();

    void ResetCalcer();

    void DealYX();

    void DealYC();

    void DealYK();

    void DealYK(int stationId, int address, uchar ation);

    void DealCallUp(ushort wStationId);

    void SendSureFrameI();

    void SendStart();

    void SendChannelCheckConfirmed();

    void SendStartConfirmed();

    void SendStopConfirmed();

    friend class IEC104MasterProtocol;
    friend class IEC104SlaveProtocol;

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
