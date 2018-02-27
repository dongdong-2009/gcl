#ifndef MODBUS_MASTER_H
#define MODBUS_MASTER_H

class ModBusMaster
{
public:
    ModBusMaster();

    //master deal
    void dealReadCoilStatus();

    void dealReadInputStatus();

    void dealReadHoldingRegisters();

    void dealReadInputRegisters();

private:
    void dealReadRegisters(int iReadSlaveAddress, int iReadAddress, int iReadCount, int mode);
    void dealReadStatus(bool bIsYx, int iReadSlaveAddress, int iReadAddress, int iReadCount);
};

#endif // MODBUS_MASTER_H
