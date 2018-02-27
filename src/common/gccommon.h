
#ifndef GCCOMMON_H
#define GCCOMMON_H

#include <ccxx/ccxx.h>

#include "gcl.h"

using namespace std;
namespace GCCommon
{

class GCUnNameObj
{
public:
    GCUnNameObj()
    {
       _desc = GCL_STRING_EMPTY;
    }

    ~GCUnNameObj()
    {
    }

    GCUnNameObj& operator=(const GCUnNameObj &o)
    {
        _desc     = o._desc;
        return *this;
    }

    inline void setDesc(GString szVal)
    {
        _desc = szVal;
    }

    inline GString desc()
    {
        return _desc;
    }

protected:
    GString _desc;

};

class GCNameObj: public GCUnNameObj
{
public:
    GCNameObj()
    {
        clear();
    }

    ~GCNameObj()
    {
    }
    void clear()
    {
        _id   = 0;
        _name = GCL_STRING_EMPTY;
        _res  = 0;
    }

    GCNameObj& operator=(const GCNameObj &o) {
        GCUnNameObj::operator =(o);
        _name     = o._name;
        _id       = o._id;
        _res      = o._res;
        return *this;
    }

    GCNameObj(GString szName,GString szDesc,uint32 Id)
    {
        _name = szName;
        _desc = szDesc;
        _id   = Id;
        _res  = 0;
    }

    GCNameObj(GString szName,GString szDesc,uint32 Id,int Res)
    {
        _name = szName;
        _desc = szDesc;
        _id   = Id;
        _res  = Res;
    }

    inline void setName(GString szVal){_name = szVal;}
    inline GString name(){return _name;}

    inline void setId(uint32 Val){_id = Val;}
    inline uint32 id(){return _id;}

    inline void setRes(int val){_res = val;}
    inline int res(){return _res;}

protected:
    GString _name;
    uint32 _id;
    int    _res;//保留

};

//通用对象
class GCCommonObj: public GCNameObj
{
public:
    GCCommonObj()
    {
        clear();
    }

    ~GCCommonObj()
    {
    }

    GCCommonObj& operator=(const GCCommonObj &o)
    {
        GCNameObj::operator =(o);
        _ext    = o._ext;
        _code   = o._code;
        _type   = o._type;
        _level  = o._level;
        return *this;
    }

    void clear()
    {
        GCNameObj::clear();
        _code  = 0;
        _type  = 0;
        _level = 0;
        _ext = GCL_STRING_EMPTY;
    }

public:
   inline void setBase(uint32 id,GString name,GString desc,int res=0)
   {
       _name = name;
       _desc = desc;
       _id   = id;
       _res  = res;
   }

   inline void setBaseEx(GString ext,uint32 code,uint32 type,uint32 level)
   {
       _ext    = ext;
       _code   = code;
       _type   = type;
       _level  = level;
   }

    inline void setExt(GString val){_ext = val;}
    inline GString ext(){return _ext;}

    inline void setPram(GString val){_ext = val;}
    inline GString pram(){return _ext;}

    inline void setCode(uint32 val){_code = val;}
    inline uint32 code(){return _code;}

    inline void setType(uint32 val){_type = val;}
    inline uint32 type(){return _type;}

    inline void setLevel(uint32 val){_level = val;}
    inline uint32 level(){return _level;}

    inline void setProp(GString val){_ext = val;}
    inline GString prop(){return _ext;}

protected:
    GString _ext;   //扩展属性
    uint32 _code;  //内码
    uint32 _type;  //类型
    uint32 _level; //等级

};


//数据单元
class GCDataElement
{
public:
    GCDataElement()
    {
        clear();
    }

    ~GCDataElement()
    {
    }

    GCDataElement& operator=(const GCDataElement &o)
    {
        _id   = o._id;
        _type = o._type;
        _pram  = o._pram;
        _len  = o._len;
        memcpy(_buff,o._buff,GCL_LEN_DATA_EM);
        return *this;
    }

    GCDataElement(uint32 id,uint32 type,uint32 pram,uint32 len,char *buffer=NULL)
    {
       setValue(id,type,pram,len,buffer);
    }

    GCDataElement(uint32 pram,uint32 len,char *buffer=NULL)
    {
        setValue(0,0,pram,len,buffer);
    }

