#include "modbus_protocol.h"





#include <vector>


ModbusMasterProtocolFactory* ModbusMasterProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<ModbusMasterProtocolFactory>();


//ModbusSlaveProtocolFactory* ModbusSlaveProtocolFactory::s_instance =
//        ProtocolFactoryManager::createAndRegister<ModbusSlaveProtocolFactory>();


ModbusProtocol::ModbusProtocol()
{
    _yxRecTable  = createAndRegisterWorkerTable<MonsbAddressWorkerTable, ModbusProtocol>("YxReceiveTable", MonsbType_rtu_state);
    _ykRecTable  = createAndRegisterWorkerTable<MonsbAddressWorkerTable, ModbusProtocol>("YkReceiveTable", MonsbType_rtu_state);
    _ycRecTable  = createAndRegisterWorkerTable<YcaddAddressWorkerTable, ModbusProtocol>("YcReceiveTable", YcaddType_rtu_value);

    _yxSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, ModbusProtocol>("YxSendTable", MonsbType_rtu_state);
    _ykSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, ModbusProtocol>("YkSendTable", MonsbType_rtu_state);
    _ycSendTable = createAndRegisterWorkerTable<YcaddAddressWorkerTable, ModbusProtocol>("YcSendTable", YcaddType_rtu_value);

    _yxRecTable ->setMinValidAddress(1);
    _ykRecTable ->setMinValidAddress(1);
    _ycRecTable ->setMinValidAddress(1);

    _yxSendTable->setMinValidAddress(1);
    _ykSendTable->setMinValidAddress(1);
    _ycSendTable->setMinValidAddress(1);

    _sendMonsb   = createAndRegisterCases<ModbusProtocol, SendMonsbCases>(this);
    _sendYcadd   = createAndRegisterCases<ModbusProtocol, SendYcaddCases>(this);
    _sendMonsbControl= createAndRegisterCases<ModbusProtocol, SendMonsbControlCases>(this);

    _timeReadCoilStatus       = new CxTimer();
    _timeReadInputStatus      = new CxTimer();
    _timeReadHoldingRegisters = new CxTimer();
    _timeReadInputRegisters   = new CxTimer();

    _timeReadCoilStatus      ->setTimerTimeOut(this);
    _timeReadInputStatus     ->setTimerTimeOut(this);
    _timeReadHoldingRegisters->setTimerTimeOut(this);
    _timeReadInputRegisters  ->setTimerTimeOut(this);

    _readCoilStatusInterval = 0;
    _readInputStatusInterval = 0;
    _readHoldingRegistersInterval = 0;
    _readInputRegistersInterval = 0;

    _readCoilStatusIsYx = true;
    _readInputStatusIsYx = true;
    _forceSingleCoilIsYx = false;
    _forceMultipleCoilsIsYx = false;
}

ModbusProtocol::~ModbusProtocol()
{
    delete _timeReadCoilStatus      ;
    delete _timeReadInputStatus     ;
    delete _timeReadHoldingRegisters;
    delete _timeReadInputRegisters  ;
}

void ModbusProtocol::toContext(CxISetSkv &context) const
{
    ProtocolBase::toContext(context);
    context.setValue(CS_EntryModbusReadCoilStatusInterval,       _readCoilStatusInterval);
    context.setValue(CS_EntryModbusReadInputStatusInterval,      _readInputStatusInterval);
    context.setValue(CS_EntryModbusReadHoldingRegistersInterval, _readHoldingRegistersInterval);
    context.setValue(CS_EntryModbusReadInputRegistersInterval,   _readInputRegistersInterval);
}

void ModbusProtocol::fromContext(const CxIGetSkv &context)
{
    ProtocolBase::fromContext(context);
    _readCoilStatusInterval         = context.getValue(CS_EntryModbusReadCoilStatusInterval ,        _readCoilStatusInterval);
    _readInputStatusInterval        = context.getValue(CS_EntryModbusReadInputStatusInterval ,       _readInputStatusInterval);
    _readHoldingRegistersInterval   = context.getValue(CS_EntryModbusReadHoldingRegistersInterval ,  _readHoldingRegistersInterval);
    _readInputRegistersInterval     = context.getValue(CS_EntryModbusReadInputRegistersInterval ,    _readInputRegistersInterval);
}

void ModbusProtocol::toReport(std::vector<std::string> &sReports) const
{
    sReports.push_back(std::string("ReadCoilStatusInterval      : " + CxString::toString(_readCoilStatusInterval      )));
    sReports.push_back(std::string("ReadInputStatusInterval     : " + CxString::toString(_readInputStatusInterval     )));
    sReports.push_back(std::string("ReadHoldingRegistersInterval: " + CxString::toString(_readHoldingRegistersInterval)));
    sReports.push_back(std::string("ReadInputRegistersInterval  : " + CxString::toString(_readInputRegistersInterval  )));
}

void ModbusProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
}

int ModbusProtocol::sendMonsb(const MonsbMeasure *oMonsb, int value, SendMeasureArgvs argvs)
{
    if (! oMonsb)
        return SendMeasureFail_Nomeasure;
    MonsbAddressWorker* oWorker = _yxSendTable->workerByMeasure(oMonsb);
    if (! oWorker)
        return SendMeasureFail_Nomeasure;
    //ForceSingleCoil
    //ForceMultipleCoils
    int iSlaveAddress = oWorker->stationId();
    int iAddress = oWorker->address();
    bool bOn = oMonsb->value() == CIRtuState_He;
    if (_forceSingleCoilIsYx)
    {
        return ForceSingleCoil(iSlaveAddress, iAddress, bOn);
    }
    else if (_forceMultipleCoilsIsYx)
    {
        std::vector<bool> values;
        values.push_back(bOn);
        return ForceMultipleCoils(iSlaveAddress, iAddress, values);
    }
    else
    {
        cxDebug() << protocolName() << " - sendMonsb -> fail! Monsb : " << oMonsb->measureName();
        return SendMeasureFail_Noprovide;
    }
}

