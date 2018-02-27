#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H


#include <ccxx/ccxx.h>


class ProcessInfo
{
public:
    ProcessInfo();
    ~ProcessInfo();

    static std::string getCurrentProcessInfo();

};

#endif // PROCESS_INFO_H
