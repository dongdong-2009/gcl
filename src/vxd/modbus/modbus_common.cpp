#include "modbus_common.h"
#include "modbus_global.h"
#include <script/gcl_script_vxd.h>

using namespace std;

ModBus_Common_Protocol::ModBus_Common_Protocol()
{
    _timeReadCoilStatus       = new CxTimer();
    _timeReadInputStatus      = new CxTimer();
    _timeReadHoldingRegisters = new CxTimer();
    _timeReadInputRegisters   = new CxTimer();

    _timeSendBlock = new CxTimer();

    _timeReadCoilStatus      ->setTimerTimeOut(this);
    _timeReadInputStatus     ->setTimerTimeOut(this);
    _timeReadHoldingRegisters->setTimerTimeOut(this);
    _timeReadInputRegisters  ->setTimerTimeOut(this);
    _timeSendBlock           ->setTimerTimeOut(this);

    //读取 线圈
    _readCoilStatusInterval = 0;
    _typeCoil = 0;
    _modeCoil = 0;
    _szBuffersCoil = "";

    //*
    _readInputStatusInterval = 0;
    _typeInputStatus = 0;
    _modeInputStatus = 0;
    _szBuffersInputStatus = "";

    //*
    _readHoldingRegistersInterval = 0;
    _typeHodingRegister = 0;
    _modeHodingRegister = 0;
    _szBuffersHodingRegister = "";

    //*
    _readInputRegistersInterval = 0;
    _typeInputRegister = 0;
    _modeInputRegister = 0;
    _szBuffersInputRegister = "";

    _dataMode = 0;
    _addrType = 0;
    _addrLimit = 125;

    _sessionFlag = 0;
    _sendEnable = false;
    _tmLast = CxTime::currentMsepoch();

    _ykFunctionCode = 6;
    //     CxChannelManager::setPacketInterinfoOut(true);
}

ModBus_Common_Protocol::~ModBus_Common_Protocol()
{
    delete _timeReadCoilStatus      ;
    delete _timeReadInputStatus     ;
    delete _timeReadHoldingRegisters;
    delete _timeReadInputRegisters  ;
    delete _timeSendBlock;
}

void ModBus_Common_Protocol::toContext(CxISetSkv& context) const
{
    ProtocolCommon::toContext(context);

    context.setValue(CS_EntryModbusReadCoilStatusInterval,       _readCoilStatusInterval);
    context.setValue(CS_EntryModbusReadInputStatusInterval,      _readInputStatusInterval);
    context.setValue(CS_EntryModbusReadHoldingRegistersInterval, _readHoldingRegistersInterval);
    context.setValue(CS_EntryModbusReadInputRegistersInterval,   _readInputRegistersInterval);

    context.setValue(CS_EntryModbusHoldingRegistersType,_typeInputRegister);
    context.setValue(CS_EntryModbusAddrType ,_addrType);
    context.setValue(CS_EntryModbusAddrLimit ,_addrLimit);
}

void ModBus_Common_Protocol::fromContext(const CxIGetSkv& context)
{
    ProtocolCommon::fromContext(context);

    _readCoilStatusInterval         = context.getValue(CS_EntryModbusReadCoilStatusInterval ,        _readCoilStatusInterval);
    _readInputStatusInterval        = context.getValue(CS_EntryModbusReadInputStatusInterval ,       _readInputStatusInterval);
    _readHoldingRegistersInterval   = context.getValue(CS_EntryModbusReadHoldingRegistersInterval ,  _readHoldingRegistersInterval);
    _readInputRegistersInterval     = context.getValue(CS_EntryModbusReadInputRegistersInterval ,    _readInputRegistersInterval);

    string split = ",";

    _typeCoil             = context.getValue(CS_EntryModbusCoilStatusType,_typeCoil);
    _modeCoil             = context.getValue(CS_EntryModbusCoilStatusCmdMode,_modeCoil);
    _szBuffersCoil        = context.getValue(CS_EntryModbusCoilStatusCmd,std::string());
    CxString::splitCase(_cmdBuffersCoil,_szBuffersCoil,split);

    _typeInputStatus             = context.getValue(CS_EntryModbusInputStatusType,_typeInputStatus);
    _modeInputStatus             = context.getValue(CS_EntryModbusInputStatusCmdMode,_modeInputStatus);
    _szBuffersInputStatus        = context.getValue(CS_EntryModbusInputStatusCmd,std::string());
    CxString::splitCase(_cmdBuffersInputStatus,_szBuffersInputStatus,split);

    _typeHodingRegister              = context.getValue(CS_EntryModbusHoldingRegistersType,_typeHodingRegister);
    _modeHodingRegister             = context.getValue(CS_EntryModbusHoldingRegistersCmdMode,_modeHodingRegister);
    _szBuffersHodingRegister        = context.getValue(CS_EntryModbusHoldingRegistersCmd,std::string());
    CxString::splitCase(_cmdBuffersHodingRegister,_szBuffersHodingRegister,split);

    _typeInputRegister              = context.getValue(CS_EntryModbusInputRegistersType,_typeInputRegister);
    _modeInputRegister              = context.getValue(CS_EntryModbusInputRegistersInCmdMode,_modeInputRegister);
    _szBuffersInputRegister        = context.getValue(CS_EntryModbusInputRegistersCmd,std::string());
    CxString::splitCase(_cmdBuffersInputRegister,_szBuffersInputRegister,split);

    _addrType                       = context.getValue(CS_EntryModbusAddrType ,_addrType);
    _addrLimit                       = context.getValue(CS_EntryModbusAddrLimit ,_addrLimit);

    _ykFunctionCode              = context.getValue(CS_EntryModbusInputRegistersType,_ykFunctionCode);
}

int ModBus_Common_Protocol::sendControl(const MonsbMeasure *oMonsb, int fromValue, int toValue, const SendControlArgvs & argvs)
{
    if (! oMonsb)
        return SendMeasureFail_Nomeasure;
    MonsbAddressWorker* oWorker = _ykSendTable->workerByMeasure(oMonsb);
    if (! oWorker)
        return SendMeasureFail_Nomeasure;
    int iSlaveAddress = oWorker->stationId();
    int iAddress = oWorker->address();
    int iResult = 0;
    switch (_ykFunctionCode)
    {
    case 5:
        iResult = ForceSingleCoil(iSlaveAddress, iAddress, (toValue != 0));
        break;
    case 6:
        iResult = PresetSingleRegister(iSlaveAddress, iAddress, toValue);
        break;
    case 15:
    {
        std::vector<bool> values;
        values.push_back((toValue != 0));
        iResult = ForceMultipleCoils(iSlaveAddress, iAddress, values);
    }
        break;
    case 16:
    {
        std::vector<ushort> values;
        values.push_back(toValue);
        iResult = PresetMultipleRegisters(iSlaveAddress, iAddress, values);
    }
        break;
    default:
        break;
    }
    return iResult;
}

