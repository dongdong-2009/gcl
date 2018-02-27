#ifndef MODBUS_TCP_COMMON_H
#define MODBUS_TCP_COMMON_H

#include <vdi/protocol_common.h>
#include "modbus_tcp_head.h"
#include "modbus_asdu.h"

class ModBus_TCP_Protocol : public ProtocolCommon
{

public:
    ModBus_TCP_Protocol();

    ~ModBus_TCP_Protocol();
    //协议类型
    inline const GString& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus_TCP; }
    //存储参数
    void toContext(CxISetSkv& context) const ;
    //加载参数
    void fromContext(const CxIGetSkv& context);
public:
    //定时器
   void protocolTimerTimeOut(const CxTimer* oTimer);
    //数据处理
    void protocolReceivedData(const uchar* pData, int iLength);
    //启动
    void protocolAfterStart();
    //停止
    void protocolAfterStop();
    //任务
    void doTask(GCCommon::GCDataElement dt);
    //校时
    void clock();
    //数据处理
    void dealData();
    //总召
    void callUp();
    //custom data
    int sendCustomData(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData);
private:
    //数据处理
    int  findHeader(uchar *pData,int iLength);
    //帧检查
    bool checkFrame(uchar *pData, int iLength,int &nFrmLen);
    //
    int setUpFrame(bool isResp = true, bool sendNow=true);

    int sendVTLData(int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData);

private:
    //线圈状态
    int ReadCoilStatus(int iSlaveAddress, int iAddress, ushort wCount);
    //
    int ReadInputStatus(int iSlaveAddress, int iAddress, ushort wCount);
    //保持寄存器
    int ReadHoldingRegisters(int iSlaveAddress, int iAddress, ushort wCount);

    int ReadInputRegisters(int iSlaveAddress, int iAddress, ushort wCount);

    int ForceSingleCoil(int iSlaveAddress, int iAddress, bool bOn);

    int PresetSingleRegister(int iSlaveAddress, int iAddress, ushort wForceData);

    int ForceMultipleCoils(int iSlaveAddress, int iAddress, const std::vector<bool> &values);

    int PresetMultipleRegisters(int iSlaveAddress, int iAddress, const std::vector<ushort>& values);

private:
    void TimeReadCoilStatus();

    void TimeReadInputStatus();

    void TimeReadHoldingRegisters();

    void TimeReadInputRegisters();

    void ModbusReceiveModelResponse();

    void ModbusReceiveModelReply();

    void ModbusReceiveModelDeal();

    void ModbusReceiveModelForce();

    //master deal
    void DealReadCoilStatus();

    void DealReadInputStatus();

    void DealReadHoldingRegisters();

    void DealReadInputRegisters();

    //master not deal
    //void DealForceSingleCoil();
    //void DealPresetSingleRegister();
    //void DealForceMultipleCoils();
    //void DealPresetMultipleRegisters();

    void DealReadStatus(bool bIsYx, int iReadSlaveAddress, int iReadAddress, int iReadCount);

    void DealReadRegisters(int iReadSlaveAddress, int iReadAddress, int iReadCount,int mode);

    //slave response
    void ResponseReadCoilStatus();

    void ResponseReadInputStatus();

    void ResponseReadHoldingRegisters();

    void ResponseReadInputRegisters();

    void ResponseForceSingleCoil();

    void ResponsePresetSingleRegister();

    void ResponseForceMultipleCoils();

    void ResponsePresetMultipleRegisters();

    void ResponseReadStatus(bool bIsYx, uchar byFuncCode);

    void ResponseReadRegisters(uchar byFuncCode);

    int SendModbusReadData(uchar iSlaveAddress, uchar byFuncCode, ushort iAddress, ushort iQuantity);

    int SendModbusReadData(uchar iSlaveAddress, uchar byFuncCode, const std::vector<uchar>& data);

    int SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort iAddress, ushort data);

    int SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort iAddress, ushort iQuantity, std::vector<uchar> &data);
    //发送缓冲区
    bool sendBuffer();