int ModbusProtocol::sendMonsbs(const std::map<const MonsbMeasure *, int> &monsbValues, MonsbTypeEnum eMonsbType, SendMeasureArgvs argvs)
{
    return SendMeasureFail_Nomeasure;
}

int ModbusProtocol::sendAllMonsb(MonsbTypeEnum eMonsbType, int *value, SendMeasureArgvs argvs)
{
    return SendMeasureFail_Noprovide;
}

int ModbusProtocol::sendYcadd(const YcaddMeasure *oYcadd, double value, SendMeasureArgvs argvs)
{
    return SendMeasureFail_Noprovide;
}

int ModbusProtocol::sendYcadds(const std::map<const YcaddMeasure *, double> &ycaddsValue, YcaddTypeEnum eYcaddType, SendMeasureArgvs argvs)
{
    return SendMeasureFail_Noprovide;
}

int ModbusProtocol::sendAllYcadd(YcaddTypeEnum eYcaddType, double *value, SendMeasureArgvs argvs)
{
    return SendMeasureFail_Noprovide;
}

int ModbusProtocol::sendControl(const MonsbMeasure *oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs)
{
    if (! oMonsb)
        return SendMeasureFail_Nomeasure;
    MonsbAddressWorker* oWorker = _ykSendTable->workerByMeasure(oMonsb);
    if (! oWorker)
        return SendMeasureFail_Nomeasure;
    //ForceSingleCoil
    //ForceMultipleCoils
    int iSlaveAddress = oWorker->stationId();
    int iAddress = oWorker->address();
//    bool bOn = oMonsb->value() == CIRtuState_He;
    if (! _forceSingleCoilIsYx) {
        return ForceSingleCoil(iSlaveAddress, iAddress, true);
    } else if (! _forceMultipleCoilsIsYx) {
        std::vector<bool> values;
        values.push_back(true);
        return ForceMultipleCoils(iSlaveAddress, iAddress, values);
    } else {
        cxDebug() << protocolName() << " - sendMonsb -> fail! Monsb : " << oMonsb->measureName();
        return SendControlFail_Noprovide;
    }
}

bool ModbusProtocol::provideSendControl(int iMeasureSonType, const MonsbMeasure *oMonsb)
{
    if (iMeasureSonType == MonsbType_rtu_state) {
        if (oMonsb) {
            return _ykSendTable->workerByMeasure(oMonsb);
        } else {
            return true;
        }
    } else {
        return false;
    }
}

int ModbusProtocol::ReadCoilStatus(int iSlaveAddress, int iAddress, ushort wCount)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x01)
        return -1;
    _readCoilStatusSlaveAddress = iSlaveAddress;
    _readCoilStatusAddress = iAddress;
    _readCoilStatusCount = wCount;
    return SendModbusReadData(iSlaveAddress, 1, iAddress - 1, wCount);
}

int ModbusProtocol::ReadInputStatus(int iSlaveAddress, int iAddress, ushort wCount)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x01)
        return -1;
    _readInputStatusSlaveAddress = iSlaveAddress;
    _readInputStatusAddress = iAddress;
    _readInputStatusCount = wCount;
    return SendModbusReadData(iSlaveAddress, 2, iAddress - 1, wCount);
}

int ModbusProtocol::ReadHoldingRegisters(int iSlaveAddress, int iAddress, ushort wCount)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x01)
        return -1;
    _readHoldingRegistersSlaveAddress = iSlaveAddress;
    _readHoldingRegistersAddress = iAddress;
    _readHoldingRegistersCount = wCount;
    return SendModbusReadData(iSlaveAddress, 3, iAddress - 1, wCount);
}

int ModbusProtocol::ReadInputRegisters(int iSlaveAddress, int iAddress, ushort wCount)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x01)
        return -1;
    _readInputRegistersSlaveAddress = iSlaveAddress;
    _readInputRegistersAddress = iAddress;
    _readInputRegistersCount = wCount;
    return SendModbusReadData(iSlaveAddress, 4, iAddress - 1, wCount);
}

int ModbusProtocol::ForceSingleCoil(int iSlaveAddress, int iAddress, bool bOn)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > 0xFFFF || iAddress < 0x01)
        return -1;
    ushort data = ( true ) ? 0xFF00 : 0x0000;
    return SendModbusForceData(iSlaveAddress, 5, iAddress - 1, data);
}

int ModbusProtocol::PresetSingleRegister(int iSlaveAddress, int iAddress, ushort wForceData)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > 0xFFFF || iAddress < 0x01)
        return -1;
    return SendModbusForceData(iSlaveAddress, 5, iAddress - 1, wForceData);
}

int ModbusProtocol::ForceMultipleCoils(int iSlaveAddress, int iAddress, const std::vector<bool> &values)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - values.size()) || iAddress < 0x01)
        return -1;
    std::vector<uchar> data = CxBuffer::toBuffer(values);
    return SendModbusForceData(iSlaveAddress, 15, iAddress - 1, values.size(), data);
}

