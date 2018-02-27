#include "fileinfo_sharememory.h"


#include <common/gcl_resource.h>

using namespace std;

static fn_void_int_t fn_fileInfoShareMemoryAfterLoadCallBack = 0;
static fn_void_int_t fn_fileInfoShareMemoryBeforeCloseCallBack = 0;

static FileInfoShareMemory f_fileInfoShareMemory;

static bool f_bCanWriteFileInfoShareMemory = false;


FileInfoShareMemory *FileInfoShareMemory::memoryManager()
{
    return & f_fileInfoShareMemory;
}

void FileInfoShareMemory::open(bool bCanWrite, int iCheckInterval)
{
    f_bCanWriteFileInfoShareMemory = bCanWrite;
    timerCheckSharememory(iCheckInterval);
    CxTimerManager::startTimer(FileInfoShareMemory::timerCheckSharememory, 200); //程序退出时自动 stopTimer
}

void FileInfoShareMemory::timerCheckSharememory(int iInterval)
{
    if (f_fileInfoShareMemory.status() == ci_sharememory_status_invalid)
    {
        string sFileInfoMapName = CxApplication::findConfig(CS_SectionShareMemory, CS_EntryFileInfoMapName, std::string());
        if (sFileInfoMapName.size()>0)
        {
            int iFileInfoCount = f_bCanWriteFileInfoShareMemory ? CxApplication::findConfig(CS_SectionShareMemory, CS_EntryFileInfoCount, int(0)) : 0;
            f_fileInfoShareMemory.load(sFileInfoMapName, iFileInfoCount);
            if (f_fileInfoShareMemory.status() == ci_sharememory_status_good)
            {
                if (fn_fileInfoShareMemoryAfterLoadCallBack) fn_fileInfoShareMemoryAfterLoadCallBack(0);
            }
        }
    }
    else if (f_fileInfoShareMemory.status() == ci_sharememory_status_closing)
    {
        if (fn_fileInfoShareMemoryBeforeCloseCallBack) fn_fileInfoShareMemoryBeforeCloseCallBack(0);
        f_fileInfoShareMemory.shutdown();
    }
}

void FileInfoShareMemory::setBeforeCloseCallback(fn_void_int_t fn)
{
    fn_fileInfoShareMemoryBeforeCloseCallBack = fn;
}

void FileInfoShareMemory::setAfterLoadCallback(fn_void_int_t fn)
{
    fn_fileInfoShareMemoryAfterLoadCallBack = fn;
}

void FileInfoShareMemory::close()
{
    if (! f_fileInfoShareMemory.isReadOnly())
    {
        f_fileInfoShareMemory.setStatus(ci_sharememory_status_closing);
        CxApplication::waiting(250);
    }

    f_fileInfoShareMemory.shutdown();
}