bool ModBus_Common_Protocol::provideSendControl(int iMeasureSonType, const MonsbMeasure *oMonsb)
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

void ModBus_Common_Protocol::protocolReceivedData(const uchar *pData, int iLength)
{
#ifndef  GMAC_TEST
    //收到数据
    receivedProtocolDataNow();
    //数据压榨
    _revBuffer.putIn(pData,(uint16)iLength);
    //数据处理
    dealData();

    //outProtocolPrompt()<<"dealData()"<<iLength<<cxEndLine;
#endif
}

void ModBus_Common_Protocol::doTask(GCCommon::GCDataElement dt)
{
    ProtocolCommon::doTask(dt);
    switch(dt.type())
    {
    case GCL_CMD_SYS_CLOCK://校时
        clock();
        break;
    case GCL_CMD_SYS_INFO://信息
        break;
    case GCL_CMD_SYS_SCENCE://信息
        break;
    default:
        break;
    }
}

void ModBus_Common_Protocol::clock()
{

}

void ModBus_Common_Protocol::dealData()
{
    uint16 wLen = 0;
    uchar *pData = _revBuffer.getBuf(wLen);
    int nStart = findHeader(pData,wLen);
    //未找到头
    if(nStart<0)
    {
        reset(512);
//        outProtocolDebug()<<"no find complete frame!" <<wLen<<cxEndLine;
        return;
    }
    //帧检查
    int nFrmLen = 0;
    //帧OK
    if(checkFrame(pData+nStart,wLen-nStart,nFrmLen))
    {
        _recFuncCode    = _asduRev.fc();
        _recSlaveAddress = _asduRev.addr();

        _recDealData.clear();
        int len = _asduRev.len();
        uchar* pBuf = _asduRev.buf();
        //no 长度
        if((len>1)&&pBuf!=NULL)
        {
            for(int i=1;i<len;++i)//第一个字节为长度
            {
                _recDealData.push_back(pBuf[i]);
            }
        }
        //主站,读响应
        if ( (_recFuncCode == 1 && _readCoilStatusIsMaster) || (_recFuncCode == 2 && _readInputStatusIsMaster)
             || (_recFuncCode == 3 && _readHoldingRegistersIsMaster) || (_recFuncCode == 4 && _readInputRegistersIsMaster) )
        {
            ModbusReceiveModelDeal();
        }
        //从站,读请求
        else if ( _recFuncCode <= 4)
        {
            ModbusReceiveModelReply();
            ModbusReceiveModelResponse(); //Read
        }
        //主站,写响应
        else  if ( (_recFuncCode == 5 && _forceSingleCoilIsMaster) || (_recFuncCode == 6 && _presetSingleRegisterIsMaster )
                   || (_recFuncCode == 15 && _forceMultipleCoilsIsMaster) || (_recFuncCode == 16 && _presetMultipleRegistersIsMaster) )
        {
            ModbusReceiveModelReply();
        }
        //从站,写请求
        else if ( _recFuncCode == 5 || _recFuncCode == 6 || _recFuncCode == 15 || _recFuncCode == 16)
        {
            ModbusReceiveModelForce( ); //Write
        }
        else
        {
            _recState = 0;
        }
        _sendEnable = true;
        sendBuffer(); //多帧发送
    }
    else
    {
        outProtocolDebug()<<"check fail" <<wLen<< cxEndLine;
        //解决分帧问题
        reset();
    }
    //移除
    if((nStart+nFrmLen)>0)_revBuffer.removeLeft(nStart+nFrmLen);
    //多帧
    if((nStart+nFrmLen)<wLen)
    {
        dealData();
    }
}

void ModBus_Common_Protocol::callUp()
{

}

//发送命令数据
int ModBus_Common_Protocol::sendCustomData(int iSourceId, int iCommandId, std::map<string, string> sParams, int iDataLength, const char *pData)
{
    //#define GCL_CMD_SYS_VTL_W       (GCL_CMD_SYS_BASE+0x4A) //VLT写1  标准模式
    //#define GCL_CMD_SYS_VTL_W_16    (GCL_CMD_SYS_BASE+0x4B) //VLT写2  标准16进制模式
    //#define GCL_CMD_SYS_VTL_B       (GCL_CMD_SYS_BASE+0x4C) //VLT写3  字节模式
    //#define GCL_CMD_SYS_VTL_B_16    (GCL_CMD_SYS_BASE+0x4C) //VLT写3  字节模式16进制模式
    int res = 0;
    switch(iCommandId)
    {
    case GCL_CMD_SYS_VTL_W: //64
    case GCL_CMD_SYS_VTL_W_16://65
    case GCL_CMD_SYS_VTL_B://66
    case GCL_CMD_SYS_VTL_B_16://67
        res = sendVTLData(iCommandId,sParams,iDataLength,pData);
        break;
    default:
        break;
    }

    return res;
}

