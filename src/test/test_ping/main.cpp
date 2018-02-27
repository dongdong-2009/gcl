//    Ping.c
//
//    利用ICMP回送请求与回答报文模拟Ping命令
//
//    Step:
//    1. Open ICMP handle
//    2. Sends an IPv4 ICMP echo request and returns any echo response replies.
//    3. Display the infomation for response echo.
//    4. Close ICMP handle
//
//    Configuration before Build Solution:
//    Project->Properties->Linker->Input->Addtional Dependencies : Iphlpapi.lib Ws2_32.lib

#undef UNICODE

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>

int main( int argc, char ** argv )
{

    /* 声明并初始化变量 */
    HANDLE hIcmp = INVALID_HANDLE_VALUE;
    unsigned long ulDestIpAddr = INADDR_NONE;
    char sendBuffer [] = "Send Buffer";
    LPVOID lpReplyBuffer = NULL;
    int iReplySize = 0;
    DWORD dwTimeout = 1000;
    DWORD dwRetVal = 0;

    /* 打开ICMP句柄    */
    hIcmp = IcmpCreateFile ();
    if ( INVALID_HANDLE_VALUE == hIcmp )
    {
        printf ( "Open ICMP failure!\n" );
        return EXIT_FAILURE;
    }

    /* 验证命令行参数 */
    if ( argc != 2 )
    {
        printf ( "Usage : %s IP Address\n", argv[0] );
        IcmpCloseHandle( hIcmp );
        return EXIT_FAILURE;
    }

    ulDestIpAddr = inet_addr ( argv[1] );
    if ( *argv[1] == INADDR_NONE )
    {
        printf ( "Usage : %s IP Address\n", argv[0] );
        IcmpCloseHandle( hIcmp );
        return EXIT_FAILURE;
    }

    iReplySize = sizeof ( ICMP_ECHO_REPLY ) + sizeof ( sendBuffer );
    lpReplyBuffer = malloc ( iReplySize );

    /* 发送ICMP报文并等待回送请求 */
    dwRetVal = IcmpSendEcho ( hIcmp, ulDestIpAddr, sendBuffer, sizeof(sendBuffer),
                              NULL, lpReplyBuffer, iReplySize, dwTimeout );
    if ( 0 != dwRetVal )
    {
        // 利用一指针获取 Request Echo 的 ICMP_ECHO_REPLY 结构实例
        PICMP_ECHO_REPLY pReplyEcho = (PICMP_ECHO_REPLY) lpReplyBuffer;

        in_addr replyIpAddr;
        replyIpAddr.S_un.S_addr = pReplyEcho->Address;
        printf ( "Reply from : %s\n", inet_ntoa(replyIpAddr) );

        if ( dwRetVal > 1 )
        {
            printf ( "Retrieved %d ICMP response message.\n", dwRetVal );
            printf ( "----The infomation for the first message.----\n" );
        }
        else
        {
            printf ( "Retrieved %d ICMP reponse message.\n", dwRetVal );
            printf ( "----The information for the message.----\n" );
        }

        printf ( "The status : %ld\n", pReplyEcho->Status );
        printf ( "The reply data : %s\n", pReplyEcho->Data );
        printf ( "The Roundtrip time : %ld milliseconds.\n", pReplyEcho->RoundTripTime );
    }
    else
    {
        printf ( "ICMP send failure.\n" );
        printf ( "The IcmpSendEcho call error : %ld\n", GetLastError() );
        IcmpCloseHandle( hIcmp );
        return EXIT_FAILURE;
    }

    /* 关闭ICMP句柄 */
    IcmpCloseHandle( hIcmp );

    return EXIT_SUCCESS;

}