    void clear()
    {
        _lead = GCL_MSG_FLAG_LEAD;
        _id   = 0;
        _type = 0;
        _pram  = 0;
        _len  = 0;
        memset(_buff,0,GCL_LEN_DATA_EM);
    }
    void setValue(uint32 id,uint32 type,uint32 pram,uint32 len,char *buffer=NULL)
    {
        _id   = id;
        _type = type;
        _pram  = pram;
        _len  = len;
        memset(_buff,0,GCL_LEN_DATA_EM);
        if(_len>GCL_LEN_DATA_EM)_len = GCL_LEN_DATA_EM;
        if(buffer!=NULL && _len>0)
        {
            memcpy(_buff,buffer,_len);
        }        
    }

    inline size_t fixLenth()
    {
        return sizeof(uint32)*4+sizeof(uint16)+sizeof(uchar);
    }

    int size()
    {
        return _len+fixLenth();
    }

    int maxSize()
    {
        return GCL_LEN_DATA_EM+fixLenth();
    }

    bool toBuffer(char *pBuf)
    {
        if(pBuf == NULL) return false;

        int pos = 0;

        _lead = GCL_MSG_FLAG_LEAD;
        memcpy(pBuf+pos,&_lead,sizeof(uint32));
        pos+=sizeof(uint32);

        memcpy(pBuf+pos,&_id,sizeof(uint32));
        pos+=sizeof(uint32);

        memcpy(pBuf+pos,&_type,sizeof(uint32));
        pos+=sizeof(uint32);

        memcpy(pBuf+pos,&_pram,sizeof(uint32));
        pos+=sizeof(uint32);

        memcpy(pBuf+pos,&_len,sizeof(uint16));
        pos+=sizeof(uint16);

        if(_len>GCL_LEN_DATA_EM) return false;

        if(_len>0 && _len<=GCL_LEN_DATA_EM)
        {
            memcpy(pBuf+pos,_buff,_len);
            pos+=_len;
        }
        //先不算校验
        pBuf[pos] =0;  //CxCrc::calcsum8(pBuf,pos);

        return true;
    }

    bool fromBuffer(char *pBuf,uint32 len)
    {        
        if(pBuf == NULL) return false;
        //长度错误
        if(len<fixLenth()) return false;

        int pos = 0;

        memcpy(&_lead,pBuf+pos,sizeof(uint32));
        if(_lead!=GCL_MSG_FLAG_LEAD) return false;

        pos+=sizeof(uint32);

        memcpy(&_id,pBuf+pos,sizeof(uint32));
        pos+=sizeof(uint32);

        memcpy(&_type,pBuf+pos,sizeof(uint32));
        pos+=sizeof(uint32);

        memcpy(&_pram,pBuf+pos,sizeof(uint32));
        pos+=sizeof(uint32);

        memcpy(&_len,pBuf+pos,sizeof(uint16));
        pos+=sizeof(uint16);

        if(_len>(len-fixLenth())) return false;
        if(_len>GCL_LEN_DATA_EM)return false;

        if(_len>0)
        {
            memcpy(_buff,pBuf+pos,_len);
            pos+=_len;
        }
         //先不算校验
        _crc = pBuf[pos];
        return true;
    }

    inline void setId(uint32 Val){_id = Val;}
    inline uint32 id(){return _id;}

    inline char *buffer(){return &_buff[0];}
    inline uint32 dataLenth(){return _len;}

    void setBuffer(uint32 len,char *buffer)
    {
        _len  = len;
        memset(_buff,0,GCL_LEN_DATA_EM);
        if(_len>GCL_LEN_DATA_EM)_len = GCL_LEN_DATA_EM;
        if(buffer!=NULL && _len>0)
        {
            memcpy(_buff,buffer,_len);
        }
    }
    inline void setPram(uint32 Val){_pram = Val;}
    inline uint32 pram(){return _pram;}


    inline void setType(uint32 Val){_type = Val;}
    inline uint32 type(){return _type;}

    GString toString()
    {
        return CxString::format("id=%d,type=0x%08x,parm=0x%08x,len=%d",_id,_type,_pram,_len);
    }

