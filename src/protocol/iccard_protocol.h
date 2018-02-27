#ifndef ICCARD_PROTOCOL_H
#define ICCARD_PROTOCOL_H


#include <vdi/protocol_base.h>


class IccardProtocol : public ProtocolBase
{

public:

    IccardProtocol();

    ~IccardProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Iccard; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    //返回最后获取到Iccard号的时间
    inline long long lastTimeUpdateIccard() { return _lastTimeUpdateIccard; }

    //返回最后获取到的Iccard号
    inline std::string lastIccard() { return _lastIccard ; }

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

    void DealData();

    unsigned char istoASCs(uchar* desBuffer, uchar* srcBuffer,int len);

private:
    uchar _pCardbuffer[512];
    int _cardLen;
    long long _lastTimeUpdateIccard;
    std::string _lastIccard;

};

class IccardProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(IccardProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Iccard;
    }

    ProtocolBase* createObject(){
        return new IccardProtocol();
    }
};

#endif // ICCARD_PROTOCOL_H
