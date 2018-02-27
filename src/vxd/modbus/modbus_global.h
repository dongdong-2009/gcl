#ifndef MODBUS_GLOBAL_H
#define MODBUS_GLOBAL_H

#include <ccxx/cxglobal.h>

using namespace std;

const string CS_Modbus_StationID="stationID";
const string CS_Modbus_BeginAddr="beginAddr";
const string CS_Modbus_FC="fc";
const string CS_Modbus_Count="count";

const int CI_ADDR_TYPE_GE_MODICON = 1;
const int CI_ADDR_TYPE_SIMENS     = 0;

#endif // MODBUS_GLOBAL_H
