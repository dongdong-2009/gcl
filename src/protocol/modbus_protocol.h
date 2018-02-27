#ifndef MODBUSPROTOCOL_H
#define MODBUSPROTOCOL_H


#include <vdi/protocol_base.h>


#include <vdi/monsb_address_worker.h>
#include <vdi/ycadd_address_worker.h>
#include <vdi/send_monsb_cases.h>
#include <vdi/send_ycadd_cases.h>
#include <vdi/send_monsb_control_cases.h>



class ModbusProtocol : public ProtocolBase, public IProtocolSendMonsb, public IProtocolSendYcadd, public IProtocolSendMonsbControl
{
public:
    ModbusProtocol();

    ~ModbusProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    //monsb send
    int sendMonsb(const MonsbMeasure* oMonsb, int value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    int sendMonsbs(const std::map<const MonsbMeasure*, int>& monsbValues, MonsbTypeEnum eMonsbType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    int sendAllMonsb(MonsbTypeEnum eMonsbType, int * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    //ycadd send
    int sendYcadd(const YcaddMeasure* oYcadd, double value, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    int sendYcadds(const std::map<const YcaddMeasure*, double>& ycaddsValue, YcaddTypeEnum eYcaddType, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    int sendAllYcadd(YcaddTypeEnum eYcaddType, double * value = NULL, SendMeasureArgvs argvs = SendMeasureArgvs::NullArgs());

    //control
    int sendControl(const MonsbMeasure* oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs = SendControlArgvs::nullArgs());

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

    //buffer manufacture
    int ReadCoilStatus(int iSlaveAddress, int iAddress, ushort wCount);

    int ReadInputStatus(int iSlaveAddress, int iAddress, ushort wCount);

    int ReadHoldingRegisters(int iSlaveAddress, int iAddress, ushort wCount);

    int ReadInputRegisters(int iSlaveAddress, int iAddress, ushort wCount);

    int ForceSingleCoil(int iSlaveAddress, int iAddress, bool bOn);

    int PresetSingleRegister(int iSlaveAddress, int iAddress, ushort wForceData);

    int ForceMultipleCoils(int iSlaveAddress, int iAddress, const std::vector<bool> &values);

    int PresetMultipleRegisters(int iSlaveAddress, int iAddress, const std::vector<ushort>& values);

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

    void monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag);

    void ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag);

private:
    void TimeReadCoilStatus();

    void TimeReadInputStatus();

    void TimeReadHoldingRegisters();

    void TimeReadInputRegisters();

    void ModbusReceiveModelResponse(uchar data);

    void ModbusReceiveModelReply(uchar data);

    void ModbusReceiveModelDeal(uchar data);

    void ModbusReceiveModelForce(uchar data);

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

    void DealReadRegisters(int iReadSlaveAddress, int iReadAddress, int iReadCount);

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

    int SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort iAddress, ushort iQuantity, const std::vector<uchar>& data);

//data / field
private:
    MonsbAddressWorkerTable* _yxRecTable;
    MonsbAddressWorkerTable* _ykRecTable;
    YcaddAddressWorkerTable* _ycRecTable;

    MonsbAddressWorkerTable* _yxSendTable;
    MonsbAddressWorkerTable* _ykSendTable;
    YcaddAddressWorkerTable* _ycSendTable;

    std::vector<int> _stationIdAll;

    SendMonsbCases* _sendMonsb;
    SendYcaddCases* _sendYcadd;
    SendMonsbControlCases* _sendMonsbControl;

    uchar _readCoilStatusIndex;
    uchar _readInputStatusIndex;
    uchar _readHoldingRegistersIndex;
    uchar _readInputRegistersIndex;

    CxTimer* _timeReadCoilStatus;
    CxTimer* _timeReadInputStatus;
    CxTimer* _timeReadHoldingRegisters;
    CxTimer* _timeReadInputRegisters;

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
    uchar _recData[1024];

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

    friend class ModbusMasterProtocol;
    friend class ModbusSlaveProtocol;
};

class ModbusMasterProtocol : public ModbusProtocol
{
public:
    ModbusMasterProtocol();

    ~ModbusMasterProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus_Master; }

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

};

class ModbusSlaveProtocol : public ModbusProtocol
{
public:
    ModbusSlaveProtocol();

    ~ModbusSlaveProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Modbus_Slave; }

};

class ModbusMasterProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(ModbusMasterProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Modbus_Master;
    }

    ProtocolBase* createObject(){
        return new ModbusMasterProtocol();
    }

};

class ModbusSlaveProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(ModbusSlaveProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Modbus_Slave;
    }

    ProtocolBase* createObject(){
        return new ModbusSlaveProtocol();
    }

};
#endif // MODBUSPROTOCOL_H
