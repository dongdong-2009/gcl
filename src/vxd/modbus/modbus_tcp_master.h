#ifndef MODBUS_TCP_MASTER_H
#define MODBUS_TCP_MASTER_H

#include "modbus_tcp_common.h"

class ModBusTcpMaster
{
public:
    ModBusTcpMaster();
private:
    void ModbusReceiveModelDeal();
};

#endif // MODBUS_TCP_MASTER_H
