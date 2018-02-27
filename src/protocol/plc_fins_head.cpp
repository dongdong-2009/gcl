#include "plc_fins_head.h"

#include "plc_fins_global.h"

GCLFINSHead::GCLFINSHead()
{
    clear();
}

GCLFINSHead::~GCLFINSHead()
{
}

void GCLFINSHead::clear()
{
    _lead = ASM570_FINS_FRM_LEADFlAG;
    _len = 0;
    _cmd = 0;
    _err = 0;
    _bInverse = false;
}

GCLFINSHead::GCLFINSHead(uint len, uint cmd, uint err)
{
    _lead = ASM570_FINS_FRM_LEADFlAG; //0x46494e53
    _len = len;
    _cmd = cmd;
    _err = err;
    _bInverse = false;
}

bool GCLFINSHead::fromBuff(uchar *pBuf, int nLen)
{
    if(pBuf==NULL) return false;
    if(nLen<size())return false;

    int pos = 0;
    //Lead
    if(!CxBuffer::fromBuf(pBuf+pos,nLen-pos,_lead,_bInverse))return false;
    //Len
    pos+=sizeof(uint);
    if(!CxBuffer::fromBuf(pBuf+pos,nLen-pos,_len,_bInverse))return false;
    //Cmd
    pos+=sizeof(uint);
    if(!CxBuffer::fromBuf(pBuf+pos,nLen-pos,_cmd,_bInverse))return false;
    //Err
    pos+=sizeof(uint);
    if(!CxBuffer::fromBuf(pBuf+pos,nLen-pos,_err,_bInverse))return false;

    return true;
}

int GCLFINSHead::toBuff(uchar *pBuf)
{
    if(pBuf==NULL) return 0;

    int pos = 0;
    //Lead
    pos+=CxBuffer::toBuf(pBuf+pos,_lead,_bInverse);
    //Len
    pos+=CxBuffer::toBuf(pBuf+pos,_len,_bInverse);
    //Cmd
    pos+=CxBuffer::toBuf(pBuf+pos,_cmd,_bInverse);
    //Err
    pos+=CxBuffer::toBuf(pBuf+pos,_err,_bInverse);

    return pos;
}
