#include "modbus_tcp_common.h"
#include "modbus_global.h"
#include <script/gcl_script_vxd.h>

using namespace std;

ModBus_TCP_ProtocolFactory* ModBus_TCP_ProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<ModBus_TCP_ProtocolFactory>();

TCP_ModbusMasterProtocolFactory* TCP_ModbusMasterProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<TCP_ModbusMasterProtocolFactory>();


TCP_ModbusSlaveProtocolFactory* TCP_ModbusSlaveProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<TCP_ModbusSlaveProtocolFactory>();




ModBus_TCP_Protocol::ModBus_TCP_Protocol()
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

    _readCoilStatusInterval = 10000;
    _readInputStatusInterval = 10000;
    _readHoldingRegistersInterval = 10000;
    _readInputRegistersInterval = 10000;

    _readCoilStatusIsYx = true;
    _readInputStatusIsYx = true;
    _forceSingleCoilIsYx = false;
    _forceMultipleCoilsIsYx = false;

    _dataMode = 0;

    _typeInputRegister = 0;
    _sessionFlag = 0;
    _sendEnable = false;
    _tmLast = CxTime::currentMsepoch();

    //     CxChannelManager::setPacketInterinfoOut(true);
}

ModBus_TCP_Protocol::~ModBus_TCP_Protocol()
{
    delete _timeReadCoilStatus      ;
    delete _timeReadInputStatus     ;
    delete _timeReadHoldingRegisters;
    delete _timeReadInputRegisters  ;
    delete _timeSendBlock;
}
//const std::string CS_EntryModbusCoilStatusType = "CoilStatusType";
//const std::string CS_EntryModbusInputStatusType = "InputStatusType";
//const std::string CS_EntryModbusHoldingRegistersType = "HoldingRegistersType";
//const std::string CS_EntryModbusInputRegistersInType = "InputRegistersType";

void ModBus_TCP_Protocol::toContext(CxISetSkv& context) const
{
    ProtocolCommon::toContext(context);

    context.setValue(CS_EntryModbusReadCoilStatusInterval,       _readCoilStatusInterval);
    context.setValue(CS_EntryModbusReadInputStatusInterval,      _readInputStatusInterval);
    context.setValue(CS_EntryModbusReadHoldingRegistersInterval, _readHoldingRegistersInterval);
    context.setValue(CS_EntryModbusReadInputRegistersInterval,   _readInputRegistersInterval);

    context.setValue(CS_EntryModbusHoldingRegistersType,_typeInputRegister);

}

void ModBus_TCP_Protocol::fromContext(const CxIGetSkv& context)
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

    _typeInputRegister              = context.getValue(CS_EntryModbusHoldingRegistersType,_typeInputRegister);
    _modeInputRegister              = context.getValue(CS_EntryModbusInputRegistersInCmdMode,_modeInputRegister);
    _szBuffersInputRegister        = context.getValue(CS_EntryModbusInputRegistersCmd,std::string());
    CxString::splitCase(_cmdBuffersInputRegister,_szBuffersInputRegister,split);


}
void ModBus_TCP_Protocol::protocolReceivedData(const uchar *pData, int iLength)
{
#ifndef  GMAC_TEST
    //收到数据
    receivedProtocolDataNow();
    //数据压榨
    _revBuffer.putIn(pData,(uint16)iLength);
    //数据处理
    dealData();

//    outProtocolPrompt()<<"dealData()"<<iLength<<cxEndLine;
#endif
}

void ModBus_TCP_Protocol::doTask(GCCommon::GCDataElement dt)
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
void ModBus_TCP_Protocol::clock()
{

}

