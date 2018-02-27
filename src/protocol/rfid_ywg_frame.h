#ifndef RFID_YWG_FRAME_H
#define RFID_YWG_FRAME_H


#include "rfid_ywg_global.h"

#include <ccxx/cxglobal.h>


class RfidYWGReadGM
{
public:
    RfidYWGReadGM();
    //清除
    void clear();
    //From缓冲区
    bool fromBuff(uchar *pBuf,int nLen);

    inline uint8 antenna(){return _antenna;}
    inline uint8 epcLen(){return _epcLen;}
    inline uchar* epc(){return &_epc[0];}
    inline uint8 tidLen(){return _tidLen;}
    inline uchar* tid(){return &_tid[0];}
    inline uint8  userDataLen(){return _userDataLen;}
    inline uchar* userDta(){return &_userData[0];}

private:
    uint8   _antenna; //天线
    uint8   _epcLen;
    uchar   _epc[RFID_LEN_TMP];
    uint8   _tidLen;
    uchar   _tid[RFID_LEN_TMP];
    uint8   _userDataLen;
    uchar   _userData[RFID_LEN_TMP];

};


class RfidYWGFrame
{
public:
    RfidYWGFrame();

public:
    bool setUpFrame(uchar *pBuf, uint16 &len);
    //From缓冲区
    bool fromBuff(uchar *pBuf,int nLen);
    //To缓冲区
    int toBuff(uchar *pBuf);
    //获取长度
    int getSize();
public:
    //
    inline int getCmd(){return (int)_cmd;}
    void setData(uchar cmd,uint8 *pData,uint8 len);
    uchar *getData(int &len);

private:
   uint8  _sendBuff[RFID_LEN_BUF];
   uint16 _sendLen;
private:
   uchar   _lead;
   uint16 _dataLen;
   uchar   _cmd;
   uint8  _data[RFID_LEN_BUF];
   uint16 _crc;
};

#endif // RFID_YWG_FRAME_H
