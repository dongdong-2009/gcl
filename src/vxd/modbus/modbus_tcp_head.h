#ifndef MODBUS_TCP_HEAD_H
#define MODBUS_TCP_HEAD_H

#include <common/gcl.h>

class ModBusTcpHead
{
public:
    ModBusTcpHead() {}
    ~ModBusTcpHead(){}

    ModBusTcpHead& operator=(const ModBusTcpHead &o)
    {
        _sessionFlag   = o._sessionFlag;
        _protcolFlag   = o._protcolFlag;
        _asduLen       = o._asduLen;
        return *this;
    }
    ModBusTcpHead(uint16 session,uint16 protcol,uint16 len)
    {
        setValue(session,protcol,len);
    }

    //From缓冲区
    bool fromBuff(uchar *pBuf,int nLen);
    //To缓冲区
    int toBuff(uchar *pBuf);
    //设置值
    void setValue(uint16 session, uint16 protcol, uint16 len)
    {
        _sessionFlag = session;
        _protcolFlag = protcol;
        _asduLen = len;
    }
    //获取长度
    static int size(){return sizeof(uint16)*3;}
public:
    inline uint16 sessionFlag(){return _sessionFlag;}
    inline void setSessionFlag(uint16 val){_sessionFlag = val;}

    inline uint16 protcolFlag(){return _protcolFlag;}
    inline void setProtcolFlag(uint16 val){_protcolFlag = val;}

    inline uint16 asduLen(){return _asduLen;}
    inline void setAsduLen(uint16 val){_asduLen = val;}


protected:
    uint16 _sessionFlag; //事物标志
    uint16 _protcolFlag; //协议标志
    uint16 _asduLen;
};

#endif // MODBUS_TCP_HEAD_H
