#include "modbus_tcp.h"
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
}

ModBus_TCP_Protocol::~ModBus_TCP_Protocol()
{
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
        _headSend.setProtcolFlag(0);
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

bool ModBus_TCP_Protocol::sendBuffer()
{
    if(!_sendEnable)
    {
        //超时处理
        if((CxTime::currentMsepoch()-_tmLast)>1000)
        {
            size_t nCount = _sentFrms.size();
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

bool ModBus_TCP_Protocol::protolAfterVfy()
{
    if(_headLast.sessionFlag()==_headRev.sessionFlag()) return true;
    else return false;
}

void ModBus_TCP_Protocol::protolAfterSend()
{
    _headSend = _headRev;
    _headSend.setAsduLen(_asduSend.size());
}
///Modbus Master---------

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