int ModBus_Common_Protocol::sendVTLData(int iCommandId, std::map<string, string> sParams, int iDataLength, const char *pData)
{
    string stationID; stationID = CxContainer::value(sParams,CS_Modbus_StationID,stationID);
    string beginAddr; beginAddr = CxContainer::value(sParams,CS_Modbus_BeginAddr,beginAddr);
    string fc; fc = CxContainer::value(sParams,CS_Modbus_FC,fc);
    string count; count = CxContainer::value(sParams,CS_Modbus_Count,count);

    if(stationID.length()<1 || beginAddr.length()<1 || fc.length()<1 || count.length()<1) return -1;

    byte nId = CxString::toInt32(stationID);
    byte nFc = CxString::toInt32(fc);
    uint16 nBeginAddr = CxString::toInt32(beginAddr);
    uint16 nCount = CxString::toInt32(count);

    //地址修正
    nBeginAddr = addrMod(nBeginAddr,true);

    vector<uchar> vBuf;
    vBuf.clear();
    switch(iCommandId)
    {
    case GCL_CMD_SYS_VTL_W://64
    {
        for(int i =0;i<iDataLength;i++)
        {
            vBuf.push_back(pData[i]);
        }
    }
        break;
    case GCL_CMD_SYS_VTL_W_16://65
    {
        string s(pData,iDataLength);
        vBuf = CxString::fromHexstring(string(pData,iDataLength));
    }
        break;
    case GCL_CMD_SYS_VTL_B://66
    {
        for(int i =0;i<iDataLength;i++)
        {
            vBuf.push_back(char(0));
            vBuf.push_back(pData[i]);
        }
    }
        break;
    case GCL_CMD_SYS_VTL_B_16://67
    {
        string s(pData,iDataLength);
        vector<uchar> v = CxString::fromHexstring(string(pData,iDataLength));
        for(size_t i =0;i<v.size();i++)
        {
            vBuf.push_back(char(0));
            vBuf.push_back(v.at(i));
        }
    }
        break;
    default:
        break;
    }

    uint16 pos = 0;
    int res = 0;

    switch(nFc)
    {
    case 3:
    case 4:
    {
        memset(_sendBuf,0,GCL_LEN_DATA_EM);
        _sendBuf[pos++] = nId;
        _sendBuf[pos++] = nFc;
        CxBuffer::toBuf(&_sendBuf[pos],nBeginAddr,0);
        pos+=sizeof(uint16);
        CxBuffer::toBuf(&_sendBuf[pos],nCount,0);
        pos+=sizeof(uint16);
        _asduSend.setValue(pos,_sendBuf);
        res = setUpFrame(false,false);
    }
        break;
    case 5://写单线圈
    case 6://写单寄存器
        if(vBuf.size()+4<GCL_LEN_DATA_EM)
        {
            memset(_sendBuf,0,GCL_LEN_DATA_EM);
            _sendBuf[pos++] = nId;
            _sendBuf[pos++] = nFc;
            CxBuffer::toBuf(&_sendBuf[pos],nBeginAddr,0);
            pos+=sizeof(uint16);

            memcpy(&_sendBuf[pos],vBuf.data(),vBuf.size());
            pos+=vBuf.size();

            _asduSend.setValue(pos,_sendBuf);
            res = setUpFrame(false,false);
        }
        break;
    case 15://写多线圈
    case 16://写多寄存器
        if(vBuf.size()+7<GCL_LEN_DATA_EM)
        {
            memset(_sendBuf,0,GCL_LEN_DATA_EM);
            _sendBuf[pos++] = nId;
            _sendBuf[pos++] = nFc;
            CxBuffer::toBuf(&_sendBuf[pos],nBeginAddr,0);
            pos+=sizeof(uint16);
            CxBuffer::toBuf(&_sendBuf[pos],nCount,0);
            pos+=sizeof(uint16);
            _sendBuf[pos++] = vBuf.size();
            memcpy(&_sendBuf[pos],vBuf.data(),vBuf.size());
            pos+=vBuf.size();
            _asduSend.setValue(pos,_sendBuf);
            res = setUpFrame(false,false);
        }
        break;
    default:
        break;
    }
    return res;
}

//IsMaster : 1 , 2 , 3, 4
//Receive : yx , yc , yk
void ModBus_Common_Protocol::ModbusReceiveModelDeal()
{

    switch (_asduRev.fc())
    {
    case 1:
        DealReadCoilStatus();
        break;
    case 2:
        DealReadInputStatus();
        break;
    case 3:
        DealReadHoldingRegisters();
        break;
    case 4:
        DealReadInputRegisters();
        break;
    default:
        break;
    }
}

//IsSlave : 1 , 2 , 3 , 4
//Response : yx , yc , yk
void ModBus_Common_Protocol::ModbusReceiveModelResponse()
{
    switch (_asduRev.fc())
    {
    case 1:
        ResponseReadCoilStatus();
        break;
    case 2:
        ResponseReadInputStatus();
        break;
    case 3: //保存寄存器
        ResponseReadHoldingRegisters();
        break;
    case 4://输入寄存器
        ResponseReadInputRegisters();
        break;
    default:
        break;
    }
}

//IsSlave : 1 , 2 , 3 , 4
//Response : yx , yc , yk
void ModBus_Common_Protocol::ModbusReceiveModelReply()
{

    if(_asduRev.len()<4)return;

    uchar *pBuf = _asduRev.buf();
    int pos = 0;
    CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_recResponseAddress,_dataMode);
    pos += sizeof(uint16);
    CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_recResponseQuantity,_dataMode);

    _recResponseAddress = addrMod(_recResponseAddress,false);
}

//IsSlave : 5 , 6 , 15 , 16
//Receive : yx , yk , yc
void ModBus_Common_Protocol::ModbusReceiveModelForce()
{
    uchar fc = _asduRev.fc();
    switch(fc)
    {
    case 5://yx
    case 6:
    {
        if(_asduRev.len()<4)break;

        uchar *pBuf = _asduRev.buf();
        int pos = 0;
        CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_recForceAddress,_dataMode);
        pos += sizeof(uint16);
        CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_recForceSingleData,_dataMode);
        pos += sizeof(uint16);

        //地址修正
        _recForceAddress = addrMod(_recForceAddress,false);

        if(fc==5)ResponseForceSingleCoil(); //单线圈
        else ResponsePresetSingleRegister(); //单寄存器
    }
        break;
    case 15:
    case 16:
    {
        if(_asduRev.len()<5)break;

        uchar *pBuf = _asduRev.buf();
        int pos = 0;
        CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_recForceAddress,_dataMode);
        pos += sizeof(uint16);
        CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_recForceMultipleQuantity,_dataMode);
        pos += sizeof(uint16);

        //地址修正
        _recForceAddress = addrMod(_recForceAddress,false);

        uchar count = pBuf[pos];
        pos++;

        if (count> 0)
        {
            if(_asduRev.len()<(pos+count))break;
            _recForceMultipleData.resize(count);

            for(int i=0;i<count;i++)
            {
                _recForceMultipleData[i]=pBuf[pos++];
            }

            if(fc==15)ResponseForceMultipleCoils();//多线圈
            else ResponsePresetMultipleRegisters();//多寄存器
        }
    }
        break;
    default:
        break;
    }

    //发送响应
    _asduSend = _asduRev;

    protolAfterSend();

    setUpFrame();
}

