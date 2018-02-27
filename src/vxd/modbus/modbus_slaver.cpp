#include "modbus_slaver.h"

ModBusSlaver::ModBusSlaver()
{
}
///做从站,读响应
///
void ModBusSlaver::ResponseReadCoilStatus()
{
    ResponseReadStatus(_readCoilStatusIsYx, 1);
}

void ModBusSlaver::ResponseReadInputStatus()
{
    ResponseReadStatus(_readInputStatusIsYx, 2);
}

void ModBusSlaver::ResponseReadStatus(bool bIsYx, uchar byFuncCode)
{
    if (bIsYx) {
        std::vector<uchar> data;
        data.resize(CxBuffer::count(_recResponseQuantity, 8));
        const std::vector<MonsbAddressWorker*>* oWorkers = _yxSendTable->workers();
        for (int i = 0; i < oWorkers->size(); ++i) {
            MonsbAddressWorker* oWorker = oWorkers->at(i);
            if (! oWorker->measure() || oWorker->measure()->value() != 1) continue;
            int iAddress = oWorker->address();
            if (oWorker->stationId() == _recSlaveAddress && iAddress >= _recResponseAddress && iAddress < _recResponseAddress + _recResponseQuantity)
            {
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
        data.resize(CxBuffer::count(_recResponseQuantity, 8));
        const std::vector<MonsbAddressWorker*>* oWorkers = _ykSendTable->workers();
        for (int i = 0; i < oWorkers->size(); ++i) {
            MonsbAddressWorker* oWorker = oWorkers->at(i);
            const MonsbMeasure* oMonsb = oWorker->measure();
            if (! oMonsb) continue;
            int iAddress = oWorker->address();
            if (oWorker->stationId() == _recSlaveAddress && iAddress >= _recResponseAddress && iAddress < _recResponseAddress + _recResponseQuantity) {
                MonsbControlTask* oTask = MonsbControlTaskManager::firstTask(oMonsb);
                if (! oTask) continue;
                uchar indexMod;
                int iIndex = CxBuffer::index(8, iAddress, &indexMod, _recResponseAddress);
                if (iIndex >= 0 && iIndex < data.size() && indexMod < 8) {
                    uchar value = data[ iIndex ];
                    CxBuffer::setBitTrue(&value, indexMod);
                    data[ iIndex ] = value;
                }
            }
        }
        SendModbusReadData(_recSlaveAddress, byFuncCode, data);
    }
}

void ModBusSlaver::ResponseReadHoldingRegisters()
{
    ResponseReadRegisters(3);
}

void ModBusSlaver::ResponseReadInputRegisters()
{
    ResponseReadRegisters(4);
}

void ModBusSlaver::ResponseReadRegisters(uchar byFuncCode)
{
    std::vector<uchar> data;
    data.resize(_recResponseQuantity * 2);
    const std::vector<YcaddAddressWorker*>* oWorkers = _ycSendTable->workers();
    for (int i = 0; i < oWorkers->size(); ++i) {
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
///////////////////////////////////////////////////////////////////////////
//做从站,写响应
//单线圈
void ModBusSlaver::ResponseForceSingleCoil()
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
void ModBusSlaver::ResponsePresetSingleRegister()
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
void ModBusSlaver::ResponseForceMultipleCoils()
{
    if(_yxRecTable==NULL) return;
    if(_yxRecTable->workers()==NULL || _yxRecTable->workers()->size()<1) return;

    int iSlaveAddress = _recSlaveAddress;
    int iAddress = _recForceAddress;
    std::map<int, int> addressValues;
    for (int i = 0; i < _recForceMultipleData.size(); ++i)
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
void ModBusSlaver::ResponsePresetMultipleRegisters()
{
    if(_ycRecTable==NULL) return;
    if(_ycRecTable->workers()==NULL || _ycRecTable->workers()->size()<1) return;

    int iSlaveAddress = _recSlaveAddress;
    int iAddress = _recForceAddress;
    double dValue = 0;
    std::map<int, double> addressValues;
    for (int i = 0; i < _recForceMultipleData.size(); ++i)
    {
        uchar data = _recForceMultipleData.at(i);
        if (i % 2)
        {
            dValue += data;
            addressValues[iAddress] = dValue;
            iAddress ++;
        }
        else
        {
            dValue = data << 8;
        }
    }

    if(addressValues.size()>0)
    {
        _ycRecTable->receiveAddressValues(iSlaveAddress, addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }

}
