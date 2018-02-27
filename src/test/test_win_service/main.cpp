
#include "windows.h"

SERVICE_STATUS          gSvcStatus;  //服务状态
SERVICE_STATUS_HANDLE   gSvcStatusHandle; //服务状态句柄
HANDLE                  ghSvcStopEvent = NULL;//服务停止句柄

#define SERVER_NAME    TEXT("my_server") //服务名
VOID WINAPI Server_main( DWORD, LPTSTR *); //服务入口点
void ServerReportEvent(LPTSTR szName,LPTSTR szFunction); //写Windows日志
VOID ReportSvcStatus( DWORD, DWORD, DWORD ); //服务状态和SCM交互
VOID WINAPI SvcCtrlHandler( DWORD );  //SCM回调函数
VOID ServerProgram(DWORD, LPTSTR *); //服务主程序

#define SVC_ERROR (WM_USER+0x01)
int main()
{
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { SERVER_NAME, (LPSERVICE_MAIN_FUNCTION)Server_main },
        { NULL, NULL }
    };

    if (!StartServiceCtrlDispatcher(DispatchTable))
    {
         ServerReportEvent(SERVER_NAME,TEXT("StartServiceCtrlDispatcher"));
    }
    return 0;
}

VOID WINAPI Server_main(DWORD dwArgc, LPTSTR *lpszArgv )
{
    // Register the handler function for the service
    gSvcStatusHandle = RegisterServiceCtrlHandler(SERVER_NAME,SvcCtrlHandler);

    if( gSvcStatusHandle==NULL )
    {
       ServerReportEvent(SERVER_NAME,TEXT("RegisterServiceCtrlHandler"));
        return;
    }

    // These SERVICE_STATUS members remain as set here
    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS; //只有一个单独的服务
    gSvcStatus.dwServiceSpecificExitCode = 0;

    // Report initial status to the SCM
    ReportSvcStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );

    // Perform service-specific initialization and work.
    ghSvcStopEvent = CreateEvent(
        NULL,    // default security attributes
        TRUE,    // manual reset event
        FALSE,   // not signaled
        NULL);   // no name

    if ( ghSvcStopEvent == NULL)
    {
        ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
        return;
    }

    // Report running status when initialization is complete.
    ReportSvcStatus( SERVICE_RUNNING, NO_ERROR, 0 );

    // TO_DO: Perform work until service stops.
//    ServerProgram(dwArgc, lpszArgv); //你需要的操作在此添加代码

    while(1)
    {
          // Check whether to stop the service.
      WaitForSingleObject(ghSvcStopEvent, INFINITE);
      ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
      return;
    }
}

void ServerReportEvent(LPTSTR szName,LPTSTR szFunction)
{
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    unsigned int len = sizeof(szFunction);
    TCHAR *Buffer = new TCHAR[len];

    hEventSource = RegisterEventSource(NULL, szName);

    if( NULL != hEventSource )
    {
//        StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());
//        strcpy(Buffer,szFunction);
//        wcstombs
 //       mbstowcs(Buffer,szFunction,len);

        wcscpy(Buffer,szFunction);

        lpszStrings[0] = szName;
        lpszStrings[1] = Buffer;

        ReportEvent(hEventSource,        // event log handle
            EVENTLOG_ERROR_TYPE, // event type
            0,                   // event category
            SVC_ERROR,           // event identifier
            NULL,                // no security identifier
            2,                   // size of lpszStrings array
            0,                   // no binary data
            lpszStrings,         // array of strings
            NULL);               // no binary data
        DeregisterEventSource(hEventSource);
    }
}

VOID ReportSvcStatus( DWORD dwCurrentState,
                     DWORD dwWin32ExitCode,
                     DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure.

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ( (dwCurrentState == SERVICE_RUNNING) ||
        (dwCurrentState == SERVICE_STOPPED) )
        gSvcStatus.dwCheckPoint = 0;
    else gSvcStatus.dwCheckPoint = dwCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus( gSvcStatusHandle, &gSvcStatus );
}

VOID WINAPI SvcCtrlHandler( DWORD dwCtrl )
{
    // Handle the requested control code.
  switch(dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

        // Signal the service to stop.

        SetEvent(ghSvcStopEvent);

        return;

    case SERVICE_CONTROL_INTERROGATE:
        // Fall through to send current status.
        break;

    default:
        break;
    }
    ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
}