void ModBus_TCP_Protocol::dealData()
{
    uint16 wLen = 0;
    uchar *pData = _revBuffer.getBuf(wLen);
    int nStart = findHeader(pData,wLen);
    //未找到头
    if(nStart<0)
    {
        reset();
        outProtocolDebug()<<"no find head" <<wLen<<cxEndLine;
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

        if ( (_recFuncCode == 1 && _readCoilStatusIsMaster) || (_recFuncCode == 2 && _readInputStatusIsMaster)
             || (_recFuncCode == 3 && _readHoldingRegistersIsMaster) || (_recFuncCode == 4 && _readInputRegistersIsMaster) )
        {
            ModbusReceiveModelDeal();
        }
        else if ( _recFuncCode <= 4)
        {
            ModbusReceiveModelResponse(); //Read
        }
        else  if ( (_recFuncCode == 5 && _forceSingleCoilIsMaster) || (_recFuncCode == 6 && _presetSingleRegisterIsMaster )
                   || (_recFuncCode == 15 && _forceMultipleCoilsIsMaster) || (_recFuncCode == 16 && _presetMultipleRegistersIsMaster) )
        {
            ModbusReceiveModelReply();
        }
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

void ModBus_TCP_Protocol::callUp()
{

}
//发送命令数据
int ModBus_TCP_Protocol::sendCustomData(int iSourceId, int iCommandId, std::map<string, string> sParams, int iDataLength, const char *pData)
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

int ModBus_TCP_Protocol::sendVTLData(int iCommandId, std::map<string, string> sParams, int iDataLength, const char *pData)
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

    if(nBeginAddr>0)nBeginAddr -= 1;

    vector<uchar> vBuf;
    vBuf.clear();
    switch(iCommandId)
    {
    case GCL_CMD_SYS_VTL_W://64
    {
        for(size_t i =0;i<iDataLength;i++)vBuf.push_back(pData[i]);
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
        for(size_t i =0;i<iDataLength;i++)
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
int ModBus_TCP_Protocol::findHeader(uchar *pData, int iLength)
{
    if((pData==NULL)||(iLength<6)) return -1;

    int nFind = -1;

    for(int i=0;i<iLength;i++)
    {
        if(_headRev.fromBuff(pData+i,iLength-i))
        {
            if(_headRev.asduLen()+_headRev.size()<=(iLength-i))
            {
                nFind = i;
                break;
            }
        }
    }
    return nFind;
}

bool ModBus_TCP_Protocol::checkFrame(uchar *pData, int iLength, int &nFrmLen)
{
    if((pData==NULL)||(iLength<6)) return false;
    bool bRet = false;
    if(_headRev.fromBuff(pData,iLength))
    {
        nFrmLen = _headRev.asduLen()+_headRev.size();
        if(nFrmLen<=iLength)
        {
            if(_asduRev.fromBuff(pData+_headRev.size(),_headRev.asduLen()))
            {
                bRet = true;
            }
        }
    }
    return bRet;
}

int ModBus_TCP_Protocol::setUpFrame(bool isResp, bool sendNow)
{
    memset(_sendBuf,0,GCL_LEN_DATA_EM);
    _sendLen = 0;

    if(isResp)
    {
        _headSend = _headRev;
    }
    else
    {
        _headSend.setSessionFlag(++_sessionFlag);
        _headSend.setProtcolFlag(1);
    }

    _headSend.setAsduLen(_asduSend.size());

    int pos = 0;


    pos = _headSend.toBuff(_sendBuf);
    if(pos==0) return -1;

    _sendLen += pos;
    pos = _asduSend.toBuff(_sendBuf+_sendLen);
    if(pos==0) return -1;
    _sendLen += pos;

    if(sendNow)
    {
        writeData(_sendBuf,_sendLen);
    }
    else
    {
        //        push(_sendBuf,_sendLen);
        vector<uchar> v1;
        for(int i=0;i<_sendLen;i++) v1.push_back(_sendBuf[i]);
        _sentFrms.push(v1);
        _sendEnable= true;
    }

    return _sendLen;
}

//IsMaster : 1 , 2 , 3, 4
//Receive : yx , yc , yk
void ModBus_TCP_Protocol::ModbusReceiveModelDeal()
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
void ModBus_TCP_Protocol::ModbusReceiveModelResponse()
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
void ModBus_TCP_Protocol::ModbusReceiveModelReply()
{

    if(_asduRev.len()<4)return;

    uchar *pBuf = _asduRev.buf();
    int pos = 0;
    CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_recResponseAddress,_dataMode);
    pos += sizeof(uint16);
    CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_recResponseQuantity,_dataMode);

}

//IsSlave : 5 , 6 , 15 , 16
//Receive : yx , yk , yc
void ModBus_TCP_Protocol::ModbusReceiveModelForce()
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
    _headSend = _headRev;
    _headSend.setAsduLen(_asduSend.size());
    setUpFrame();
}

void ModBus_TCP_Protocol::protocolAfterStart()
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

    CxContainer::append(stationIdAll,_yxRecTable->stationIds());
    CxContainer::append(stationIdAll,_ykRecTable->stationIds());
    CxContainer::append(stationIdAll,_ycRecTable->stationIds());

    _stationIdAll.clear();
    for (int i = 0; i < stationIdAll.size(); ++i)
    {
        int iStationId = stationIdAll.at(i);
        if (!CxContainer::contain(_stationIdAll,iStationId))
        {
            _stationIdAll.push_back(iStationId);
        }
    }
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

void ModBus_TCP_Protocol::protocolAfterStop()
{
    _timeReadCoilStatus->stop();
    _timeReadInputStatus->stop();
    _timeReadHoldingRegisters->stop();
    _timeReadInputRegisters->stop();
    _timeSendBlock->stop();

    ProtocolCommon::protocolAfterStop();
}

void ModBus_TCP_Protocol::protocolTimerTimeOut(const CxTimer *oTimer)
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
int ModBus_TCP_Protocol::ReadCoilStatus(int iSlaveAddress, int iAddress, ushort wCount)
{
    //    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x01)
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x00)
        return -1;
    _readCoilStatusSlaveAddress = iSlaveAddress;
    _readCoilStatusAddress = iAddress;
    _readCoilStatusCount = wCount;
    return SendModbusReadData(iSlaveAddress, 1, iAddress - 1, wCount);
}
//读输入状态
int ModBus_TCP_Protocol::ReadInputStatus(int iSlaveAddress, int iAddress, ushort wCount)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x00)
        return -1;
    _readInputStatusSlaveAddress = iSlaveAddress;
    _readInputStatusAddress = iAddress;
    _readInputStatusCount = wCount;
    return SendModbusReadData(iSlaveAddress, 2, iAddress - 1, wCount);
}
//读保持寄存器
int ModBus_TCP_Protocol::ReadHoldingRegisters(int iSlaveAddress, int iAddress, ushort wCount)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x00)
        return -1;
    _readHoldingRegistersSlaveAddress = iSlaveAddress;
    _readHoldingRegistersAddress = iAddress;
    _readHoldingRegistersCount = wCount;
    return SendModbusReadData(iSlaveAddress, 3, iAddress - 1, wCount);
}
//读输入寄存器
int ModBus_TCP_Protocol::ReadInputRegisters(int iSlaveAddress, int iAddress, ushort wCount)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - wCount) || iAddress < 0x00)
        return -1;
    _readInputRegistersSlaveAddress = iSlaveAddress;
    _readInputRegistersAddress = iAddress;
    _readInputRegistersCount = wCount;
    return SendModbusReadData(iSlaveAddress, 4, iAddress - 1, wCount);
}
//写单线圈
int ModBus_TCP_Protocol::ForceSingleCoil(int iSlaveAddress, int iAddress, bool bOn)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > 0xFFFF || iAddress < 0x00)
        return -1;
    ushort data = ( true ) ? 0xFF00 : 0x0000;
    return SendModbusForceData(iSlaveAddress, 5, iAddress - 1, data);
}
//写单寄存器
int ModBus_TCP_Protocol::PresetSingleRegister(int iSlaveAddress, int iAddress, ushort wForceData)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > 0xFFFF || iAddress < 0x00)
        return -1;
    return SendModbusForceData(iSlaveAddress, 6, iAddress - 1, wForceData);
}
//写多线圈
int ModBus_TCP_Protocol::ForceMultipleCoils(int iSlaveAddress, int iAddress, const std::vector<bool> &values)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - values.size()) || iAddress < 0x00)
        return -1;

    std::vector<uchar> data =  CxBuffer::toBuffer(values);
    return SendModbusForceData(iSlaveAddress, 15, iAddress - 1, values.size(), data);
}
//写多寄存器
int ModBus_TCP_Protocol::PresetMultipleRegisters(int iSlaveAddress, int iAddress, const std::vector<ushort> &values)
{
    if (iSlaveAddress > 0xFF || iSlaveAddress < 0x00 || iAddress > (0xFFFF - values.size()) || iAddress < 0x00)
        return -1;

    std::vector<uchar> data;

    data.resize(values.size() * 2);
    for (int i = 0; i < values.size(); ++i) {
        ushort value = values.at(i);
        data[ i * 2 ] = value >> 8;
        data[ i * 2 + 1 ] = value;
    }
    return SendModbusForceData(iSlaveAddress, 16, iAddress - 1, values.size(), data);
}
//////////////////////////////////////////////////////////////////////////////////////////
//定时读线圈状态
void ModBus_TCP_Protocol::TimeReadCoilStatus()
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
        MonsbAddressWorkerTable* oTable = (_readCoilStatusIsYx) ? _yxRecTable : _ykRecTable;

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
void ModBus_TCP_Protocol::TimeReadInputStatus()
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

        MonsbAddressWorkerTable* oTable = (_readCoilStatusIsYx) ? _yxRecTable : _ykRecTable;
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
void ModBus_TCP_Protocol::TimeReadHoldingRegisters()
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
        YcaddAddressWorkerTable* oTable = _ycRecTable;
        if (_readHoldingRegistersIndex > oTable->stationIds().size() - 1) {
            _readHoldingRegistersIndex = 0;
        }

        if (_readHoldingRegistersIndex < oTable->stationIds().size()) {
            int iSlaveAddress = oTable->stationIds().at(_readHoldingRegistersIndex);
            int iMinAddress = CxContainer::value(oTable->stationIdMins(),iSlaveAddress);
            int iMaxAddress = CxContainer::value(oTable->stationIdMaxes(),iSlaveAddress);
            int iCount = iMaxAddress - iMinAddress + 1;
            if (iCount > 125) return;
            ReadHoldingRegisters(iSlaveAddress, iMinAddress, iCount);
            _readHoldingRegistersIndex ++;
        }
    }
}
//定时读输入寄存器
void ModBus_TCP_Protocol::TimeReadInputRegisters()
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
            if (iCount > 125) return;
            ReadInputRegisters(iSlaveAddress, iMinAddress, iCount);
            _readInputRegistersIndex ++;
        }
    }
}
bool ModBus_TCP_Protocol::sendBuffer()
{
    if(!_sendEnable)
    {
        //超时处理
        if((CxTime::currentMsepoch()-_tmLast)>1000)
        {
            int nCount = _sentFrms.size();
            if(nCount>0)//超时清空发送队列
            {
                for(size_t i=0;i<nCount;++i)_sentFrms.pop();
            }
            _tmLast = CxTime::currentMsepoch();
        }
        return false;
    }

    bool bRet = false;
    if (_sentFrms.size()>0)
    {
        std::vector<uchar> buffer = _sentFrms.front();
        _sentFrms.pop();

        uchar *pData = buffer.data();
        int iLength = buffer.size();
        if(writeData(pData,iLength)>0)
        {
            //保留最后发出帧信息
            if(_headLast.fromBuff(pData,iLength))
            {
                int nFrmLen = _headLast.asduLen()+_headLast.size();
                if(nFrmLen<=iLength)
                {
                    if(_asduLast.fromBuff(pData+_headLast.size(),_headLast.asduLen()))
                    {
                        bRet = true;
                    }
                }
            }
            _sendEnable = false;
            _tmLast = CxTime::currentMsepoch();
        }
    }
    return bRet;

}
/////////////////////////////////////////////////////////////////////////////////////////
// 报文组织
int ModBus_TCP_Protocol::SendModbusReadData(uchar iSlaveAddress, uchar byFuncCode, ushort wAddress, ushort wQuantity)
{
    uchar buffer[4];
    int pos =0;
    buffer[pos++] = wAddress >> 8;
    buffer[pos++] = wAddress;
    buffer[pos++] = wQuantity >> 8;
    buffer[pos++] = wQuantity;

    _asduSend.setValue(iSlaveAddress,byFuncCode,pos,buffer);
    return setUpFrame(false);
}

int ModBus_TCP_Protocol::SendModbusReadData(uchar iSlaveAddress, uchar byFuncCode, const std::vector<uchar> &data)
{
    int iSize = data.size();
    if (iSize > 255) return -1;

    _asduSend.setValue(iSlaveAddress,byFuncCode,iSize,(uchar*)data.data());
    return setUpFrame(false);
}

int ModBus_TCP_Protocol::SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort wAddress, ushort data)
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

int ModBus_TCP_Protocol::SendModbusForceData(uchar iSlaveAddress, uchar byFuncCode, ushort wAddress, ushort wQuantity, std::vector<uchar> &data)
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



///Modbus Master*********
TCP_ModbusMasterProtocol::TCP_ModbusMasterProtocol()
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

TCP_ModbusMasterProtocol::~TCP_ModbusMasterProtocol()
{
}

///Modbus Master---------

///Modbus Slave*********
TCP_ModbusSlaveProtocol::TCP_ModbusSlaveProtocol()
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

TCP_ModbusSlaveProtocol::~TCP_ModbusSlaveProtocol()
{

}


///Modbus Slave---------



