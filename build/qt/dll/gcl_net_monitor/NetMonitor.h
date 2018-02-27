#ifndef IP_MONITOR_H
#define IP_MONITOR_H

//#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <iostream>

#define DEF_BUF_SIZE 1024
#define IP_HEADER_SIZE 20
#define ICMP_HEADER_SIZE 12

typedef struct _ICMP_HEADER
{
	BYTE bType;        //类型
	BYTE bCode;        //代码
	USHORT nCheckSum;  //校验各
	USHORT nId;        //进程ID
	USHORT nSequence;  //序号
	UINT nTimeStamp;   //时间
}ICMP_HEADER, *PICMP_HEADER;

USHORT GetCheckSum(LPBYTE lpBuff, DWORD dwSize)
{
	DWORD dwCheckSum = 0;
	USHORT* lpWord = (USHORT*)lpBuff;
	while (dwSize > 1)
	{
		dwCheckSum += *lpWord++;
		dwSize -= 2;
	}
	if (dwSize == 1)
		dwCheckSum += *((LPBYTE)lpBuff);
	dwCheckSum = (dwCheckSum >> 16) + (dwCheckSum & 0XFFFF);
	return (USHORT)(~dwCheckSum);
}

BOOL Ping(const char* lpDestIP)
{
	SOCKADDR_IN DestSockAddr;
	DestSockAddr.sin_family = AF_INET;
	DestSockAddr.sin_addr.S_un.S_addr = inet_addr(lpDestIP);
	DestSockAddr.sin_port = htons(0);
	char ICMPPack[ICMP_HEADER_SIZE] = { 0 };
	PICMP_HEADER pICMPHeader = (PICMP_HEADER)ICMPPack;
	pICMPHeader->bType = 8;
	pICMPHeader->bCode = 0;
	pICMPHeader->nId = (USHORT)::GetCurrentProcessId();
	pICMPHeader->nCheckSum = 0;
	pICMPHeader->nTimeStamp = 0;

	WORD version = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(version, &wsaData) != 0)
	{
        printf("WSAStartup error\n");
		return FALSE;
	}
	SOCKET s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	int nTime = 1000;
	int ret = ::setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTime, sizeof(nTime));
	char szRcvBuff[DEF_BUF_SIZE];
	SOCKADDR_IN SourceSockAddr;
	for (int i = 0; i<4; i++)
	{
		pICMPHeader->nCheckSum = 0;
		pICMPHeader->nSequence = i;
		pICMPHeader->nTimeStamp = ::GetTickCount();
		pICMPHeader->nCheckSum = GetCheckSum((LPBYTE)(ICMPPack), ICMP_HEADER_SIZE);
		int nRet = ::sendto(s, ICMPPack, ICMP_HEADER_SIZE, 0, (SOCKADDR*)&DestSockAddr, sizeof(DestSockAddr));
		if (nRet == SOCKET_ERROR)
		{
            printf("send error.\n");
			return FALSE;
		}
		int nLen = sizeof(SOCKADDR_IN);
		if (nRet == SOCKET_ERROR)
		{
            int nError = ::WSAGetLastError();
            printf("Recv Error:%d.\n", nError);
			return FALSE;
		}
		nRet = ::recvfrom(s, szRcvBuff, DEF_BUF_SIZE, 0, (SOCKADDR*)&SourceSockAddr, &nLen);
		if (nRet == SOCKET_ERROR)
		{
			return FALSE;
		}

        PICMP_HEADER pRcvHeader = (PICMP_HEADER)(szRcvBuff + IP_HEADER_SIZE);
        int nTime = ::GetTickCount() - pRcvHeader->nTimeStamp;
        printf("从目标地址传回: %s bytes=%d time=%dms\n", inet_ntoa(SourceSockAddr.sin_addr), nRet, nTime);
		::Sleep(1000);
	}
	closesocket(s);
	WSACleanup();
	return TRUE;
}

#endif /*IP_MONITOR_H*/
