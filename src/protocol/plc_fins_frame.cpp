#include "plc_fins_frame.h"

GCLFINSFrame::GCLFINSFrame() //Head belong to Frame, Design mode
{
   _bInverse = false;
   _RSV = 0;
   _SID = 0; //++ per time
}

bool GCLFINSFrame::fromBuff(uchar *pBuf, int nLen)
{
    if(pBuf==NULL) return false;
    if(nLen<getMinSize())return false;

    if(!GCLFINSHead::fromBuff(pBuf,nLen))return false;
    int pos = GCLFINSHead::size();

    _ICF  = pBuf[pos++];
    _RSV  = pBuf[pos++];
    _GCT  = pBuf[pos++];

    if(!_dstAddr.fromBuff(pBuf+pos,nLen-pos))return false;
    pos += _dstAddr.size();

    if(!_srcAddr.fromBuff(pBuf+pos,nLen-pos))return false;
    pos += _srcAddr.size();

    _SID = pBuf[pos++];

    _MRC = pBuf[pos++];
    _SRC = pBuf[pos++];

    if((_ICF&0xC0)==ASM570_FINS_RESP) //响应
    {
        if(!CxBuffer::fromBuf(pBuf+pos,nLen-pos,_endCode,_bInverse))return false;
        pos +=sizeof(uint16);
    }

    //计算数据域长度
    _dataLen = nLen-pos; //

    if(_dataLen>0)
    {
        if(_dataLen<=ASM570_FINS_DATA_LEN_MAX)
        {
           memset(_data,0,ASM570_FINS_DATA_LEN_MAX);
           memcpy(_data,pBuf+pos,_dataLen);
           pos += _dataLen;
        }
    }

    return true;
}

int GCLFINSFrame::toBuff(uchar *pBuf)
{
    if(pBuf==NULL) return 0;
    int pos = 0;

    pos += GCLFINSHead::toBuff(pBuf);
    pBuf[pos++] = _ICF;
    pBuf[pos++] = _RSV;
    pBuf[pos++] = _GCT;

    pos += _dstAddr.toBuff(pBuf+pos);
    pos += _srcAddr.toBuff(pBuf+pos);

    pBuf[pos++] = _SID;
    pBuf[pos++] = _MRC;
    pBuf[pos++] = _SRC;

    if(_dataLen>0)
    {
        if(_dataLen<=ASM570_FINS_DATA_LEN_MAX)
        {
           memcpy(pBuf+pos,_data,_dataLen);
           pos+=_dataLen;
        }
    }
    return getSize();
}

int GCLFINSFrame::getSize()
{
    return getMinSize()+_dataLen;
}

