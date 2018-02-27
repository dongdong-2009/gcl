#include "modbus_tcp_master.h"

ModBusTcpMaster::ModBusTcpMaster()
{
}
//IsMaster : 1 , 2 , 3, 4
//Receive : yx , yc , yk
void ModBusTcpMaster::ModbusReceiveModelDeal()
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
