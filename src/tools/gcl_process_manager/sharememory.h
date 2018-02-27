#ifndef SHAREMEMORY_H
#define SHAREMEMORY_H
#include "global.h"

using namespace std;



class ShareMemory
{
public:
    ShareMemory();

    virtual ~ShareMemory();

    bool create(const char* memoryName, size_t size);
    void realese();
    bool createevent();
    LPVOID getmp();

private:
    HANDLE m_hFileHandle;
    //char*  oProcesses;
    size_t memorySize;
    LPVOID lp;

    /*
    HANDLE hServerWriteOver;
    HANDLE hClientReadOver;
    HANDLE hServerReadOver;
    HANDLE hClientWriteOver;
    */
};


#endif // SHAREMEMORY_H
