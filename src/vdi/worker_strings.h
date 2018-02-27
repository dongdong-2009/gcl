#ifndef WORKER_STRINGS_H
#define WORKER_STRINGS_H

#include "protocol_global.h"


class WorkerStrings
{
public:

    static std::string Worker() { return "Worker"; }
    static std::string address() { return "address"; }
    static std::string Monsb() { return "Monsb"; }
    static std::string Ycadd() { return "Ycadd"; }
    static std::string Straw() { return "Straw"; }

    static std::string port() { return "port"; }
    static std::string stationId() { return "stationId"; }
    static std::string stationName() { return "stationName"; }

};
#endif // !defined(WORKER_STRINGS_H)