void ModBus_Common_Protocol::protocolAfterStart()
{
    ProtocolCommon::protocolAfterStart();
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

    //init stationIds
    _stationMinMaxMids.clear();
    initStationMinMaxMids(_yxRecTable);
    initStationMinMaxMids(_ycRecTable);
    initStationMinMaxMids(_ywRecTable);
    _readHoldingRegistersIt = _stationMinMaxMids.begin();

    //init timer
    if(_readCoilStatusInterval>0)
    {
        _timeReadCoilStatus->setInterval(_readCoilStatusInterval);
        _timeReadCoilStatus->start();
    }
    if(_readInputStatusInterval>0)
    {
        _timeReadInputStatus->setInterval(_readInputStatusInterval);
        _timeReadInputStatus->start();
    }
    if(_readHoldingRegistersInterval>0)
    {
        _timeReadHoldingRegisters->setInterval(_readHoldingRegistersInterval);
        _timeReadHoldingRegisters->start();
    }
    if(_readInputRegistersInterval>0)
    {
        _timeReadInputRegisters->setInterval(_readInputRegistersInterval);
        _timeReadInputRegisters->start();
    }

    _timeSendBlock->setInterval(100);
    _timeSendBlock->start();
}

void ModBus_Common_Protocol::protocolAfterStop()
{
    _timeReadCoilStatus->stop();
    _timeReadInputStatus->stop();
    _timeReadHoldingRegisters->stop();
    _timeReadInputRegisters->stop();
    _timeSendBlock->stop();

    ProtocolCommon::protocolAfterStop();
}

void ModBus_Common_Protocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
    if(oTimer == _timeSendBlock)
    {
        sendBuffer();
    }
    else if (oTimer == _timeReadCoilStatus) {
        TimeReadCoilStatus();
    } else if (oTimer == _timeReadInputStatus) {
        TimeReadInputStatus();
    } else if (oTimer == _timeReadHoldingRegisters) {
        TimeReadHoldingRegisters();
    } else if (oTimer == _timeReadInputRegisters) {
        TimeReadInputRegisters();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//读线圈状态
int ModBus_Common_Protocol::ReadCoilStatus(int iSlaveAddress, int iAddress, ushort wCount)
{
    //    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x01)
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x00)
        return -1;
    iAddress = addrMod(iAddress,true);
    return SendModbusReadData(iSlaveAddress, 1, iAddress, wCount,false);
}
//读输入状态
int ModBus_Common_Protocol::ReadInputStatus(int iSlaveAddress, int iAddress, ushort wCount)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x00)
        return -1;
    iAddress = addrMod(iAddress,true);
    return SendModbusReadData(iSlaveAddress, 2, iAddress, wCount,false);
}
//读保持寄存器
int ModBus_Common_Protocol::ReadHoldingRegisters(int iSlaveAddress, int iAddress, ushort wCount)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x00)
        return -1;
    iAddress = addrMod(iAddress,true);
    return SendModbusReadData(iSlaveAddress, 3, iAddress, wCount,false);
}
//读输入寄存器
int ModBus_Common_Protocol::ReadInputRegisters(int iSlaveAddress, int iAddress, ushort wCount)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x00)
        return -1;
    iAddress = addrMod(iAddress,true);
    return SendModbusReadData(iSlaveAddress, 4, iAddress, wCount,false);
}
//写单线圈
int ModBus_Common_Protocol::ForceSingleCoil(int iSlaveAddress, int iAddress, bool bOn)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > 0xFFFF || iAddress < 0x00)
        return -1;
    ushort data = ( bOn ) ? 0xFF00 : 0x0000;

    iAddress = addrMod(iAddress,true);
    return SendModbusForceData(iSlaveAddress, 5, iAddress, data);
}
//写单寄存器
int ModBus_Common_Protocol::PresetSingleRegister(int iSlaveAddress, int iAddress, ushort wForceData)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > 0xFFFF || iAddress < 0x00)
        return -1;

    iAddress = addrMod(iAddress,true);
    return SendModbusForceData(iSlaveAddress, 6, iAddress, wForceData);
}
//写多线圈
int ModBus_Common_Protocol::ForceMultipleCoils(int iSlaveAddress, int iAddress, const std::vector<bool> &values)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - (int)values.size()) || iAddress < 0x00)
        return -1;

    std::vector<uchar> data =  CxBuffer::toBuffer(values);

    iAddress = addrMod(iAddress,true);
    return SendModbusForceData(iSlaveAddress, 15, iAddress, values.size(), data);
}
//写多寄存器
int ModBus_Common_Protocol::PresetMultipleRegisters(int iSlaveAddress, int iAddress, const std::vector<ushort> &values)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - (int)values.size()) || iAddress < 0x00)
        return -1;

    std::vector<uchar> data;

    data.resize(values.size() * 2);
    for (size_t i = 0; i < values.size(); ++i) {
        ushort value = values.at(i);
        data[ i * 2 ] = value >> 8;
        data[ i * 2 + 1 ] = value;
    }

    iAddress = addrMod(iAddress,true);
    return SendModbusForceData(iSlaveAddress, 16, iAddress, values.size(), data);
}
//////////////////////////////////////////////////////////////////////////////////////////
//定时读线圈状态
void ModBus_Common_Protocol::TimeReadCoilStatus()
{
    if(_modeCoil>0 && _cmdBuffersCoil.size()>0)
    {
        for(size_t i=0;i<_cmdBuffersCoil.size();++i)
        {
            _asduSend.setValue(_cmdBuffersCoil.at(i));
            setUpFrame(false,false);
        }
    }
    else
    {
        MonsbAddressWorkerTable* oTable = _yxRecTable;

        if (_readCoilStatusIndex > oTable->stationIds().size() - 1) {
            _readCoilStatusIndex = 0;
        }

        if (_readCoilStatusIndex < oTable->stationIds().size())
        {
            int iSlaveAddress = oTable->stationIds().at(_readCoilStatusIndex);
            int iMinAddress = CxContainer::value(oTable->stationIdMins(),iSlaveAddress);
            int iMaxAddress = CxContainer::value(oTable->stationIdMaxes(),iSlaveAddress);
            int iCount = iMaxAddress - iMinAddress + 1;
            if (iCount > 2000) return;
            ReadCoilStatus(iSlaveAddress, iMinAddress, iCount);
            _readCoilStatusIndex ++;
        }
    }
}
//定时读输入状态
void ModBus_Common_Protocol::TimeReadInputStatus()
{
    if(_modeInputStatus>0 && _cmdBuffersInputStatus.size()>0)
    {
        for(size_t i=0;i<_cmdBuffersInputStatus.size();++i)
        {
            _asduSend.setValue(_cmdBuffersInputStatus.at(i));
            setUpFrame(false,false);
        }
    }
    else
    {

        MonsbAddressWorkerTable* oTable = _yxRecTable;
        if (_readCoilStatusIndex > oTable->stationIds().size() - 1) {
            _readCoilStatusIndex = 0;
        }

        if (_readCoilStatusIndex < oTable->stationIds().size()) {
            int iSlaveAddress = oTable->stationIds().at(_readCoilStatusIndex);
            int iMinAddress = CxContainer::value(oTable->stationIdMins(),iSlaveAddress);
            int iMaxAddress = CxContainer::value(oTable->stationIdMaxes(),iSlaveAddress);
            int iCount = iMaxAddress - iMinAddress + 1;
            if (iCount > 2000) return;
            ReadInputStatus(iSlaveAddress, iMinAddress, iCount);
            _readCoilStatusIndex ++;
        }
    }
}
//定时读保持寄存器
void ModBus_Common_Protocol::TimeReadHoldingRegisters()
{
    if(_modeHodingRegister>0 && _cmdBuffersHodingRegister.size()>0)
    {
        for(size_t i=0;i<_cmdBuffersHodingRegister.size();++i)
        {
            _asduSend.setValue(_cmdBuffersHodingRegister.at(i));
            setUpFrame(false,false);
        }
    }
    else
    {
        if (_stationMinMaxMids.size() <= 0) return;
        if (_readHoldingRegistersIt == _stationMinMaxMids.end())
        {
            _readHoldingRegistersIt = _stationMinMaxMids.begin();
        }
        int iSlaveAddress = _readHoldingRegistersIt->first;
        const std::pair<int, int> & minMaxMid = _readHoldingRegistersIt->second;
        int iMinAddress = minMaxMid.first;
        int iMaxAddress = minMaxMid.second;
        int iCount = iMaxAddress - iMinAddress + 1;
        if (iCount > _addrLimit) return;
        ReadHoldingRegisters(iSlaveAddress, iMinAddress, iCount);
        _readHoldingRegistersIt ++;
    }
}
//定时读输入寄存器
void ModBus_Common_Protocol::TimeReadInputRegisters()
{
    if(_modeInputRegister>0 && _cmdBuffersInputRegister.size()>0)
    {
        for(size_t i=0;i<_cmdBuffersInputRegister.size();++i)
        {
            _asduSend.setValue(_cmdBuffersInputRegister.at(i));
            setUpFrame(false,false);
        }
    }
    else
    {
        YcaddAddressWorkerTable* oTable = _ycRecTable;
        if (_readInputRegistersIndex > oTable->stationIds().size() - 1) {
            _readInputRegistersIndex = 0;
        }

        if (_readInputRegistersIndex < oTable->stationIds().size()) {
            int iSlaveAddress = oTable->stationIds().at(_readInputRegistersIndex);
            int iMinAddress = CxContainer::value(oTable->stationIdMins(),iSlaveAddress);
            int iMaxAddress = CxContainer::value(oTable->stationIdMaxes(),iSlaveAddress);
            int iCount = iMaxAddress - iMinAddress + 1;
            if (iCount > _addrLimit) return;
            ReadInputRegisters(iSlaveAddress, iMinAddress, iCount);
            _readInputRegistersIndex ++;
        }
    }
}

