#include "modbus_tcp_head.h"

#include <ccxx/ccxx.h>

bool ModBusTcpHead::fromBuff(uchar *pBuf, int nLen)
{
    if((pBuf==NULL)||(nLen<size())) return false;

    int pos = 0;
    CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_sessionFlag,0);
    pos += sizeof(uint16);

    CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_protcolFlag,0);
    pos += sizeof(uint16);

    CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_asduLen,0);
    pos += sizeof(uint16);

    return true;
}

int ModBusTcpHead::toBuff(uchar *pBuf)
{
     if((pBuf==NULL)) return 0;

     int pos = 0;
     CxBuffer::toBuf(pBuf+pos,_sessionFlag,0);
     pos += sizeof(uint16);

     CxBuffer::toBuf(pBuf+pos,_protcolFlag,0);
     pos += sizeof(uint16);

     CxBuffer::toBuf(pBuf+pos,_asduLen,0);
     pos += sizeof(uint16);

     return pos;
}

