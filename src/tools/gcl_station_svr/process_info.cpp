#include "process_info.h"

ProcessInfo::ProcessInfo()
{

}

ProcessInfo::~ProcessInfo()
{

}

std::string ProcessInfo::getCurrentProcessInfo()
{
    return std::string();
}



// ProcessMemory.cpp : 定义控制台应用程序的入口点。
//



//ProcessMemory.cpp
//用法：
//    argv[1]是子进程的包含路径的完整可执行文件名
//    ProcessMemory notepad 06. 07.//编译： 08.
//    cl ProcessMemory.cpp   09.
//功能：
//    获取子进程的内存使用情况 12.

#undef UNICODE

#include <stdio.h>
#include <windows.h>
#include <Psapi.h>
#include <iostream>
#include <string>
using namespace std;

//#pragma comment(lib, "psapi.lib")

/*
typedef struct _PROCESS_MEMORY_COUNTERS {
  DWORD cb;
  DWORD PageFaultCount;
  SIZE_T PeakWorkingSetSize; //峰值内存使用
  SIZE_T WorkingSetSize; //内存使用
  SIZE_T QuotaPeakPagedPoolUsage;
  SIZE_T QuotaPagedPoolUsage;
  SIZE_T QuotaPeakNonPagedPoolUsage;
  SIZE_T QuotaNonPagedPoolUsage;
  SIZE_T PagefileUsage; //虚拟内存使用
  SIZE_T PeakPagefileUsage; //峰值虚拟内存使用
} PROCESS_MEMORY_COUNTERS,
*PPROCESS_MEMORY_COUNTERS;


*/

//提升进程权限
bool AdjustPurview()
{
    TOKEN_PRIVILEGES TokenPrivileges;
    BOOL bRet;
    HANDLE hToken;

    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &TokenPrivileges.Privileges[0].Luid);
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);

    TokenPrivileges.PrivilegeCount = 1;
    TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    bRet = AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, 0, NULL, NULL);

    CloseHandle(hToken);
    return bRet == TRUE;
}


void PrintProcessNameAndID( DWORD processID )
{
    char szProcessName[MAX_PATH] = "........";

    AdjustPurview();
    //取得进程的句柄
    HANDLE hProcess = OpenProcess(
                PROCESS_QUERY_INFORMATION |
                PROCESS_VM_READ,
                FALSE,
                processID );
    //取得进程名称
    if ( hProcess )
    {
        //HMODULE hMod;
        //DWORD cbNeeded;
        //if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
        {
            GetModuleFileNameEx(hProcess,NULL,szProcessName,MAX_PATH);
        }

        PROCESS_MEMORY_COUNTERS pmc;

        GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc));
        //GetProcessMemoryInfo(ProcessInfo.hProcess, &pmcex, sizeof(pmcex));
        //以下各项均以KB为单位 45.
        //在任务管理器中显示为：峰值工作设置（内存）
        printf("%d/n", pmc.PeakWorkingSetSize/1024);
        //在任务管理器中显示为：工作设置（内存）
        printf("%d/n", pmc.WorkingSetSize/1024);
        printf("%d/n", pmc.QuotaPeakPagedPoolUsage/1024);
        //在任务管理器中显示为：分页池
        printf("%d/n", pmc.QuotaPagedPoolUsage/1024);
        printf("%d/n", pmc.QuotaPeakNonPagedPoolUsage/1024);
        //在任务管理器中显示为：非页面缓冲池
        printf("%d/n", pmc.QuotaNonPagedPoolUsage/1024);
        printf("%d/n", pmc.PeakPagefileUsage/1024);
        //在任务管理器中显示为：内存（专用工作集） //这是任务管理器的默认显示项！ (虚拟内存)
        printf("%d/n", pmc.PagefileUsage/1024);
    }

    //回显进程名称和ID
    printf( " %-20d%-20s", processID , szProcessName);

    CloseHandle( hProcess );
}





int main1(int argc, char* argv[])
{    //CreateProcess的第一个参数
    STARTUPINFO StartupInfo = {0};
    StartupInfo.cb = sizeof(STARTUPINFO);
    //CreateProcess的第二个参数
    PROCESS_INFORMATION ProcessInfo = {0};

    DWORD aProcesses[1024], cbNeeded, cProcesses;

    unsigned int i;

    //枚举系统进程ID列表

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return 0;
    }

    //计算进程数量

    cProcesses = cbNeeded / sizeof(DWORD);

    // 输出每个进程的名称和ID

    for ( i = 0; i < cProcesses; i++ )
    {    PrintProcessNameAndID( aProcesses[i] );
    }

    //CreateProcess(NULL, argv[1], NULL, NULL, TRUE, NULL, NULL, NULL, &StartupInfo, &ProcessInfo);
    ////作用为：子进程运行结束后，查看它的内存使用情况。
    ////    此时，所有非Peak的项均为0，所有Peak项显示运行期间的峰值
    ////如果想在子进程结束之前监控它的内存使用情况，可以将这句替换为 \
    ////    Sleep(1000);
    ////    作用为：子进程运行1000毫秒之后，查看它的内存使用情况
    //WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
    //PROCESS_MEMORY_COUNTERS pmc;
    ////PROCESS_MEMORY_COUNTERS_EX pmcex;
    //GetProcessMemoryInfo(ProcessInfo.hProcess, &pmc, sizeof(pmc));
    ////GetProcessMemoryInfo(ProcessInfo.hProcess, &pmcex, sizeof(pmcex));
    ////以下各项均以KB为单位 45.
    ////在任务管理器中显示为：峰值工作设置（内存）
    //printf("%d/n", pmc.PeakWorkingSetSize/1024);
    ////在任务管理器中显示为：工作设置（内存）
    //printf("%d/n", pmc.WorkingSetSize/1024);
    //printf("%d/n", pmc.QuotaPeakPagedPoolUsage/1024);
    ////在任务管理器中显示为：分页池
    //printf("%d/n", pmc.QuotaPagedPoolUsage/1024);
    //printf("%d/n", pmc.QuotaPeakNonPagedPoolUsage/1024);
    ////在任务管理器中显示为：非页面缓冲池
    //printf("%d/n", pmc.QuotaNonPagedPoolUsage/1024);
    //printf("%d/n", pmc.PeakPagefileUsage/1024);
    ////在任务管理器中显示为：内存（专用工作集） //这是任务管理器的默认显示项！
    //printf("%d/n", pmc.PagefileUsage/1024);
    return 0;
}