int ModbusProtocol::PresetMultipleRegisters(int iSlaveAddress, int iAddress, const std::vector<ushort> &values)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - values.size()) || iAddress < 0x01)
        return -1;
    std::vector<uchar> data;
    data.resize(values.size() * 2);
    for (size_t i = 0; i < values.size(); ++i) {
        ushort value = values.at(i);
        data[ i * 2 ] = value >> 8;
        data[ i * 2 + 1 ] = value;
    }
    return SendModbusForceData(iSlaveAddress, 16, iAddress - 1, values.size(), data);
}

void ModbusProtocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
    if (oTimer == _timeReadCoilStatus) {
        TimeReadCoilStatus();
    } else if (oTimer == _timeReadInputStatus) {
        TimeReadInputStatus();
    } else if (oTimer == _timeReadHoldingRegisters) {
        TimeReadHoldingRegisters();
    } else if (oTimer == _timeReadInputRegisters) {
        TimeReadInputRegisters();
    }
}

void ModbusProtocol::TimeReadCoilStatus()
{
    MonsbAddressWorkerTable* oTable = (_readCoilStatusIsYx) ? _yxRecTable : _ykRecTable;
//    GFunc_OutDebug(std::string("D20120329_1: TimeReadCoilStatus 1 _readCoilStatusIsYx = %1 ; Table Name = %2").arg(_readCoilStatusIsYx).arg(oTable->tableName()));
    if (_readCoilStatusIndex > oTable->stationIds().size() - 1) {
        _readCoilStatusIndex = 0;
    }

//    GFunc_OutDebug(std::string("D20120329_1: TimeReadCoilStatus 2 [%1 - %2]").arg(_readCoilStatusIndex).arg(oTable->stationIds().count()));
    if (_readCoilStatusIndex < oTable->stationIds().size()) {
        int iSlaveAddress = oTable->stationIds().at(_readCoilStatusIndex);
        int iMinAddress = CxContainer::value( oTable->stationIdMins(), iSlaveAddress, 0);
        int iMaxAddress = CxContainer::value( oTable->stationIdMaxes(), iSlaveAddress, 0);
        int iCount = iMaxAddress - iMinAddress + 1;
        if (iCount > 2000) return;
        ReadCoilStatus(iSlaveAddress, iMinAddress, iCount);
//        GFunc_OutDebug("D20120329_1: TimeReadCoilStatus 3");
        _readCoilStatusIndex ++;
    }
}

void ModbusProtocol::TimeReadInputStatus()
{
    MonsbAddressWorkerTable* oTable = (_readCoilStatusIsYx) ? _yxRecTable : _ykRecTable;
    if (_readCoilStatusIndex > oTable->stationIds().size() - 1) {
        _readCoilStatusIndex = 0;
    }

    if (_readCoilStatusIndex < oTable->stationIds().size()) {
        int iSlaveAddress = oTable->stationIds().at(_readCoilStatusIndex);
        int iMinAddress = CxContainer::value( oTable->stationIdMins(), iSlaveAddress, 0);
        int iMaxAddress = CxContainer::value( oTable->stationIdMaxes(), iSlaveAddress, 0);
        int iCount = iMaxAddress - iMinAddress + 1;
        if (iCount > 2000) return;
//        ReadCoilStatus(iSlaveAddress, iMinAddress, iCount);
        ReadInputStatus(iSlaveAddress, iMinAddress, iCount);
        _readCoilStatusIndex ++;
    }
}

void ModbusProtocol::TimeReadHoldingRegisters()
{
    YcaddAddressWorkerTable* oTable = _ycRecTable;
    if (_readHoldingRegistersIndex > oTable->stationIds().size() - 1) {
        _readHoldingRegistersIndex = 0;
    }

    if (_readHoldingRegistersIndex < oTable->stationIds().size()) {
        int iSlaveAddress = oTable->stationIds().at(_readHoldingRegistersIndex);
        int iMinAddress = CxContainer::value( oTable->stationIdMins(), iSlaveAddress, 0);
        int iMaxAddress = CxContainer::value( oTable->stationIdMaxes(), iSlaveAddress, 0);
        int iCount = iMaxAddress - iMinAddress + 1;
        if (iCount > 125) return;
        ReadHoldingRegisters(iSlaveAddress, iMinAddress, iCount);
        _readHoldingRegistersIndex ++;
    }
}

void ModbusProtocol::TimeReadInputRegisters()
{
    YcaddAddressWorkerTable* oTable = _ycRecTable;
    if (_readInputRegistersIndex > oTable->stationIds().size() - 1) {
        _readInputRegistersIndex = 0;
    }

    if (_readInputRegistersIndex < oTable->stationIds().size()) {
        int iSlaveAddress = oTable->stationIds().at(_readInputRegistersIndex);
        int iMinAddress = CxContainer::value( oTable->stationIdMaxes(), iSlaveAddress, 0);
        int iMaxAddress = CxContainer::value( oTable->stationIdMaxes(), iSlaveAddress, 0);
        int iCount = iMaxAddress - iMinAddress + 1;
        if (iCount > 125) return;
        ReadInputRegisters(iSlaveAddress, iMinAddress, iCount);
        _readInputRegistersIndex ++;
    }
}

