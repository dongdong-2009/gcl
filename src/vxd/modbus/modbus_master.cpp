#include "modbus_master.h"

ModBusMaster::ModBusMaster()
{
}

/// 做主站，读数据，响应帧处理
void ModBusMaster::dealReadCoilStatus()
{
    dealReadStatus(_readCoilStatusIsYx, _readCoilStatusSlaveAddress, _readCoilStatusAddress, _readCoilStatusCount);
}

void ModBusMaster::dealReadInputStatus()
{
    dealReadStatus(_readInputStatusIsYx, _readInputStatusSlaveAddress, _readInputStatusAddress, _readInputStatusCount);
}

void ModBusMaster::dealReadStatus(bool bIsYx, int iReadSlaveAddress, int iReadAddress, int iReadCount)
{
    if (_recSlaveAddress != iReadSlaveAddress) return;
    int iAddress = iReadAddress;
    std::map<int, int> addressValues;

    for (int i = 0; i < _recDealData.size(); ++i)
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
    if (bIsYx)
    {
        _yxRecTable->receiveAddressValues(iReadSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}

void ModBusMaster::dealReadHoldingRegisters()
{
    if(_typeHodingRegister==GCL_VT_VL)//变长
    {
        if(_headLast.sessionFlag()!=_headRev.sessionFlag()) return;

        if((_asduRev.fc()&0x80)>0) //返回吗错误
        {
            outProtocolDebug()<<"error:"<<_asduRev.fc()<<cxEndLine;
            return;
        }

        uchar *pData = _asduLast.buf();
        uint16 beginAddr = 0;
        CxBuffer::fromBuf(pData,sizeof(uint16),beginAddr,0);

        beginAddr += 1; //规约规定

        map<string,string> mapPram;
        mapPram[CS_Modbus_StationID] = CxString::format("%d",_asduRev.addr());
        mapPram[CS_Modbus_BeginAddr] = CxString::format("%d",beginAddr);
        mapPram[CS_Modbus_FC] = CxString::format("%d",_asduRev.fc());

        uchar *pBuf = _recDealData.data();
        GclLuaVxd::dealCustomData(terminalId(),3,mapPram,_recDealData.size(),(char*)pBuf,0);
    }
    else
    {
        dealReadRegisters(_readHoldingRegistersSlaveAddress, _readHoldingRegistersAddress, _readHoldingRegistersCount,_typeHodingRegister);
    }
}

void ModBusMaster::dealReadInputRegisters()
{
    if(_typeHodingRegister==GCL_VT_VL)//变长
    {
        uchar *pBuf = _recDealData.data();
        //        GclLuaVxd::dealBuffer();
    }
    else
    {
        dealReadRegisters(_readInputRegistersSlaveAddress, _readInputRegistersAddress, _readInputRegistersCount,_typeInputRegister);
    }
}
void ModBusMaster::dealReadRegisters(int iReadSlaveAddress, int iReadAddress, int iReadCount, int mode)
{
    if (_recSlaveAddress != iReadSlaveAddress)
    {
        outProtocolPrompt()<<"DealReadRegisters:" << iReadSlaveAddress << (int)_recSlaveAddress<<cxEndLine;
        return;
    }
    int iAddress = iReadAddress;
    short nValue = 0;
    double dValue = 0;
    std::map<int, double> addressValues;

    switch(mode)
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
            CxBuffer::fromBuf(pBuf+i*sizeof(float),sizeof(float),fVal,1);
            outProtocolDebug()<<"YC Value:" << iAddress << fVal<<cxEndLine;
            addressValues[iAddress] = (double)fVal;
            iAddress ++;
        }
    }
        break;
    case 1://短整形2 short
    {
        uchar *pBuf = _recDealData.data();
        int count = _recDealData.size()/sizeof(short);
        for(int i=0;i<count;++i)
        {
            float fVal = 0.0f;
            CxBuffer::fromBuf(pBuf+i*sizeof(short),sizeof(short),nValue,false);
            outProtocolDebug()<<"YC Value:" << iAddress << fVal<<cxEndLine;
            addressValues[iAddress] = (double)nValue;
            iAddress ++;
        }
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