private:
    uchar _readCoilStatusIndex;
    uchar _readInputStatusIndex;
    uchar _readHoldingRegistersIndex;
    uchar _readInputRegistersIndex;

    CxTimer* _timeReadCoilStatus;
    CxTimer* _timeReadInputStatus;
    CxTimer* _timeReadHoldingRegisters;
    CxTimer* _timeReadInputRegisters;
    CxTimer* _timeSendBlock; //阻塞调用

    std::vector<int> _stationIdAll;

    //config
    int _readCoilStatusInterval;
    int _readInputStatusInterval;
    int _readHoldingRegistersInterval;
    int _readInputRegistersInterval;

    bool _readCoilStatusIsYx;
    bool _readInputStatusIsYx;
    bool _forceSingleCoilIsYx;
    bool _forceMultipleCoilsIsYx;

    bool _readCoilStatusIsMaster;
    bool _readInputStatusIsMaster;
    bool _readHoldingRegistersIsMaster;
    bool _readInputRegistersIsMaster;
    bool _forceSingleCoilIsMaster;
    bool _presetSingleRegisterIsMaster;
    bool _forceMultipleCoilsIsMaster;
    bool _presetMultipleRegistersIsMaster;

    int _recState;
    uchar _recSlaveAddress;
    uchar _recFuncCode;
    int _recModelState;
    int _recDealDataIndex;
    std::vector<uchar> _recDealData;
    ushort _recResponseAddress;
    ushort _recResponseQuantity;
    ushort _recForceAddress;
    ushort _recForceSingleData;
    ushort _recForceMultipleQuantity;
    int _recForceMultipleDataIndex;
    std::vector<uchar> _recForceMultipleData;
    ushort _recCrc;
    uchar _recData[GCL_LEN_DT];

    int _readCoilStatusSlaveAddress;
    int _readCoilStatusAddress;
    int _readCoilStatusCount;

    int _readInputStatusSlaveAddress;
    int _readInputStatusAddress;
    int _readInputStatusCount;

    int _readHoldingRegistersSlaveAddress;
    int _readHoldingRegistersAddress;
    int _readHoldingRegistersCount;

    int _readInputRegistersSlaveAddress;
    int _readInputRegistersAddress;
    int _readInputRegistersCount;

    friend class TCP_ModbusMasterProtocol;
    friend class TCP_ModbusSlaveProtocol;
private:
    ModBusTcpHead  _headRev,_headSend,_headLast;
    ModBusASDU     _asduRev,_asduSend,_asduLast;
    uchar _sendBuf[GCL_LEN_DATA_EM];
    uint16 _sendLen;
    int  _dataMode;

    std::queue<std::vector<uchar> > _sentFrms;
    std::vector<uchar> _sendLast;
    msepoch_t _tmLast;
    bool _sendEnable;
//test
private:
    int TestReadCoilStatus();
    int TestReadInputRegisters();
    int TestReadInputStatus();
    int TestReadHoldingRegisters();
private:
    bool _bTest;
    uint16 _sessionFlag;
//数据类型
    int  _typeCoil;
    int  _typeInputStatus;
    int  _typeHodingRegister;
    int  _typeInputRegister;
//命令缓冲区
    std::string _szBuffersCoil;
    std::string _szBuffersInputStatus;
    std::string _szBuffersHodingRegister;
    std::string _szBuffersInputRegister;

    std::vector<std::vector<char> > _cmdBuffersCoil;
    std::vector<std::vector<char> > _cmdBuffersInputStatus;
    std::vector<std::vector<char> > _cmdBuffersHodingRegister;
    std::vector<std::vector<char> > _cmdBuffersInputRegister;
//命令模式
    int  _modeCoil;
    int  _modeInputStatus;
    int  _modeHodingRegister;
    int  _modeInputRegister;
};

class ModBus_TCP_ProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(ModBus_TCP_ProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Modbus_TCP;
    }

    ProtocolBase* createObject(){
        return new ModBus_TCP_Protocol();
    }
};

#endif // MODBUS_TCP_COMMON_H