void ModbusProtocol::protocolReceivedData(const uchar *pData, int iLength)
{
    for (int i = 0; i < iLength; ++i) {
        switch ( _recState )
        {
        case 0:
        {
            if ( std::find(_stationIdAll.begin(), _stationIdAll.end(), pData [ i ] ) != _stationIdAll.end() )
            {
                _recState = 1;
                _recSlaveAddress = pData [ i ];
                _recData[0] = _recSlaveAddress;
            }
        }
            break;
        case 1:
        {
            if ( pData [ i ] <= 24 ) {
                _recState = 2;
                _recFuncCode = pData [ i ];
                _recData[1] = _recFuncCode;
            } else {
                _recState = 0;
            }
        }
            break;
        case 2:
        {
            if ( (_recFuncCode == 1 && _readCoilStatusIsMaster) || (_recFuncCode == 2 && _readInputStatusIsMaster)
                 || (_recFuncCode == 3 && _readHoldingRegistersIsMaster) || (_recFuncCode == 4 && _readInputRegistersIsMaster) ) {
                ModbusReceiveModelDeal( pData [ i ] );
            } else if ( _recFuncCode <= 4) {
                ModbusReceiveModelResponse( pData [ i ] );
            } else  if ( (_recFuncCode == 5 && _forceSingleCoilIsMaster) || (_recFuncCode == 6 && _presetSingleRegisterIsMaster )
                 || (_recFuncCode == 15 && _forceMultipleCoilsIsMaster) || (_recFuncCode == 16 && _presetMultipleRegistersIsMaster) ) {
                ModbusReceiveModelReply( pData [ i ] );
//                GFunc_OutDebug(std::string("_recFuncCode : %1 ").arg(_recFuncCode));
//                GFunc_OutDebug(CxString::toHexstring(pData, iLength));
//                qDebug() << "16";
//                _recState = 0;
            } else if ( _recFuncCode == 5 || _recFuncCode == 6 || _recFuncCode == 15 || _recFuncCode == 16) {
                ModbusReceiveModelForce( pData [ i ] );
            } else {
                _recState = 0;
            }
        }
            break;
        } // end case
    }
}

//IsMaster : 1 , 2 , 3, 4
//Receive : yx , yc , yk
void ModbusProtocol::ModbusReceiveModelDeal(uchar data)
{
    switch (_recModelState)
    {
    case 0:
    {
        if (data > 0) {
            _recDealData.resize(data);

            _recModelState = 1;
            _recDealDataIndex = 0;
            _recData[2] = data;
        } else {
            _recModelState = 2;
        }
    }
        break;
    case 1:
    {
        _recDealData[_recDealDataIndex] = data;
        _recData[3 + _recDealDataIndex] = data;
        _recDealDataIndex ++;
        if (_recDealDataIndex >= _recDealData.size()) {
            _recModelState = 2;

        }
    }
        break;
    case 2:
    {
        _recCrc = data << 8;
        _recModelState = 3;
    }
        break;
    case 3:
    {
        _recCrc += data;
        _recModelState = 0;
        _recState = 0;
        ushort uCrc;
        if(_recDealDataIndex > 0)
        {
            uCrc = CxCrc::calcModbus(&_recData[0], 3 + _recDealDataIndex);

        }else {
            uCrc = CxCrc::calcModbus(&_recData[0], 3);
        }
        if(_recCrc == uCrc)
        {
            cxDebug() << protocolName() << " ModbusReceiveModel_Deal : FunctionCode " << _recFuncCode;
            switch (_recFuncCode)
            {
            case 1:
            {
                DealReadCoilStatus();
            }
                break;
            case 2:
            {
                DealReadInputStatus();
            }
                break;
            case 3:
            {
                DealReadHoldingRegisters();
            }
                break;
            case 4:
            {
                DealReadInputRegisters();
            }
                break;
            }
        }
    }
        break;
    default:
    {
        _recModelState = 0;
        _recState = 0;
    }
    }
}

//IsSlave : 1 , 2 , 3 , 4
//Response : yx , yc , yk
void ModbusProtocol::ModbusReceiveModelResponse(uchar data)
{
    switch (_recModelState)
    {
    case 0:
    {
        _recResponseAddress = data << 8;
        _recModelState = 1;
    }
        break;
    case 1:
    {
        _recResponseAddress += data;
        _recModelState = 2;
    }
        break;
    case 2:
    {
        _recResponseQuantity = data << 8;
        _recModelState = 3;
    }
        break;
    case 3:
    {
        _recResponseQuantity += data;
        _recModelState = 4;
    }
        break;
    case 4:
    {
        _recCrc = data << 8;
        _recModelState = 5;
    }
        break;
    case 5:
    {
        _recCrc += data;
        _recModelState = 0;
        _recState = 0;

        cxDebug() << protocolName() << " - ModbusReceiveModel_Response : FunctionCode " << _recFuncCode;
        switch (_recFuncCode)
        {
        case 1:
        {
            ResponseReadCoilStatus();
        }
            break;
        case 2:
        {
            ResponseReadInputStatus();
        }
            break;
        case 3:
        {
            ResponseReadHoldingRegisters();
        }
            break;
        case 4:
        {
            ResponseReadInputRegisters();
        }
            break;
        }

    }
        break;
    default:
    {
        _recModelState = 0;
        _recState = 0;
    }
    }
}

//IsSlave : 1 , 2 , 3 , 4
//Response : yx , yc , yk
void ModbusProtocol::ModbusReceiveModelReply(uchar data)
{
    switch (_recModelState)
    {
    case 0:
    {
        _recResponseAddress = data << 8;
        _recModelState = 1;
    }
        break;
    case 1:
    {
        _recResponseAddress += data;
        _recModelState = 2;
    }
        break;
    case 2:
    {
        _recResponseQuantity = data << 8;
        _recModelState = 3;
    }
        break;
    case 3:
    {
        _recResponseQuantity += data;
        _recModelState = 4;
    }
        break;
    case 4:
    {
        _recCrc = data << 8;
        _recModelState = 5;
    }
        break;
    case 5:
    {
        _recCrc += data;
        _recModelState = 0;
        _recState = 0;
    }
        break;
    default:
    {
        _recModelState = 0;
        _recState = 0;
    }
    }
}

