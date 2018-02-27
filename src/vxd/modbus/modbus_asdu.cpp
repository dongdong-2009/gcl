#include "modbus_asdu.h"

bool ModBusASDU::fromBuff(uchar *pBuf, int nLen)
{
    if((pBuf==NULL) || (nLen<3)) return false;

    int pos = 0;
    _addr = pBuf[pos++]; //no *pBuf++
    _fc   = pBuf[pos++];

    setBuf(nLen-pos,pBuf+pos);

    return true;
}

int ModBusASDU::toBuff(uchar *pBuf)
{
    if(pBuf==NULL) return 0;

    int pos = 0;

    pBuf[pos++] = _addr; //no *pBuf++
    pBuf[pos++] = _fc;
    if(_len>0)
    {
        memcpy(pBuf+pos,_buf,_len);
    }
    return _len+pos;
}
