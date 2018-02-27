#ifndef PLC_FINS_HEAD_H
#define PLC_FINS_HEAD_H


#include <ccxx/ccxx.h>


class GCLFINSHead
{
public:
    GCLFINSHead();
    ~GCLFINSHead();
    GCLFINSHead& operator=(const GCLFINSHead &o)
    {
        _lead  = o._lead;
        _len   = o._len;
        _cmd   = o._cmd;
        _err   = o._err;
        return *this;
    }
    void clear();


    GCLFINSHead(uint len,uint cmd,uint err );

    void setLen(uint val){_len = val;}
    inline uint length(){return _len;}

    void setCmd(uint val){_cmd = val;}
    inline uint cmd(){return _cmd;}

    void setError(uint val){_err = val;}
    inline uint error(){return _err;}

    //From缓冲区
    bool fromBuff(uchar *pBuf,int nLen);
    //To缓冲区
    int toBuff(uchar *pBuf);
    //获取长度
    inline int size()
    {
        return  sizeof(uint)*4;
    }
protected:
    uint _lead;
    uint _len;
    uint _cmd;
    uint _err;
protected:
//    uint _byteSeq; //字节序
    bool _bInverse;

};
#endif // PLC_FINS_HEAD_H
