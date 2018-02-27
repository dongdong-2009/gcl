#include "plc_fins_addr.h"

GCLFINSAddr::GCLFINSAddr()
{
}

GCLFINSAddr::~GCLFINSAddr()
{
}

GCLFINSAddr::GCLFINSAddr(uchar NA, uchar node, uchar em)
{
    setValue(NA,node,em);
}

bool GCLFINSAddr::fromBuff(uchar *pBuf, int nLen)
{
    if(pBuf==NULL || nLen<size()) return false;

    _NA = *pBuf++;
    _node = *pBuf++;
    _em = *pBuf;
    return true;
}

uchar GCLFINSAddr::toBuff(uchar *pBuf)
{
    if(pBuf==NULL) return 0;
    *pBuf++ = _NA;
    *pBuf++ = _node;
    *pBuf   = _em;
    return size();
}

void GCLFINSAddr::setValue(uchar NA, uchar node, uchar em)
{
    _NA = NA;
    _node = node;
    _em = em;
}

