#include "rfid_ywg_frame.h"

#include <ccxx/cxcrc.h>
#include <ccxx/cxbuffer.h>


RfidYWGFrame::RfidYWGFrame()
{
    memset(_sendBuff,0,RFID_LEN_BUF);
    _sendLen = 0;
    _dataLen = 0;
    _cmd = 0;
    _lead = 0;
    _crc = 0;
    memset(_data,0,RFID_LEN_BUF);

}

bool RfidYWGFrame::setUpFrame(uchar *pBuf, uint16 &len)
{
    if(pBuf==NULL) return false;

    _sendLen = toBuff(_sendBuff);


    bool bRet = false;
    int pos = 0;
    if(_sendLen>1)
    {
        int i=0;
        pBuf[pos++] = _sendBuff[i];
        for(i=1;i<_sendLen;i++)
        {
            if(_sendBuff[i]!=0x55 && _sendBuff[i]!=0x56)
            {
                pBuf[pos++] = _sendBuff[i];
            }
            else if(_sendBuff[i]==0x55)
            {
                pBuf[pos++] = 0x56;
                pBuf[pos++] = 0x56;
            }
            else
            {
                pBuf[pos++] = 0x56;
                pBuf[pos++] = 0x57;
            }
        }
        bRet = true;
    }

    len = pos;
    return bRet;
}

bool RfidYWGFrame::fromBuff(uchar *pBuf, int nLen)
{
    if(pBuf==NULL||nLen<RFID_YWG_FRM_LEN_MIN) return false;

    int pos = 0;

    _lead  = pBuf[pos++];

    _dataLen = 0;
    CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_dataLen,false);
    pos+=sizeof(uint16);

    //长度错误
    if(_dataLen>RFID_LEN_BUF) return false;
    if((pos+_dataLen)>(nLen-2)) return false;

    _cmd = pBuf[pos++];

    if(_dataLen>1 && _dataLen<RFID_LEN_BUF)
    {
        memset(_data,0,RFID_LEN_BUF);
        memcpy(_data,pBuf+pos,_dataLen-1);
        pos += (_dataLen-1);
    }
    _crc = 0;
    CxBuffer::fromBuf(pBuf+pos,sizeof(uint16),_crc,false);
    pos+=sizeof(uint16);
    return true;
}

int RfidYWGFrame::toBuff(uchar *pBuf)
{
    if(pBuf==NULL) return 0;
    int pos = 0;

    pBuf[pos++] = RFID_YWG_FRM_LEADFlAG;
    pBuf[pos++] = (_dataLen>>8);
    pBuf[pos++] = (_dataLen&0xFF);
    pBuf[pos++] = _cmd;
    if(_dataLen>1 && _dataLen<RFID_LEN_BUF )
    {
        memcpy(pBuf+pos,_data,_dataLen-1);
        pos += (_dataLen-1);
    }

    _crc = (uint16)CxCrc::calculate_crc16_2(pBuf+1,(uchar)(_dataLen+2));

    pBuf[pos++] = (_crc>>8);
    pBuf[pos++] = (_crc&0xFF);
    return getSize();
}

int RfidYWGFrame::getSize()
{
    return _dataLen+RFID_YWG_FRM_LEN_FIX;
}

void RfidYWGFrame::setData(uchar cmd, uint8 *pData, uint8 len)
{
    _cmd = cmd;
    if(pData!=NULL && len>0 && len<RFID_LEN_BUF)
    {
        memset(_data,0,RFID_LEN_BUF);
        memcpy(_data,pData,len);
        _dataLen = len+1;
    }
}

uchar *RfidYWGFrame::getData(int &len)
{
    len = 0;
    if(_dataLen>1)
    {
        len = _dataLen - 1;
        return &_data[0];
    }
    return NULL;
}
/////////////////////////////////////////////////////////////////////////
RfidYWGReadGM::RfidYWGReadGM()
{
    clear();
}

void RfidYWGReadGM::clear()
{
    _antenna=0; //天线
    _epcLen=0;
    memset(_epc,0,RFID_LEN_TMP);
    _tidLen=0;
    memset(_tid,0,RFID_LEN_TMP);
    _userDataLen=0;
    memset(_userData,0,RFID_LEN_TMP);
}
//缓冲区处理
//指令长度	命令字	操作成功	天线端口	EPC长度	EPC数据	TID长度	TID数据	用户数据区长度	标签用户数据	CRC
//(2字节)	97H	    00H	   01H～04H	1字节	(N字节)	1字节	(N字节)	   1字节	    (N字节)	   (2字节)
//只处理天线端口到标签用户数据
bool RfidYWGReadGM::fromBuff(uchar *pBuf, int nLen)
{
   clear();

   if(pBuf==NULL || nLen < 4) return false;

   int pos = 0;

   _antenna =  pBuf[pos++];
   _epcLen  =  pBuf[pos++];
   if(_epcLen>0)
   {
       if((pos+_epcLen) < nLen && _epcLen<RFID_LEN_TMP)
       {
          memcpy(_epc,pBuf+pos,_epcLen);
          pos += _epcLen;
       }
       else
       {
           return false;
       }
   }

   _tidLen  =  pBuf[pos++];
   if(_tidLen>0)
   {
       if((pos+_tidLen) < nLen && _tidLen<RFID_LEN_TMP)
       {
          memcpy(_tid,pBuf+pos,_tidLen);
          pos += _tidLen;
       }
       else
       {
           return false;
       }
   }

   _userDataLen  =  pBuf[pos++];
   if(_userDataLen>0)
   {
       //must <=
       if((pos+_userDataLen) <= nLen && _userDataLen<RFID_LEN_TMP)
       {
          memcpy(_userData,pBuf+pos,_userDataLen);
          pos += _userDataLen;
       }
       else
       {
           return false;
       }
   }

   return true;
}