//IsSlave : 5 , 6 , 15 , 16
//Receive : yx , yk , yc
void ModbusProtocol::ModbusReceiveModelForce(uchar data)
{
    //receive yx / yk
    if (_recFuncCode == 5 || _recFuncCode == 6) {
        switch (_recModelState)
        {
        case 0:
        {
            _recForceAddress = data << 8;
            _recModelState = 1;
        }
            break;
        case 1:
        {
            _recForceAddress += data;
            _recModelState = 2;
        }
            break;
        case 2:
        {
            _recForceSingleData = data << 8;
            _recModelState = 3;
        }
            break;
        case 3:
        {
            _recForceSingleData += data;
            _recModelState = 4;
        }
            break;
        case 4:
        {
            _recCrc = data << 8;
            _recModelState = 5;
        }
            break;
        case 5:
        {
            _recCrc += data;
            _recModelState = 0;
            _recState = 0;

            cxDebug() << protocolName() << " - ModbusReceiveModel_Force : FunctionCode " << _recFuncCode;
            switch (_recFuncCode)
            {
            case 5:
            {
                ResponseForceSingleCoil();
            }
                break;
            case 6:
            {
                ResponsePresetSingleRegister();
            }
                break;
            }

        }
            break;
        default:
        {
            _recModelState = 0;
            _recState = 0;
        }
        }
    } else {
        switch (_recModelState)
        {
        case 0:
        {
            _recForceAddress = data << 8;
            _recModelState = 1;
        }
            break;
        case 1:
        {
            _recForceAddress += data;
            _recModelState = 2;
        }
            break;
        case 2:
        {
            _recForceMultipleQuantity = data << 8;
            _recModelState = 3;
        }
            break;
        case 3:
        {
            _recForceMultipleQuantity += data;
            _recModelState = 4;
        }
            break;
        case 4:
        {
            if (data > 0) {
                _recForceMultipleData.resize( data );
                _recForceMultipleDataIndex = 0;
                _recModelState = 5;
            } else {
                _recModelState = 6;
            }
        }
            break;
        case 5:
        {
            _recForceMultipleData[ _recForceMultipleDataIndex ] = data;
            _recForceMultipleDataIndex ++;
            if (_recForceMultipleDataIndex >= _recForceMultipleData.size()) {
                _recModelState = 6;
            }
        }
            break;
        case 7:
        {
            _recCrc = data << 8;
            _recModelState = 8;
        }
            break;
        case 8:
        {
            _recCrc += data;
            _recModelState = 0;
            _recState = 0;

            cxDebug() << protocolName() << " - ModbusReceiveModel_Force : FunctionCode " << _recFuncCode;
            switch (_recFuncCode)
            {
            case 5:
            {
                ResponseForceMultipleCoils();
            }
                break;
            case 6:
            {
                ResponsePresetMultipleRegisters();
            }
                break;
            }

        }
            break;
        default:
        {
            _recModelState = 0;
            _recState = 0;
        }
        }
    }
}

void ModbusProtocol::protocolAfterStart()
{
    //init variable
    _recState = 0;
    _recSlaveAddress = 0;
    _recFuncCode = 0;
    _recModelState = 0;
    _recDealDataIndex = 0;
    _recDealData.clear();
    _recResponseAddress = 0;
    _recResponseQuantity = 0;
    _recForceAddress= 0;
    _recForceSingleData = 0;
    _recForceMultipleQuantity = 0;
    _recForceMultipleDataIndex = 0;
    _recForceMultipleData.clear();
    _recCrc = 0;

    _readCoilStatusSlaveAddress = -1;
    _readCoilStatusAddress = -1;
    _readCoilStatusCount = 0;

    _readInputStatusSlaveAddress = -1;
    _readInputStatusAddress = -1;
    _readInputStatusCount = 0;

    _readHoldingRegistersSlaveAddress = -1;
    _readHoldingRegistersAddress = -1;
    _readHoldingRegistersCount = 0;

    _readInputRegistersSlaveAddress = -1;
    _readInputRegistersAddress = -1;
    _readInputRegistersCount = 0;


    //init stationIds
    std::vector<int> stationIdAll;
    stationIdAll.insert(stationIdAll.end(), _yxRecTable->stationIds().begin(), _yxRecTable->stationIds().end());
    stationIdAll.insert(stationIdAll.end(), _ykRecTable->stationIds().begin(), _ykRecTable->stationIds().end());
    stationIdAll.insert(stationIdAll.end(), _ycRecTable->stationIds().begin(), _ycRecTable->stationIds().end());
    _stationIdAll.clear();
    for (size_t i = 0; i < stationIdAll.size(); ++i)
    {
        int iStationId = stationIdAll.at(i);
        if (std::find(_stationIdAll.begin(), _stationIdAll.end(), iStationId) == _stationIdAll.end())
        {
            _stationIdAll.push_back(iStationId);
        }
    }

    //GFunc_OutDebug(std::string("D20120329_1: refreshStatisticses _yxRecTable->stationIds().size() = " + CxString::toString(_yxRecTable->stationIds().size())));

    //init timer
    _timeReadCoilStatus->setInterval(_readCoilStatusInterval);
    _timeReadInputStatus->setInterval(_readInputStatusInterval);
    _timeReadHoldingRegisters->setInterval(_readHoldingRegistersInterval);
    _timeReadInputRegisters->setInterval(_readInputRegistersInterval);
    _timeReadCoilStatus->start();
    _timeReadInputStatus->start();
    _timeReadHoldingRegisters->start();
    _timeReadInputRegisters->start();
}

