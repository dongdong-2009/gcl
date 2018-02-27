#include "modbus_rtu.h"
#include "modbus_global.h"

#include <script/gcl_script_vxd.h>

ModBus_RTU_ProtocolFactory* ModBus_RTU_ProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<ModBus_RTU_ProtocolFactory>();

RTU_ModbusMasterProtocolFactory* RTU_ModbusMasterProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<RTU_ModbusMasterProtocolFactory>();

RTU_ModbusSlaveProtocolFactory* RTU_ModbusSlaveProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<RTU_ModbusSlaveProtocolFactory>();


ModBus_RTU_Protocol::ModBus_RTU_Protocol()
{
}

ModBus_RTU_Protocol::~ModBus_RTU_Protocol()
{
}

int ModBus_RTU_Protocol::findHeader(uchar *pData, int iLength)
{
    if((pData==NULL)||(iLength<5)) return -1;

    int nFind = -1;

    int iLen = iLength-5;
    int addr = _asduLast.addr();
    for(int i=0;i<=iLen;i++)
    {
        if(addr!=pData[i])continue;

        //正常响应
        if(pData[i+1]<0x20)
        {
            int nFrmLen = pData[i+2]+5;
            if(nFrmLen<= (iLength-i))
            {
                uint16 wCrc = CxCrc::calcModbus(&pData[i], nFrmLen-2);
                uint16 crc = (pData[i+nFrmLen-2]<<8)|pData[i+nFrmLen-1];
                if(wCrc == crc)
                {
                   nFind = i;
                   break;
                }

            }
        }
        //异常响应
        else if(pData[i+1]>0x80 && pData[i+1]<0xA0)
        {
            int nFrmLen = 5;
            if(nFrmLen<= (iLength-i))
            {
                uint16 wCrc = CxCrc::calcModbus(&pData[i], nFrmLen-2);
                uint16 crc = (pData[i+nFrmLen-2]<<8)|pData[i+nFrmLen-1];
                if(wCrc == crc)
                {
                   nFind = i;
                   break;
                }

            }
        }
        else
        {
            continue;
        }
    }
    return nFind;
}

bool ModBus_RTU_Protocol::checkFrame(uchar *pData, int iLength, int &nFrmLen)
{
    if((pData==NULL)||(iLength<5)) return false;
    bool bRet = false;
    nFrmLen = 0;
    //正常响应
    if(pData[1]<0x20)
    {
        nFrmLen = pData[2]+5;
        if(nFrmLen<=iLength)
        {
            bRet = true;
        }
    }
    //异常响应
    else if(pData[1]>0x80 && pData[1]<0xA0)
    {
        nFrmLen = 5;
        bRet = true;
    }

    if(_asduRev.fromBuff(pData,nFrmLen))
    {
        bRet = true;
    }

    return bRet;
}

bool ModBus_RTU_Protocol::sendBuffer()
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
            if(_asduLast.fromBuff(pData,iLength))
            {
                bRet = true;
            }
            _sendEnable = false;
            _tmLast = CxTime::currentMsepoch();
        }
    }
    return bRet;

}

int ModBus_RTU_Protocol::setUpFrame(bool isResp, bool sendNow)
{
    memset(_sendBuf,0,GCL_LEN_DATA_EM);
    _sendLen = 0;

    int pos = 0;

    pos = _asduSend.toBuff(_sendBuf+_sendLen);
    if(pos==0) return -1;

    _sendLen += pos;

    uint16 wCrc = CxCrc::calcModbus(_sendBuf, _sendLen);
    CxBuffer::toBuf(&_sendBuf[_sendLen],wCrc,0);
    _sendLen+=sizeof(uint16);

    if(sendNow)
    {
        writeData(_sendBuf,_sendLen);
    }
    else
    {
        vector<uchar> v1;
        for(int i=0;i<_sendLen;i++) v1.push_back(_sendBuf[i]);
        _sentFrms.push(v1);
        _sendEnable= true;
    }

    return _sendLen;
}


///Modbus Master---------
///Modbus Master*********
RTU_ModbusMasterProtocol::RTU_ModbusMasterProtocol()
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

RTU_ModbusMasterProtocol::~RTU_ModbusMasterProtocol()
{
}



///Modbus Slave*********
RTU_ModbusSlaveProtocol::RTU_ModbusSlaveProtocol()
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

RTU_ModbusSlaveProtocol::~RTU_ModbusSlaveProtocol()
{
}
///Modbus Slave---------