    GString toStringEx()
    {
        if(_len>0)
        {
           return CxString::format("id=%d,type=0x%08x,parm=0x%08x,len=%d,buf=%s",_id,_type,_pram,_len,CxString::toHexstring(_buff,_len).data());
        }
        else
        {
            return CxString::format("id=%d,type=0x%08x,parm=0x%08x,len=%d",_id,_type,_pram,_len);
        }
    }

public:
    uint32 _lead; //引导
    uint32 _id;
    uint32 _type;
    uint32 _pram;
    uint16 _len;
    char   _buff[GCL_LEN_DATA_EM];
    uchar  _crc;
};


class GCMessage
{
public:
    GCMessage()
    {
        _info.clear();
        _tmTag = 0;
        _type = 0;
        _res  = 0;
    }
    ~GCMessage(){}
public:
    GCMessage& operator=(const GCMessage &o)
    {
        _info   = o._info;
        _tmTag  = o._tmTag;
        _type   = o._type;
        _res    = o._res;
        return *this;
    }
public:
    GCMessage(uint64 lTime,GString szInfo,uint32 nType)
    {
        _info   = szInfo;
        _tmTag  = lTime;
        _type   = nType;
        _res    = 0;
    }
    GCMessage(uint64 lTime,GString szInfo,uint32 nType,int Res)
    {
        _info   = szInfo;
        _tmTag  = lTime;
        _type   = nType;
        _res    = Res;
    }
    inline void setInfo(GString szVal){_info = szVal;}
    inline GString info(){return _info;}

    inline void setTimeTag(uint64 lVal){_tmTag = lVal;}
    inline uint64 timeTag(){return _tmTag;}


    inline void setType(uint32 nVal){_type = nVal;}
    inline uint32 type(){return _type;}

    inline void setRes(int val){_res = val;}
    inline int res(){return _res;}

public:
    GString _info;
    uint32 _type;
    uint64 _tmTag;
    int    _res;//保留
};

class GCStringInfo
{
public:
    GCStringInfo()
    {
        _value.clear();
        _pos = 0;
        _len  = 0;
    }
    ~GCStringInfo(){}
public:
    GCStringInfo& operator=(const GCStringInfo &o)
    {
        _value   = o._value;
        _pos     = o._pos;
        _len     = o._len;
        return *this;
    }
public:
    GCStringInfo(GString szValue,uint32 nPos,uint32 nLen)
    {
        _value   = szValue;
        _pos     = nPos;
        _len     = nLen;
    }
    inline void setValue(GString szVal){_value = szVal;}
    inline GString value(){return _value;}

    inline void setPos(uint32 nVal){_pos = nVal;}
    inline uint32 pos(){return _pos;}

    inline void setLen(uint32 nVal){_len = nVal;}
    inline int len(){return _len;}

public:
    GString _value;
    uint32 _pos;
    uint32 _len;//保留
};

//数据单元
class GCData
{
public:
    GCData()
    {
        clear();
    }
    ~GCData(){clear();}
public:
    GCData& operator=(const GCData &o)
    {
        _tm1970S = o._tm1970S;
        _tmMS  = o._tmMS;
        _level = o._level;
        _type = o._type;
        _len  = o._len;
        memcpy(_buff,o._buff,GCL_LEN_DATA_EM);
        return *this;
    }
    GCData& operator=(const GCDataElement &o)
    {
        _tm1970S = CxTime::currentSepoch();
        _tmMS = 0;
        _level = 0;
        _type = o._type;
        _len  = o._len;
        memset(_buff,0,GCL_LEN_DATA_EM);
        if(_len>0)memcpy(_buff,o._buff,_len);

        return *this;
    }
public:
    GCData(uint16 level,int type,uint16 len,const char *buffer=NULL)
    {
        _tm1970S = CxTime::currentSepoch();
        _tmMS = 0;
        _level = level;
        _type = type;
        _len  = len;
        memset(_buff,0,GCL_LEN_DATA_EM);
        if(_len>GCL_LEN_DATA_EM)_len = GCL_LEN_DATA_EM;
        if(buffer!=NULL && _len>0)
        {
            memcpy(_buff,buffer,_len);
        }
    }
    void clear()
    {
        _tm1970S = 0;
        _tmMS = 0;
        _level = 0;
        _type = 0;
        _len  = 0;
        memset(_buff,0,GCL_LEN_DATA_EM);
    }

    inline uint16 fixLenth(){return sizeof(uint16)*4+sizeof(uint32);}

