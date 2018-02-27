#ifndef PLC_FINS_ADDR_H
#define PLC_FINS_ADDR_H


#include <ccxx/ccxx.h>


class GCLFINSAddr
{
public:
    GCLFINSAddr();
    ~GCLFINSAddr();

    GCLFINSAddr& operator=(const GCLFINSAddr &o)
    {
        _NA   = o._NA;
        _node = o._node;
        _em   = o._em;
        return *this;
    }
    GCLFINSAddr(uchar NA,uchar node,uchar em);
    //From缓冲区
    bool fromBuff(uchar *pBuf,int nLen);
    //To缓冲区
    uchar toBuff(uchar *pBuf);
    //设置值
    void setValue(uchar NA,uchar node,uchar em);
    //获取长度
    static int size(){return uchar(3);}
public:
    inline uchar NA(){return _NA;}
    inline void setNA(uchar val){_NA = val;}

    inline uchar node(){return _node;}
    inline void setNode(uchar val){_node = val;}

    inline uchar em(){return _em;}
    inline void setEm(uchar val){_em = val;}
private:
   uchar _NA;//网络地址
   uchar _node;//节点号
   uchar _em; //单元号
};

#endif // PLC_FINS_ADDR_H