void ModbusProtocol::protocolAfterStop()
{
    _timeReadCoilStatus->stop();
    _timeReadInputStatus->stop();
    _timeReadHoldingRegisters->stop();
    _timeReadInputRegisters->stop();
}

void ModbusProtocol::monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag)
{
    if (monsbOldValues.size() > 0 && eMonsbType == MonsbType_rtu_state && ( _forceSingleCoilIsYx || _forceMultipleCoilsIsYx )) {
        if (_forceMultipleCoilsIsYx) {
            int iStationId;
            std::map<int, int> addressValues = _yxSendTable->addressValues(monsbOldValues, true, &iStationId);
            if (addressValues.size() <= 0) return;
            int iMinAddress;
            int iMaxAddress;
            if (iStationId > -1 && CxBuffer::isContinueAddress(addressValues, &iMinAddress, &iMaxAddress)) {
                std::vector<uchar> data = CxBuffer::toBuffer(addressValues, iMinAddress);
                SendModbusForceData(iStationId, 15, iMinAddress, addressValues.size(), data);
                return;
            }
        }
        if (_forceSingleCoilIsYx) {
            std::map<const MonsbMeasure * , int>::const_iterator it = monsbOldValues.begin();
            while (it != monsbOldValues.end()) {
                const MonsbMeasure* oMonsb = it->first;
                MonsbAddressWorker* oWorker = _yxSendTable->workerByMeasure(oMonsb);
                if (oWorker) {
                    int iStationId = oWorker->stationId();
                    int iAddress = oWorker->address();
                    int iValue = oMonsb->value();
                    ForceSingleCoil(iStationId, iAddress, (iValue == CIRtuState_He));
                }
                it ++;
            }
        }
    }
}

void ModbusProtocol::ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag)
{
    if (ycaddOldValues.size() > 0 && eYcaddType == YcaddType_rtu_value) {
        int iStationId;
        std::map<int, double> addressValues = _ycSendTable->addressValues(ycaddOldValues, true, &iStationId);
        if (addressValues.size() <= 0) return;
        int iMinAddress;
        int iMaxAddress;
        if (iStationId > -1 && CxBuffer::isContinueAddress(addressValues, &iMinAddress, &iMaxAddress))
        {
            std::vector<uchar> data = CxBuffer::toBuffer(addressValues, iMinAddress);
            SendModbusForceData(iStationId, 16, iMinAddress, addressValues.size(), data);
            return;
        }
        std::map<const YcaddMeasure *, double>::const_iterator it = ycaddOldValues.begin();
        while (it != ycaddOldValues.end()) {
            const YcaddMeasure* oYcadd = it->first;
            YcaddAddressWorker* oWorker = _ycSendTable->workerByMeasure(oYcadd);
            if (oWorker) {
                iStationId = oWorker->stationId();
                int iAddress = oWorker->address();
                ushort wValue = oWorker->getMeasureValue();
                PresetSingleRegister(iStationId, iAddress, wValue);
            }
            it ++;
        }
    }
}

void ModbusProtocol::DealReadCoilStatus()
{
    DealReadStatus(_readCoilStatusIsYx, _readCoilStatusSlaveAddress, _readCoilStatusAddress, _readCoilStatusCount);
}

void ModbusProtocol::DealReadInputStatus()
{
    DealReadStatus(_readInputStatusIsYx, _readInputStatusSlaveAddress, _readInputStatusAddress, _readInputStatusCount);
}

