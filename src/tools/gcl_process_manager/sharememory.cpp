#include "sharememory.h"

ShareMemory::ShareMemory()
{
    lp = NULL;
    memorySize = 0;
}

ShareMemory::~ShareMemory()
{
    realese();
}

bool ShareMemory::create(const char* memoryName, size_t size)
{
    if(memoryName == NULL || *memoryName == 0)
        return false;

    lp = NULL;
    memorySize = 0;

    if(size)
    {
        m_hFileHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, memoryName);
    }
    else
    {
        m_hFileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, memoryName);
    }

    if(m_hFileHandle == NULL || m_hFileHandle == INVALID_HANDLE_VALUE)
    {
        cout << "CreateFileMapping failed: " << GetLastError() << endl;
        return false;
    }

    lp = MapViewOfFile(m_hFileHandle, FILE_MAP_ALL_ACCESS, 0, 0, size);

    if(lp)
    {
        memorySize = size;
        VirtualLock(lp, memorySize);
        return true;
    }
    else
    {
        cout << "MapViewOfFile failed : " << endl;
        return false;
    }
}

void ShareMemory::realese()
{
    if(lp) {
            VirtualUnlock(lp, memorySize);
            UnmapViewOfFile(lp);
            CloseHandle(m_hFileHandle);
            lp = NULL;
            m_hFileHandle = INVALID_HANDLE_VALUE;
    }
    else
    {
            free(lp);
            lp = NULL;
    }
    memorySize = 0;
}

LPVOID ShareMemory::getmp()
{
    return lp;
}
