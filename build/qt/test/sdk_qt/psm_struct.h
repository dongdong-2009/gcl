#ifndef PSM_STRUCT_H
#define PSM_STRUCT_H

//字节对齐按4个字节方式来对齐
#pragma pack(push,4)


//注意！！！注意！！！注意！！！
//错了，但为了兼容以前（20141231前），不改了，不改了，不改了；错就错吧！！！^_^
//gcd_address_vqt_intText1_t           //1）、错在vqt的顺序，应该是 value quality datetime； 2）、错在 value 应该是 char value[2]
//gcd_firstAddress_vqt_intText1_t      //1）、错在vqt的顺序，应该是 value quality datetime； 2）、错在 value 应该是 char value[2]
//gcd_address_vqt_realText15_t         //1）、错在vqt的顺序，应该是 value quality datetime；
//gcd_firstAddress_vqt_realText15_t    //1）、错在vqt的顺序，应该是 value quality datetime；
//gcd_address_vqt_string128_t          //1）、错在vqt的顺序，应该是 value quality datetime；
//gcd_firstAddress_vqt_string128_t     //1）、错在vqt的顺序，应该是 value quality datetime；



//*--- 数据帧结构 --- 分 6 段
//*1 head
//    uint    head;

//*2 front
//    ushort  version;

//*3 dataLength
//    ushort  dataLength;

//*4 body
struct psm_packet_body_t
{
    int  frameType; // send received calcer : IEC104
    int  sourceOriginal;
    int  sourceAddress;
    int  resFrame;
    int  targetAddress;
    //4(head) + 2 + 2 + 10 + 2
    int  controlWord; // short controlWord; short frameNo;
    //  + 24 + 2(crc)
    int  command;
    int  reason;
    int  resCommand;
    int  container;
    int  paramType;
    int  paramCount;
};

//*5 data
//  char data[dataLength]

//*6 crc
//    int  crc;


#define CI_psm_packet_max (3980)
#define CI_PSM_PACKET_DATA (4000)
const int c_psm_head = 0x5aa55aa5;
const int CIPsmControlCode_Initiactive = 0x4000;
const int CIPsmControlCode_Passivity = 0x8000;
const int CIPsmControlCode_NACK = 0xC000;

#define ci_psm_packet_default_size (1024*2)
#define ci_psm_fix_size (sizeof(psm_packet_body_t) - 16)
#define ci_psm_packet_count(type_t, i_max_size_packet) ( (i_max_size_packet - ci_psm_fix_size) / sizeof(type_t) )
#define ci_psm_packet_default_count(type_t) ci_psm_packet_count(type_t, ci_psm_packet_default_size)

struct PsmAttach
{
    int reason;
    int containerId;
    int sourceId;
    int targetId;
    int tag;

    PsmAttach(int iReason = 0, int iContainerId = 0, int iSourceUserID = 0 , int iTargetUserID = 0, int iTag = 0) :
        reason(iReason),containerId(iContainerId),sourceId(iSourceUserID),targetId(iTargetUserID),tag(iTag) {}
//    inline PsmAttach(const PsmAttach &o) : reason(o.reason), containerId(o.containerId), sourceUserId(o.sourceUserId), targetUserID(o.targetUserID), tag(o.tag){ }
//    inline PsmAttach &operator=(const PsmAttach &other)
//    {
//        if (this != &other)
//        {
//            reason = other.reason;
//            containerId = other.containerId;
//            sourceUserId = other.sourceUserId;
//            targetUserID = other.targetUserID;
//            tag = other.tag;
//        }
//        return *this;
//    }
};


//***注意 attention : 结构体字节对齐，如果有 int ， Sizeof是4的位数；如果有 long long ， Sizeof是8的位数；
//*sdk端的实时区、遥信、遥测调用的
//quality （质量）： 1 表示好的、有效；0 表示无效
//datatime （采集时间）: 系统当前时间 now（当前毫秒数)
//value : 当前值
//address : 传送数据结构体每个都要带地址信息
//firstaddress : 表示以地址起始、结束的方式，后面传送数据结构体时就不用带地址信息
//param_type :


struct gcd_firstaddress_t
{
    int addressBegin;
    int addressEnd;
};

//*yx
#define ci_yx_address_vqt_intText1_t (0x01010205)
typedef struct gcd_address_vqt_intText1_t
{
    int address;
    char value[8];
    int quality;
    long long datetime;
    static int getType()
    {
        return ci_yx_address_vqt_intText1_t;
    }
} gcd_yx_t;

#define ci_yx_address_int32_vqt_t (0x01010203)
struct gcd_address_int32_vqt_t {
    int address;
    int value;
    int quality;
    long long datetime;
    static int getType() {
        return ci_yx_address_int32_vqt_t;
    }
};

#define ci_yx_firstAddress_vqt_intText1_t (0x01020205)
struct gcd_firstAddress_vqt_intText1_t
{
    char value[8];
    int quality;
    long long datetime;
    static int getType()
    {
        return ci_yx_firstAddress_vqt_intText1_t;
    }
};



//*yc
//real_text 只有15个有效字符
#define ci_yc_address_vqt_intText1_t (0x0101021E)
typedef struct gcd_address_vqt_realText15_t
{
    int address;
    char value[16];
    int quality;
    long long datetime;
    static int getType()
    {
        return ci_yc_address_vqt_intText1_t;
    }
} gcd_yc_t;

#define ci_yc_address_double_vqt_t (0x0101021C)
struct gcd_address_double_vqt_t {
    int address;
    double value;
    int quality;
    long long datetime;
    static int getType() {
        return ci_yc_address_double_vqt_t;
    }
};

#define ci_yc_firstAddress_vqt_realText15_t (0x0102021E)
struct gcd_firstAddress_vqt_realText15_t
{
    char value[16];
    int quality;
    long long datetime;
    static int getType()
    {
        return ci_yc_firstAddress_vqt_realText15_t;
    }
};



//*yw
//real_text 只有127个有效字符
#define ci_yw_address_vqt_string128_t (0x0101022F)
typedef struct gcd_address_vqt_string128_t
{
    int address;
    char value[128];
    int quality;
    long long datetime;
    static int getType()
    {
        return ci_yw_address_vqt_string128_t;
    }
} gcd_ym_t, gcd_str_t;

#define ci_yw_firstAddress_vqt_string128_t (0x0102022F)
struct gcd_firstAddress_vqt_string128_t
{
    char value[128];
    int quality;
    long long datetime;
    static int getType()
    {
        return ci_yw_firstAddress_vqt_string128_t;
    }
};


#pragma pack(pop)

#endif // PSM_STRUCT_H
