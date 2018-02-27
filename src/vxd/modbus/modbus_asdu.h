#ifndef MODBUS_ASDU_H
#define MODBUS_ASDU_H

#include <common/gcl.h>
#include <ccxx/ccxx.h>

class ModBusASDU
{
public:
    ModBusASDU(){}
    ~ModBusASDU(){}
    ModBusASDU& operator=(const ModBusASDU &o)
    {
        _addr   = o._addr;
        _fc   = o._fc;
        _len       = o._len;
        memcpy(_buf,o._buf,GCL_LEN_DT);
        return *this;
    }
    ModBusASDU(uint8 addr,uint8 fc,uint16 len,uchar *pBuf)
    {
        setValue(addr,fc,len,pBuf);
    }

    //From缓冲区
    bool fromBuff(uchar *pBuf,int nLen);
    //To缓冲区
    int toBuff(uchar *pBuf);
    //设置值
    void setValue(uint8 addr,uint8 fc,uint16 len,uchar *pBuf)
    {
        setAddr(addr);
        setFC(fc);
        setBuf(len,pBuf);
    }
    void setValue(uint16 len,uchar *pBuf)
    {
        if(len>2)
        {
            setAddr(pBuf[0]);
            setFC(pBuf[1]);
            setBuf(len-2,pBuf+2);
        }
    }
    void setValue(std::vector<char> &vBuf)
    {
        if(vBuf.size()>2)
        {
            uchar *pData = (uchar*)vBuf.data();
            setAddr(pData[0]);
            setFC(pData[1]);
            setBuf(vBuf.size()-2,pData+2);
        }
    }
    //获取长度
    int size(){return sizeof(uint8)*2+_len;}
public:
    inline uint8 addr(){return _addr;}
    inline void setAddr(uint8 val){_addr = val;}

    inline uint8 fc(){return _fc;}
    inline void setFC(uint8 val){_fc = val;}

    inline uint16 len(){return _len;}
    inline uchar* buf()
    {
        return &_buf[0];
    }
    void setBuf(uint16 len,uchar *pBuf)
    {
        _len = len>=GCL_LEN_DT?GCL_LEN_DT:len;

        if(pBuf==NULL) return;

        memset(_buf,0,GCL_LEN_DT);
        if(_len>0) memcpy(_buf,pBuf,_len);
     }
protected:
    uint8 _addr;
    uint8 _fc;
    uint16 _len;
    uint8 _buf[GCL_LEN_DT];
};
#endif // MODBUS_ASDU_H
