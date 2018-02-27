#ifndef PLC_FINS_FRAME_H
#define PLC_FINS_FRAME_H


#include "plc_fins_global.h"
#include "plc_fins_head.h"
#include "plc_fins_addr.h"


class GCLFINSFrame : public GCLFINSHead
{
public:
    GCLFINSFrame();
    GCLFINSFrame& operator=(const GCLFINSFrame &o)
    {
        GCLFINSHead::operator =(o); //构造一个头部
        _ICF  = o._ICF;
        _RSV  = o._RSV;
        _GCT  = o._GCT;
        _dstAddr  = o._dstAddr;
        _srcAddr  = o._srcAddr;
        _SID  = o._SID;
        _MRC  = o._MRC;
        _SRC  = o._SRC;
        _endCode = o._endCode;
        _dataLen     = o._dataLen;
        memcpy(_data,o._data,ASM570_FINS_DATA_LEN_MAX);
        return *this;
    }
    //
    void clear()
    {
        GCLFINSHead::clear();
        _RSV = 0;
        _SID = 0; //++ per time



    }

    //From缓冲区
    bool fromBuff(uchar *pBuf,int nLen);
    //To缓冲区
    int toBuff(uchar *pBuf);
    //获取长度
    int getSize();
    //设置数据
    void setData(char* pBuf,int len)
    {
        _dataLen = len>ASM570_FINS_DATA_LEN_MAX?ASM570_FINS_DATA_LEN_MAX:len;
        if(_dataLen>0 && pBuf!=NULL)
        {
            memset(_data,0,ASM570_FINS_DATA_LEN_MAX);
            memcpy(_data,pBuf,_dataLen);
        }
    }
    int getDateLen(){return _dataLen;}
    uchar *getData(){return &_data[0];}
    //获取最小长度
    int getMinSize(){return (GCLFINSHead::size()+12);}
//    int getMinSize(){return (GCLFINSHead::size()+4);} //考虑握手包

    void changeSID(){_SID++;}
    void setMS(ushort val)
    {
        _MRC = val>>8;
        _SRC = val&0xFF;
    }
public:
    inline uchar ICF(){return _ICF;}
    void setICF(uchar val){_ICF = val;}

    inline uchar GCT(){return _GCT;}
    void setGCT(uchar val){_GCT = val;}

    inline GCLFINSAddr dstAddr(){return _dstAddr;}
    void setDstAddr(GCLFINSAddr &addr){_dstAddr = addr;}

    inline GCLFINSAddr srcAddr(){return _srcAddr;}
    void setSrcAddr(GCLFINSAddr &addr){_srcAddr = addr;}

    inline uchar SID(){return _SID;}
    void setSID(uchar val){_SID = val;}

    inline uchar MRC(){return _MRC;}
    void setMRC(uchar val){_MRC = val;}

    inline uchar SRC(){return _SRC;}
    void setSRC(uchar val){_SRC = val;}

    inline ushort endCode(){return _endCode;}
    void setEndCode(ushort val){_endCode = val;}
private:
    uchar _ICF; //指令or响应
    uchar _RSV; //系统保留
    uchar _GCT; //网关允许数目
    GCLFINSAddr _dstAddr;//目标地址
    GCLFINSAddr _srcAddr;//源地址
    uchar _SID;       //服务标志号

    uchar _MRC; //主命令
    uchar _SRC; //从指令

    ushort _endCode; //响应结束码
    //数据域长度
    int _dataLen;
    //数据域内容
    uchar _data[ASM570_FINS_DATA_LEN_MAX];
};
#endif // PLC_FINS_FRAME_H
