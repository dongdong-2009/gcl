#ifndef MODBUS_COMMON_H
#define MODBUS_COMMON_H

#include <vdi/protocol_common.h>
#include "modbus_asdu.h"

class ModBus_Common_Protocol : public ProtocolCommon
{

public:
    ModBus_Common_Protocol();

    ~ModBus_Common_Protocol();
    //协议类型
    inline const GString& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus_Common; }
    //存储参数
    void toContext(CxISetSkv& context) const ;
    //加载参数
    void fromContext(const CxIGetSkv& context);
    //*control:send
    int sendControl(const MonsbMeasure* oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs());
    //*control:provide?
    bool provideSendControl(int iMeasureSonType, const MonsbMeasure* oMonsb = NULL);

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

public:
    //数据处理
    virtual int  findHeader(uchar *pData,int iLength)=0;
    //帧检查
    virtual bool checkFrame(uchar *pData, int iLength,int &nFrmLen)=0;
    //
    virtual int setUpFrame(bool isResp = true, bool sendNow=true)=0;
    //发送缓冲区
    virtual bool sendBuffer()=0;
    //
    virtual bool protolAfterVfy()=0;

    virtual void protolAfterSend()=0;

public:
    //custom data
    int sendCustomData(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData);

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

protected:
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

    void DealReadStatus(int iReadSlaveAddress, int iReadAddress, int iReadCount);

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

    void ResponseReadStatus(uchar byFuncCode);

    void ResponseReadRegisters(uchar byFuncCode,int mode);

    int SendModbusReadData(uchar iSlaveAddress, uchar byFuncCode, ushort iAddress, ushort iQuantity,bool bResp=false);

    int SendModbusReadData(uchar iSlaveAddress, uchar byFuncCode, const std::vector<uchar>& data,bool bResp=false);

    int SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort iAddress, ushort data);

    int SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort iAddress, ushort iQuantity, std::vector<uchar> &data);

    //地址修正
    int addrMod(int addr, bool bSend=false);

    //初始化 _stationMinMaxMids
    template<class AddressWorkerSon>
    void initStationMinMaxMids(const AddressWorkerTableTemplate<AddressWorkerSon> * oWorkerTable)
    {
        for (size_t i = 0; i < oWorkerTable->stationIds().size(); ++i)
        {
            int iStation = oWorkerTable->stationIds().at(i);
            std::map<int,int>::const_iterator itMin = oWorkerTable->stationIdMins().find(iStation);
            if (itMin != oWorkerTable->stationIdMins().end())
            {
                std::map<int,int>::const_iterator itMax = oWorkerTable->stationIdMaxes().find(iStation);
                if (itMax != oWorkerTable->stationIdMaxes().end())
                {
                    int iMinAddress = itMin->second;
                    int iMaxAddress = itMax->second;
                    if (iStation>=0 && iMinAddress>=0 && iMaxAddress>=0)
                    {
                        std::map<int,std::pair<int, int> >::const_iterator itMinMaxMid = _stationMinMaxMids.find(iStation);
                        if (itMinMaxMid != _stationMinMaxMids.end())
                        {
                            const std::pair<int, int> & iMinMaxMid2 = itMinMaxMid->second;
                            if (iMinAddress>iMinMaxMid2.first) iMinAddress = iMinMaxMid2.first;
                            if (iMaxAddress<iMinMaxMid2.second) iMaxAddress = iMinMaxMid2.second;
                        }
                        std::pair<int, int> iMinMaxMid(iMinAddress, iMaxAddress);
                        _stationMinMaxMids[iStation] = iMinMaxMid;
                    }
                }
            }
        }
    }

protected:
    uchar _readCoilStatusIndex;
    uchar _readInputStatusIndex;
    uchar _readInputRegistersIndex;

    CxTimer* _timeReadCoilStatus;
    CxTimer* _timeReadInputStatus;
    CxTimer* _timeReadHoldingRegisters;
    CxTimer* _timeReadInputRegisters;
    CxTimer* _timeSendBlock; //阻塞调用

    //config
    int _readCoilStatusInterval;
    int _readInputStatusInterval;
    int _readHoldingRegistersInterval;
    int _readInputRegistersInterval;

    bool _readCoilStatusIsMaster;
    bool _readInputStatusIsMaster;
    bool _readHoldingRegistersIsMaster;
    bool _readInputRegistersIsMaster;
    bool _forceSingleCoilIsMaster;
    bool _presetSingleRegisterIsMaster;
    bool _forceMultipleCoilsIsMaster;
    bool _presetMultipleRegistersIsMaster;

    //read
    //slaveNumber = <minMid, maxMid>
    std::map<int, std::pair<int, int> > _stationMinMaxMids;
    std::map<int, std::pair<int, int> >::const_iterator _readHoldingRegistersIt;

    //rec
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

protected:
    ModBusASDU     _asduRev,_asduSend,_asduLast;
    uchar _sendBuf[GCL_LEN_DATA_EM];
    uint16 _sendLen;
    int  _dataMode;

    std::queue<std::vector<uchar> > _sentFrms;
    std::vector<uchar> _sendLast;
    msepoch_t _tmLast;
    bool _sendEnable;
//test
protected:
    int TestReadCoilStatus();
    int TestReadInputRegisters();
    int TestReadInputStatus();
    int TestReadHoldingRegisters();
protected:
    bool _bTest;
    uint16 _sessionFlag;
//数据类型  标准定，自定义变长
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
//地址类型  0 西门子标准  1 GE和modicon标准
    int _addrType;
//地址数上限
    int _addrLimit;
//遥控功能码
    int _ykFunctionCode;

};

#endif // MODBUS_COMMON_H