    int size()
    {
        return _len+fixLenth();
    }
    int maxSize()
    {
        return GCL_LEN_DATA_EM+fixLenth();
    }
    bool toBuffer(char *pBuf)
    {
        if(pBuf == NULL) return false;
        int pos = 0;

        uint32* pd = (uint32*)&pBuf[pos];
        *pd  =  _tm1970S;
        uint16* pw = (uint16*)&pBuf[pos+sizeof(uint32)];
        *pw++ = _tmMS;
        *pw++ = _level;
        *pw++ = _type;
        *pw++ = _len;
        pos+= fixLenth();
        if(_len>0)
        {
            memcpy(pBuf+pos,_buff,_len);
        }
        return true;
    }
    bool fromBuffer(char *pBuf,uint16 len)
    {
        if(pBuf == NULL) return false;
        //长度错误
        if(len<fixLenth()) return false;

        int pos = 0;
        uint32* pd = (uint32*)&pBuf[pos];
        _tm1970S = *pd;
        uint16* pw = (uint16*)&pBuf[pos+sizeof(uint32)];

        _tmMS = *pw++;
        _level    = *pw++;
        _type = *pw++;
        _len  = *pw++;

        if(_len>(len-fixLenth())) return false;

        if(_len>0)
        {
            memcpy(_buff,pBuf+fixLenth(),_len);
        }
        return true;
    }
    inline char *buffer(){return &_buff[0];}
    inline uint16 dataLenth(){return _len;}

    void setBuffer(uint16 len,char *buffer)
    {
        _len  = len;
        memset(_buff,0,GCL_LEN_DATA_EM);
        if(_len>GCL_LEN_DATA_EM)_len = GCL_LEN_DATA_EM;
        if(buffer!=NULL && _len>0)
        {
            memcpy(_buff,buffer,_len);
        }
    }

    inline void setType(uint16 Val){_type = Val;}
    inline uint16 type(){return _type;}

    inline uint32 tmTag(){return _tm1970S;}
    inline void setTmTag(uint32 val){_tm1970S = val;}

    inline void setLevel(uint16 Val){_level = Val;}
    inline uint16 level(){return _level;}


public:
    uint32 _tm1970S;//s
    uint16 _tmMS;
    uint16 _level;
    uint16 _type;   //类型
    uint16 _len;    //长度
    char   _buff[GCL_LEN_DATA_EM];
};
//基本数据
class  GCBaseObj : public GCNameObj
{
public:
    GCBaseObj()
    {
        clear();
    }
    ~GCBaseObj(){clear();}

    GCBaseObj(string sId,uint32 id,string name,string desc,int type,string user,long dt,string res0,int res1)
    {
        _sId = sId;
        _id  = id;
        _name = name;
        _desc = desc;
        _type = type;
        _user = user;
        _dt   = dt;
        _res0 = res0;
        _res  = res1;
    }
public:
    GCBaseObj& operator=(const GCBaseObj &o)
    {
        GCNameObj::operator =(o);
        _sId  = o._sId;
        _res0 = o._res0;
        _user = o._user;
        _type = o._type;
        _dt   = o._dt;
        return *this;
    }
    inline string sId(){return _sId;}
    inline void setSId(string value){_sId = value;}

    inline string res0(){return _res0;}
    inline void setRes0(string value){_res0 = value;}

    inline string user(){return _user;}
    inline void setUser(string value){_user = value;}

    inline int type(){return _type;}
    inline void setType(int value){_type = value;;}

    inline long dt(){return _dt;}
    inline void setDt(long value){_dt = value;}
public:
    void clear()
    {
        GCNameObj::clear();
        _sId.clear();
        _res0.clear();
        _user.clear();
        _type = 0;
        _dt = 0;
    }
protected:
    string _sId; //ID (字符串)
    string _res0;//保留(字符串)
    int    _type;//类型
    string _user;//用户
    long   _dt; //时间

};


//安全队列
typedef CxSafeQueue<GCCommon::GCDataElement> GCDataEmQueue;
typedef CxSafeQueue<GCCommon::GCNameObj>     GCNameObjQueue;
typedef CxSafeQueue<GCCommon::GCMessage>     GCMessageQueue;
typedef CxSafeQueue<GCCommon::GCData>        GCDataQueue;
typedef CxSafeQueue<GCCommon::GCCommonObj>   GCObjQueue;
typedef CxSafeQueue<GCCommon::GCBaseObj>     GCBaseObjQueue;


typedef std::vector<GCCommon::GCCommonObj>    GCComObjVector;
typedef std::vector<GCCommon::GCData>         GCDataVector;
typedef std::vector<GCCommon::GCMessage>      GCMsgVector;
typedef std::vector<GCCommon::GCDataElement>  GCDataEmVector;
typedef std::vector<GCCommon::GCNameObj>      GCNameObjVector;
typedef std::vector<GCCommon::GCBaseObj>      GCBaseObjVector;
}

#endif // GCCOMMON_H
