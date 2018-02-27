#ifndef TRANS_PROTOCOL_H
#define TRANS_PROTOCOL_H

#include <vdi/protocol_common.h>

class GCL_Trans_Protocol : public ProtocolCommon
{
public:
    GCL_Trans_Protocol();

    ~GCL_Trans_Protocol();
    //协议类型
    inline const GString& typeSimpleName() const { return CS_ProtocolTypeSimpleName_TRANS; }
    //存储参数
    void toContext(CxISetSkv& context) const ;
    //加载参数
    void fromContext(const CxIGetSkv& context);

public:
    int sendCustomData(int iSourceId, int iCommandId, std::map<std::string, std::string> sParams, int iDataLength, const char * pData);

public:
    //定时器
   void protocolTimerTimeOut(const CxTimer* oTimer);
    //数据处理
    void protocolReceivedData(const uchar* pData, int iLength);
    //启动
    void protocolAfterStart();
    //停止
    void protocolAfterStop();
    //任务
    void doTask(GCCommon::GCDataElement dt);
    //校时
    void clock();
private:
 };


class GCL_Trans_ProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(GCL_Trans_ProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_TRANS;
    }

    ProtocolBase* createObject(){
        return new GCL_Trans_Protocol();
    }
};

#endif // HDID_PROTOCOL_ICS_20130126_H