int ModBus_Common_Protocol::addrMod(int addr, bool bSend)
{
    int ret = addr;
    //发送
    if(bSend)
    {
        //西门子模式
        if(_addrType == CI_ADDR_TYPE_SIMENS)
        {
            if(addr>0)  ret = addr-1;
        }
    }
    else{ //接收
        //西门子模式
        if(_addrType == CI_ADDR_TYPE_SIMENS)
        {
            if(addr>=0)  ret = addr+1;
        }
    }
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////
/// 做主站，读数据，响应帧处理
void ModBus_Common_Protocol::DealReadCoilStatus()
{
    if(!protolAfterVfy()) return;

    if((_asduRev.fc()&0x80)>0) //返回吗错误
    {
        outProtocolDebug()<<"error:"<<_asduRev.fc()<<cxEndLine;
        return;
    }

    int addr =  _asduLast.addr();
    uchar *pData = _asduLast.buf();

    uint16 beginAddr = 0;
    int pos = 0;
    CxBuffer::fromBuf(pData+pos,sizeof(uint16),beginAddr,0);
    pos+= sizeof(uint16);

    beginAddr = addrMod(beginAddr,false);

    if(_typeCoil==GCL_VT_VL)//变长
    {
        map<string,string> mapPram;
        mapPram[CS_Modbus_StationID] = CxString::format("%d",_asduRev.addr());
        mapPram[CS_Modbus_BeginAddr] = CxString::format("%d",beginAddr);
        mapPram[CS_Modbus_FC] = CxString::format("%d",_asduRev.fc());

        uchar *pBuf = _recDealData.data();
        GclLuaVxd::dealCustomData(terminalId(),_asduRev.fc(),mapPram,_recDealData.size(),(char*)pBuf,0);
    }
    else
    {
        uint16 count = 0;
        CxBuffer::fromBuf(pData+pos,sizeof(uint16),count,0);
        DealReadStatus(addr, beginAddr, count);
    }
}

void ModBus_Common_Protocol::DealReadInputStatus()
{
    if(!protolAfterVfy()) return;

    if((_asduRev.fc()&0x80)>0) //返回吗错误
    {
        outProtocolDebug()<<"error:"<<_asduRev.fc()<<cxEndLine;
        return;
    }

    int addr =  _asduLast.addr();
    uchar *pData = _asduLast.buf();

    uint16 beginAddr = 0;
    int pos = 0;
    CxBuffer::fromBuf(pData+pos,sizeof(uint16),beginAddr,0);
    pos+= sizeof(uint16);

    beginAddr = addrMod(beginAddr,false);


    if(_typeInputStatus==GCL_VT_VL)//变长
    {
        map<string,string> mapPram;
        mapPram[CS_Modbus_StationID] = CxString::format("%d",_asduRev.addr());
        mapPram[CS_Modbus_BeginAddr] = CxString::format("%d",beginAddr);
        mapPram[CS_Modbus_FC] = CxString::format("%d",_asduRev.fc());

        uchar *pBuf = _recDealData.data();
        GclLuaVxd::dealCustomData(terminalId(),_asduRev.fc(),mapPram,_recDealData.size(),(char*)pBuf,0);
    }
    else
    {
        uint16 count = 0;
        CxBuffer::fromBuf(pData+pos,sizeof(uint16),count,0);
        DealReadStatus(addr, beginAddr, count);
    }
}

void ModBus_Common_Protocol::DealReadStatus(int iReadSlaveAddress, int iReadAddress, int iReadCount)
{
    if (_recSlaveAddress != iReadSlaveAddress) return;
    int iAddress = iReadAddress;
    std::map<int, int> addressValues;

    for (size_t i = 0; i < _recDealData.size(); ++i)
    {
        uchar data = _recDealData.at(i);
        for (int j = 0; j < 8; ++j)
        {
            if (iAddress > iReadAddress + iReadCount) break;
            int iValue = ( ( data >> j ) & 0x01 );
            addressValues[iAddress] = iValue;
            iAddress ++;
        }
    }
    _yxRecTable->receiveAddressValues(iReadSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());

}

void ModBus_Common_Protocol::DealReadHoldingRegisters()
{
    if(!protolAfterVfy()) return;

    if((_asduRev.fc()&0x80)>0) //返回吗错误
    {
        outProtocolDebug()<<"error:"<<_asduRev.fc()<<cxEndLine;
        return;
    }

//    outProtocolDebug()<<_asduRev.fc()<<_typeHodingRegister<<cxEndLine;

    int addr =  _asduLast.addr();
    uchar *pData = _asduLast.buf();

    uint16 beginAddr = 0;
    int pos = 0;
    CxBuffer::fromBuf(pData+pos,sizeof(uint16),beginAddr,0);
    pos+= sizeof(uint16);

    beginAddr = addrMod(beginAddr,false);


    if(_typeHodingRegister==GCL_VT_VL)//变长
    {
        map<string,string> mapPram;
        mapPram[CS_Modbus_StationID] = CxString::format("%d",_asduRev.addr());
        mapPram[CS_Modbus_BeginAddr] = CxString::format("%d",beginAddr);
        mapPram[CS_Modbus_FC] = CxString::format("%d",_asduRev.fc());

        uchar *pBuf = _recDealData.data();
        GclLuaVxd::dealCustomData(terminalId(),_asduRev.fc(),mapPram,_recDealData.size(),(char*)pBuf,0);
    }
    else
    {
        uint16 count = 0;
        CxBuffer::fromBuf(pData+pos,sizeof(uint16),count,0);
        DealReadRegisters(addr, beginAddr, count,_typeHodingRegister);
    }
}

void ModBus_Common_Protocol::DealReadInputRegisters()
{
    if(!protolAfterVfy()) return;

    if((_asduRev.fc()&0x80)>0) //返回吗错误
    {
        outProtocolDebug()<<"error:"<<_asduRev.fc()<<cxEndLine;
        return;
    }

    int addr =  _asduLast.addr();
    uchar *pData = _asduLast.buf();

    uint16 beginAddr = 0;
    int pos = 0;
    CxBuffer::fromBuf(pData+pos,sizeof(uint16),beginAddr,0);
    pos+= sizeof(uint16);

    beginAddr = addrMod(beginAddr,false);

    if(_typeInputRegister==GCL_VT_VL)//变长
    {
        map<string,string> mapPram;
        mapPram[CS_Modbus_StationID] = CxString::format("%d",_asduRev.addr());
        mapPram[CS_Modbus_BeginAddr] = CxString::format("%d",beginAddr);
        mapPram[CS_Modbus_FC] = CxString::format("%d",_asduRev.fc());

        uchar *pBuf = _recDealData.data();
        GclLuaVxd::dealCustomData(terminalId(),_asduRev.fc(),mapPram,_recDealData.size(),(char*)pBuf,0);
    }
    else
    {
        uint16 count = 0;
        CxBuffer::fromBuf(pData+pos,sizeof(uint16),count,0);

        DealReadRegisters(addr, beginAddr, count,_typeInputRegister);
    }
}
void ModBus_Common_Protocol::DealReadRegisters(int iReadSlaveAddress, int iReadAddress, int iReadCount, int mode)
{
    if (_recSlaveAddress != iReadSlaveAddress)
    {
        outProtocolPrompt()<<"DealReadRegisters:" << iReadSlaveAddress << (int)_recSlaveAddress;
        return;
    }

    int iAddress = iReadAddress;
    std::map<int, double> addressValues;

    int iType = mode&0x0f;   //低四位表示数据类型
    int iFormat  = (mode&0xf0)>>4;//高四位表示数据格式


    switch(iType)
    {
    /*
    模拟量的数据格式采用IEEE STD754“短浮点数”格式 即传输4字节短浮点数，占用两个寄存器。
    格式采用，LowWord HiWord
    LowWord：HiByte LowByte       HiWord：HiByte LowByte
    */
    case 0: //浮点数
    {
        uchar *pBuf = _recDealData.data();
        int count = _recDealData.size()/sizeof(float);
        for(int i=0;i<count;++i)
        {
            float fVal = 0.0f;
            CxBuffer::fromBuf(pBuf+i*sizeof(float),sizeof(float),fVal,iFormat);
//            outProtocolDebug()<<"YC Value:" << iAddress << fVal<< CxString::toHexstring(pBuf+i*sizeof(float),sizeof(float))<<cxEndLine;
            addressValues[iAddress] = (double)fVal;
            iAddress ++;
        }
    }
        break;

    case 1://短整形2 short
    {
        short nValue = 0;
        uchar *pBuf = _recDealData.data();
        int count = _recDealData.size()/sizeof(short);
        for(int i=0;i<count;++i)
        {
            CxBuffer::fromBuf(pBuf+i*sizeof(short),sizeof(short),nValue,(bool)iFormat);
//            outProtocolDebug()<<"YC Value:" << iAddress << nValue<<cxEndLine;
            addressValues[iAddress] = (double)nValue;
            iAddress ++;
        }
    }
        break;

    case 2://整形4个字节
    {
        std::map<MonsbAddressWorker* , int> monsbWorkerValues;
        std::map<YcaddAddressWorker* , double> ycaddWorkerValues;

//        MonsbMeasure::beginSetValues(_yxRecTable->measureSonType(), protocolId());
//        YcaddMeasure::beginSetValues(_ycRecTable->measureSonType(), protocolId());

        uchar *pBuf = _recDealData.data();
        int iCount = _recDealData.size()/sizeof(int);
        int iStep = sizeof(int) / 2;
        for(int i=0;i<iCount;++i)
        {
            YcaddAddressWorker * oYcaddWorker = _ycRecTable->workerByStationIdAddress(iReadSlaveAddress, iAddress);
            if (oYcaddWorker)
            {
                int iValue = 0;
                CxBuffer::fromBuf(pBuf+i*sizeof(int),sizeof(int),iValue,iFormat);
                double dValue = iValue;
                ycaddWorkerValues[oYcaddWorker] = dValue;
//                oYcaddWorker->setValue(dValue, dValue, iSourceId, 0);
            }
            else
            {
                MonsbAddressWorker * oMonsbWorker = _yxRecTable->workerByStationIdAddress(iReadSlaveAddress, iAddress);
                if (oMonsbWorker)
                {
                    int iValue = 0;
                    CxBuffer::fromBuf(pBuf+i*sizeof(int),sizeof(int),iValue,iFormat);
                    monsbWorkerValues[oMonsbWorker] = iValue;
//                    oMonsbWorker->setValue(iValue, iValue, iSourceId, 0);
                 }
            }
            iAddress += iStep;
        }

        if (ycaddWorkerValues.size()>0)
        {
            _ycRecTable->receiveWorkerValues(ycaddWorkerValues, protocolId());
        }
        if (monsbWorkerValues.size()>0)
        {
            _yxRecTable->receiveWorkerValues(monsbWorkerValues, protocolId());
        }

//        YcaddMeasure::endSetValues();
//        MonsbMeasure::endSetValues();

        outProtocolDebug()<<"HoldingRegisters Rec Count(Register):" << iCount;
    }
        break;
    default:
        break;
    }

    if(addressValues.size()>0)
    {
        _ycRecTable->receiveAddressValues(iReadSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
///做从站,读响应
///
void ModBus_Common_Protocol::ResponseReadCoilStatus()
{
    ResponseReadStatus(1);
}

void ModBus_Common_Protocol::ResponseReadInputStatus()
{
    ResponseReadStatus(2);
}

void ModBus_Common_Protocol::ResponseReadStatus(uchar byFuncCode)
{
    std::vector<uchar> data;
    int size = CxBuffer::count(_recResponseQuantity, 8);
    data.resize(size+1,0);

    uchar * pData = & data.front();
    data[0]=size;

    const std::vector<MonsbAddressWorker*>* oWorkers = _yxSendTable->workers();
    for (size_t i = 0; i < oWorkers->size(); ++i) {
        MonsbAddressWorker* oWorker = oWorkers->at(i);
        if (! oWorker->measure() || oWorker->measure()->value() != GCL_PT_YX_ON) continue;
        int iAddress = oWorker->address();
        if (oWorker->stationId() == _recSlaveAddress && iAddress >= _recResponseAddress && iAddress < _recResponseAddress + _recResponseQuantity)
        {
            uchar indexMod;
            int iIndex = CxBuffer::index(8, iAddress, &indexMod, _recResponseAddress);
            if (iIndex >= 0 && iIndex < (int)data.size() && indexMod < 8)
            {
                CxBuffer::setBitTrue(pData+1+iIndex, indexMod);
            }
        }
    }
    SendModbusReadData(_recSlaveAddress, byFuncCode, data,true);
}

void ModBus_Common_Protocol::ResponseReadHoldingRegisters()
{
    ResponseReadRegisters(3,_typeHodingRegister);
}

void ModBus_Common_Protocol::ResponseReadInputRegisters()
{
    ResponseReadRegisters(4,_typeInputRegister);
}

void ModBus_Common_Protocol::ResponseReadRegisters(uchar byFuncCode, int mode)
{
    if(mode==0)//浮点数 float
    {
        std::vector<uchar> data;
        data.resize(_recResponseQuantity*sizeof(float)+1,0);
        uchar * pData = & data.front();
        data[0]=_recResponseQuantity*sizeof(float);

        const std::vector<YcaddAddressWorker*>* oWorkers = _ycSendTable->workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            YcaddAddressWorker* oWorker = oWorkers->at(i);
            const YcaddMeasure* oYcadd = oWorker->measure();
            if (! oYcadd) continue;
            int iAddress = oWorker->address();
            if (oWorker->stationId() == _recSlaveAddress && iAddress >= _recResponseAddress && iAddress < _recResponseAddress + _recResponseQuantity)
            {
                int iIndex = iAddress - _recResponseAddress;
                float v = oYcadd->value() * oWorker->bianbi();
                CxBuffer::toBuf(pData+1+sizeof(float)*iIndex,v,1);
            }
        }
        SendModbusReadData(_recSlaveAddress, byFuncCode, data,true);
    }
    else if(mode==1)//短整型 unit16
    {
        std::vector<uchar> data;
        data.resize(_recResponseQuantity*sizeof(uint16)+1,0);
        uchar * pData = & data.front();
        data[0]=_recResponseQuantity*sizeof(uint16);

        const std::vector<YcaddAddressWorker*>* oWorkers = _ycSendTable->workers();
        for (size_t i = 0; i < oWorkers->size(); ++i)
        {
            YcaddAddressWorker* oWorker = oWorkers->at(i);
            const YcaddMeasure* oYcadd = oWorker->measure();
            if (! oYcadd) continue;
            int iAddress = oWorker->address();
            if (oWorker->stationId() == _recSlaveAddress && iAddress >= _recResponseAddress && iAddress < _recResponseAddress + _recResponseQuantity) {
                int iIndex = iAddress - _recResponseAddress;
                ushort w = oYcadd->value() * oWorker->bianbi();
                CxBuffer::toBuf(pData+1+sizeof(uint16)*iIndex,w,0);
            }
        }
        SendModbusReadData(_recSlaveAddress, byFuncCode, data,true);
    }
}
///////////////////////////////////////////////////////////////////////////
//做从站,写响应
//单线圈
void ModBus_Common_Protocol::ResponseForceSingleCoil()
{
    int iSlaveAddress = _recSlaveAddress;
    int iAddress = _recForceAddress;
    int iValue = ( _recForceSingleData > 0 ) ? GCL_PT_YX_ON : GCL_PT_YX_OFF;

    std::map<int, int> addressValues;
    addressValues[iAddress] = iValue;
    if(_yxRecTable)
    {
        _yxRecTable->receiveAddressValues(iSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}
//单寄存器
void ModBus_Common_Protocol::ResponsePresetSingleRegister()
{
    int iSlaveAddress = _recSlaveAddress;
    int iAddress = _recForceAddress;
    double dValue = _recForceSingleData;

    std::map<int, double> addressValues;
    addressValues[iAddress] = dValue;
    if(_ycRecTable)
    {
        _ycRecTable->receiveAddressValues(iSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}
//多线圈
void ModBus_Common_Protocol::ResponseForceMultipleCoils()
{
    if(_yxRecTable==NULL) return;
    if(_yxRecTable->workers()==NULL || _yxRecTable->workers()->size()<1) return;

    int iSlaveAddress = _recSlaveAddress;
    int iAddress = _recForceAddress;
    std::map<int, int> addressValues;
    for (size_t i = 0; i < _recForceMultipleData.size(); ++i)
    {
        uchar data = _recForceMultipleData.at(i);
        for (int j = 0; j < 8; ++j)
        {
            if (iAddress > _recForceAddress + _recForceMultipleQuantity) break;
            int iValue = ( ( data >> j ) & 0x01 );

            addressValues[iAddress] = (iValue>0?GCL_PT_YX_ON:GCL_PT_YX_OFF);
            iAddress ++;
        }
    }

    if(addressValues.size()>0)
    {
        _yxRecTable->receiveAddressValues(iSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}
//多寄存器 need to do
void ModBus_Common_Protocol::ResponsePresetMultipleRegisters()
{
    if(_ycRecTable==NULL) return;
    if(_ycRecTable->workers()==NULL || _ycRecTable->workers()->size()<1) return;

    int iSlaveAddress = _recSlaveAddress;
    int iAddress = _recForceAddress;
    //double dValue = 0;
    std::map<int, double> addressValues;


    int type = _typeHodingRegister&0x0f;   //低四位表示数据类型
    int fmt  = (_typeHodingRegister&0xf0)>>4;//高四位表示数据格式

    switch(type)
    {
    /*
    模拟量的数据格式采用IEEE STD754“短浮点数”格式 即传输4字节短浮点数，占用两个寄存器。
    格式采用，LowWord HiWord
    LowWord：HiByte LowByte       HiWord：HiByte LowByte
    */
    case 0: //浮点数
    {
        uchar *pBuf = _recForceMultipleData.data();
        int count = _recForceMultipleData.size()/sizeof(float);
        for(int i=0;i<count;++i)
        {
            float fVal = 0.0f;
            CxBuffer::fromBuf(pBuf+i*sizeof(float),sizeof(float),fVal,fmt);
//            outProtocolDebug()<<"YC Value:" << iAddress << fVal<< CxString::toHexstring(pBuf+i*sizeof(float),sizeof(float))<<cxEndLine;
            addressValues[iAddress] = (double)fVal;
            iAddress ++;
        }
    }
        break;

    case 1://短整形2 short
    {
        uchar *pBuf = _recForceMultipleData.data();
        int count = _recForceMultipleData.size()/sizeof(short);
        for(int i=0;i<count;++i)
        {
            int nValue=0;
            CxBuffer::fromBuf(pBuf+i*sizeof(short),sizeof(short),nValue,false);
//            outProtocolDebug()<<"YC Value:" << iAddress << nValue<<cxEndLine;
            addressValues[iAddress] = (double)nValue;
            iAddress ++;
        }
    }
        break;
    default:
        break;
    }



//    for (int i = 0; i < _recForceMultipleData.size(); ++i)
//    {
//        uchar data = _recForceMultipleData.at(i);
//        if (i % 2)
//        {
//            dValue += data;
//            addressValues[iAddress] = dValue;
//            iAddress ++;
//        }
//        else
//        {
//            dValue = data << 8;
//        }
//    }

    if(addressValues.size()>0)
    {
        _ycRecTable->receiveAddressValues(iSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 报文组织
int ModBus_Common_Protocol::SendModbusReadData(uchar iSlaveAddress, uchar byFuncCode, ushort wAddress, ushort wQuantity,bool bResp)
{
    uchar buffer[4];
    int pos =0;
    buffer[pos++] = wAddress >> 8;
    buffer[pos++] = wAddress;
    buffer[pos++] = wQuantity >> 8;
    buffer[pos++] = wQuantity;

    _asduSend.setValue(iSlaveAddress,byFuncCode,pos,buffer);
    return setUpFrame(bResp,bResp);
}

int ModBus_Common_Protocol::SendModbusReadData(uchar iSlaveAddress, uchar byFuncCode, const std::vector<uchar> &data,bool bResp)
{
    int iSize = data.size();
    if (iSize > 255) return -1;

    _asduSend.setValue(iSlaveAddress,byFuncCode,iSize,(uchar*)data.data());
    return setUpFrame(bResp,bResp);
}

int ModBus_Common_Protocol::SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort wAddress, ushort data)
{
    uchar buffer[4];
    int pos =0;
    buffer[pos++] = wAddress >> 8;
    buffer[pos++] = wAddress;
    buffer[pos++] = data >> 8;
    buffer[pos++] = data;

    _asduSend.setValue(iSlaveAddress,byFuncCode,pos,buffer);
    return setUpFrame(false);
}

int ModBus_Common_Protocol::SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort wAddress, ushort wQuantity, std::vector<uchar> &data)
{
    int iSize = data.size();
    if (iSize > 255)   return -1;
    int iLength = 5 + iSize;
    std::vector<uchar> buffer(iLength);
    int pos =0;
    buffer[pos++] = wAddress >> 8;
    buffer[pos++] = wAddress;
    buffer[pos++] = wQuantity >> 8;
    buffer[pos++] = wQuantity;
    buffer[pos++] = iSize;
    memcpy(&buffer[pos], data.data(), iSize);

    _asduSend.setValue(iSlaveAddress,byFuncCode,iLength,(uchar*)buffer.data());
    return setUpFrame(false);
}