void ModbusProtocol::DealReadStatus(bool bIsYx, int iReadSlaveAddress, int iReadAddress, int iReadCount)
{
    if (_recSlaveAddress != iReadSlaveAddress) return;
    int iAddress = iReadAddress;
    std::map<int, int> addressValues;
    for (size_t i = 0; i < _recDealData.size(); ++i) {
        uchar data = _recDealData.at(i);
        for (int j = 0; j < 8; ++j){
            if (iAddress > iReadAddress + iReadCount) break;
            int iValue = ( ( data >> j ) & 0x01 );
            addressValues[iAddress] = iValue;
            iAddress ++;
        }
    }
    if (bIsYx)
    {
        _yxRecTable->receiveAddressValues(iReadSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
    else
    {
        //yk reserve
//        std::map<int, int>::const_iterator it = addressValues.begin();
//        while (it != addressValues.end()) {
//            int iAddress = it->first;
//            int iValue = it->second;
//            if (iValue > 0) {
                //extend
//            }
//            it ++;
//        }
    }
}

void ModbusProtocol::DealReadHoldingRegisters()
{
    DealReadRegisters(_readHoldingRegistersSlaveAddress, _readHoldingRegistersAddress, _readHoldingRegistersCount);
}

void ModbusProtocol::DealReadInputRegisters()
{
    DealReadRegisters(_readHoldingRegistersSlaveAddress, _readHoldingRegistersAddress, _readHoldingRegistersCount);
}

void ModbusProtocol::DealReadRegisters(int iReadSlaveAddress, int iReadAddress, int iReadCount)
{
    if (_recSlaveAddress != iReadSlaveAddress) return;
    int iAddress = iReadAddress;
    short nValue = 0;
    double dValue = 0;
    std::map<int, double> addressValues;
    for (size_t i = 0; i < _recDealData.size(); ++i) {
        uchar data = _recDealData.at(i);
        if (i % 2) {
            nValue |= data;
            dValue = nValue;
            addressValues[iAddress] = dValue;
            iAddress ++;
        } else {
            nValue = data << 8;
        }
    }
    _ycRecTable->receiveAddressValues(iReadSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
}

void ModbusProtocol::ResponseReadCoilStatus()
{
    ResponseReadStatus(_readCoilStatusIsYx, 1);
}

void ModbusProtocol::ResponseReadInputStatus()
{
    ResponseReadStatus(_readInputStatusIsYx, 2);
}

void ModbusProtocol::ResponseReadStatus(bool bIsYx, uchar byFuncCode)
{
    if (bIsYx) {
        std::vector<uchar> data;
        data.resize(CxBuffer::divBig(_recResponseQuantity, 8));
        const std::vector<MonsbAddressWorker*>* oWorkers = _yxSendTable->workers();
        for (size_t i = 0; i < oWorkers->size(); ++i) {
            MonsbAddressWorker* oWorker = oWorkers->at(i);
            if (! oWorker->measure() || oWorker->measure()->value() != 1) continue;
            int iAddress = oWorker->address();
            if (oWorker->stationId() == _recSlaveAddress && iAddress >= _recResponseAddress && iAddress < _recResponseAddress + _recResponseQuantity) {
                uchar indexMod;
                int iIndex = CxBuffer::index(8, iAddress, &indexMod, _recResponseAddress);
                if (iIndex >= 0 && iIndex < data.size() && indexMod < 8)
                {
                    uchar value = data[ iIndex ];
                    CxBuffer::setBitTrue(&value, indexMod);
                    data[ iIndex ] = value;
                }
            }
        }
        SendModbusReadData(_recSlaveAddress, byFuncCode, data);
    } else {
        std::vector<uchar> data;
        data.resize(CxBuffer::divBig(_recResponseQuantity, 8));
        const std::vector<MonsbAddressWorker*>* oWorkers = _ykSendTable->workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            MonsbAddressWorker* oWorker = oWorkers->at(i);
            const MonsbMeasure* oMonsb = oWorker->measure();
            if (! oMonsb) continue;
            int iAddress = oWorker->address();
            if (oWorker->stationId() == _recSlaveAddress && iAddress >= _recResponseAddress && iAddress < _recResponseAddress + _recResponseQuantity)
            {
                MonsbControlTask* oTask = MonsbControlTaskManager::firstTask(oMonsb);
                if (! oTask) continue;
                uchar indexMod;
                int iIndex = CxBuffer::index(8, iAddress, &indexMod, _recResponseAddress);
                if (iIndex >= 0 && iIndex < data.size() && indexMod < 8)
                {
                    uchar value = data[ iIndex ];
                    CxBuffer::setBitTrue(&value, indexMod);
                    data[ iIndex ] = value;
                }
            }
        }
        SendModbusReadData(_recSlaveAddress, byFuncCode, data);
    }
}

void ModbusProtocol::ResponseReadHoldingRegisters()
{
    ResponseReadRegisters(3);
}

void ModbusProtocol::ResponseReadInputRegisters()
{
    ResponseReadRegisters(4);
}

void ModbusProtocol::ResponseReadRegisters(uchar byFuncCode)
{
    std::vector<uchar> data;
    data.resize(_recResponseQuantity * 2);
    const std::vector<YcaddAddressWorker*>* oWorkers = _ycSendTable->workers();
    for (size_t i = 0; i < oWorkers->size(); ++i) {
        YcaddAddressWorker* oWorker = oWorkers->at(i);
        const YcaddMeasure* oYcadd = oWorker->measure();
        if (! oYcadd) continue;
        int iAddress = oWorker->address();
        if (oWorker->stationId() == _recSlaveAddress && iAddress >= _recResponseAddress && iAddress < _recResponseAddress + _recResponseQuantity) {
            int iIndex = iAddress - _recResponseAddress;
            ushort w = oYcadd->value() * oWorker->bianbi();
            data[ iIndex ] = w >> 8;
            data[ iIndex + 1 ] = w;
        }
    }
    SendModbusReadData(_recSlaveAddress, byFuncCode, data);
}

void ModbusProtocol::ResponseForceSingleCoil()
{
    int iSlaveAddress = _recSlaveAddress;
    int iAddress = _recForceAddress;
    int iValue = ( _recForceSingleData > 0 ) ? 1 : 0;
    std::map<int, int> addressValues;
    addressValues[ iAddress ] = iValue;
    if ( _forceSingleCoilIsYx ) {
        _yxRecTable->receiveAddressValues(iSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    } else {
        //yk reserve
    }
}

void ModbusProtocol::ResponsePresetSingleRegister()
{
    int iSlaveAddress = _recSlaveAddress;
    int iAddress = _recForceAddress;
    double dValue = _recForceSingleData;
    std::map<int, double> addressValues;
    addressValues[ iAddress ] = dValue;
    _ycRecTable->receiveAddressValues(iSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
}

void ModbusProtocol::ResponseForceMultipleCoils()
{
    int iSlaveAddress = _recSlaveAddress;
    int iAddress = _recForceAddress;
    std::map<int, int> addressValues;
    for (size_t i = 0; i < _recForceMultipleData.size()-1; ++i) {
        uchar data = _recForceMultipleData.at(i);
        for (int j = 0; j < 8; ++j){
            if (iAddress > _recForceAddress + _recForceMultipleQuantity) break;
            int iValue = ( ( data >> j ) & 0x01 );
            addressValues[iAddress] = iValue;
            iAddress ++;
        }
    }
    if (_forceMultipleCoilsIsYx) {
        _yxRecTable->receiveAddressValues(iSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    } else {
        //yk reserve
    }
}

void ModbusProtocol::ResponsePresetMultipleRegisters()
{
    int iSlaveAddress = _recSlaveAddress;
    int iAddress = _recForceAddress;
    double dValue = 0;
    std::map<int, double> addressValues;
    for (size_t i = 0; i < _recDealData.size(); ++i) {
        uchar data = _recDealData.at(i);
        if (i % 2) {
            dValue += data;
            addressValues[iAddress] = dValue;
            iAddress ++;
        } else {
            dValue = data << 8;
        }
    }
    _ycRecTable->receiveAddressValues(iSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
}

int ModbusProtocol::SendModbusReadData(uchar iSlaveAddress, uchar byFuncCode, ushort wAddress, ushort wQuantity)
{
    uchar buffer[8];
    buffer[0] = iSlaveAddress;
    buffer[1] = byFuncCode;
    buffer[2] = wAddress >> 8;
    buffer[3] = wAddress;
    buffer[4] = wQuantity >> 8;
    buffer[5] = wQuantity;
    ushort wCrc = CxCrc::calcModbus(buffer, 6);
    buffer[6] = wCrc >> 8;
    buffer[7] = wCrc;
    return writeData(buffer, 8);
}

int ModbusProtocol::SendModbusReadData(uchar iSlaveAddress, uchar byFuncCode, const std::vector<uchar> &data)
{
    int iSize = data.size();
    if (iSize > 255)
        return -1;
    int iLength = 5 + iSize;
    std::vector<uchar> buffer(iLength);
    buffer[0] = iSlaveAddress;
    buffer[1] = byFuncCode;
    buffer[2] = iSize;
    memcpy(&buffer[3], data.data(), iSize);
    ushort wCrc = CxCrc::calcModbus(buffer.data(), iLength - 2);
    buffer[3 + iSize] = wCrc >> 8;
    buffer[4 + iSize] = wCrc;
    int iResult = writeData(buffer.data(), iLength);
    return iResult;
}

int ModbusProtocol::SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort wAddress, ushort data)
{
    uchar buffer[8];
    buffer[0] = iSlaveAddress;
    buffer[1] = byFuncCode;
    buffer[2] = wAddress >> 8;
    buffer[3] = wAddress;
    buffer[4] = data >> 8;
    buffer[5] = data;
    ushort wCrc = CxCrc::calcModbus(buffer, 6);
    buffer[6] = wCrc >> 8;
    buffer[7] = wCrc;
    return writeData(buffer, 8);
}

int ModbusProtocol::SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort wAddress, ushort wQuantity, const std::vector<uchar> &data)
{
    int iSize = data.size();
    if (iSize > 255)
        return -1;
    int iLength = 9 + iSize;
    std::vector<uchar> buffer(iLength);
    buffer[0] = iSlaveAddress;
    buffer[1] = byFuncCode;
    buffer[2] = wAddress >> 8;
    buffer[3] = wAddress;
    buffer[4] = wQuantity >> 8;
    buffer[5] = wQuantity;
    buffer[6] = iSize;
    memcpy(&buffer[7], data.data(), iSize);
    ushort wCrc = CxCrc::calcModbus(buffer.data(), iLength - 2);
    buffer[7 + iSize] = wCrc >> 8;
    buffer[8 + iSize] = wCrc;
    int iResult = writeData(buffer.data(), iLength);
    return iResult;
}



///Modbus Master*********
ModbusMasterProtocol::ModbusMasterProtocol()
{
    _readCoilStatusIsMaster          =true;
    _readInputStatusIsMaster         =true;
    _readHoldingRegistersIsMaster    =true;
    _readInputRegistersIsMaster      =true;
    _forceSingleCoilIsMaster         =true;
    _presetSingleRegisterIsMaster    =true;
    _forceMultipleCoilsIsMaster      =true;
    _presetMultipleRegistersIsMaster =true;
}

ModbusMasterProtocol::~ModbusMasterProtocol()
{
}

void ModbusMasterProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ModbusProtocol::describeSelf(oDescribeUnit);

    CxDescribeInteger* oReadCoilStatusIntervalDescribe        = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryModbusReadCoilStatusInterval);          oReadCoilStatusIntervalDescribe->initValue(_readCoilStatusInterval);
    CxDescribeInteger* oReadInputStatusIntervalDescribe       = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryModbusReadInputStatusInterval);         oReadInputStatusIntervalDescribe->initValue(_readInputStatusInterval);
    CxDescribeInteger* oReadHoldingRegistersIntervalDescribe  = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryModbusReadHoldingRegistersInterval);    oReadHoldingRegistersIntervalDescribe->initValue(_readHoldingRegistersInterval);
    CxDescribeInteger* oReadInputRegistersIntervalDescribe    = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryModbusReadInputRegistersInterval);      oReadInputRegistersIntervalDescribe->initValue(_readInputRegistersInterval);
}

///Modbus Master---------



///Modbus Slave*********
ModbusSlaveProtocol::ModbusSlaveProtocol()
{
    _readCoilStatusIsMaster          =false;
    _readInputStatusIsMaster         =false;
    _readHoldingRegistersIsMaster    =false;
    _readInputRegistersIsMaster      =false;
    _forceSingleCoilIsMaster         =false;
    _presetSingleRegisterIsMaster    =false;
    _forceMultipleCoilsIsMaster      =false;
    _presetMultipleRegistersIsMaster =false;
}

ModbusSlaveProtocol::~ModbusSlaveProtocol()
{
}
///Modbus Slave---------
