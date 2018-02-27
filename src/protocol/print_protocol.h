#ifndef PRINT_PROTOCOL_H
#define PRINT_PROTOCOL_H


#include <vdi/protocol_base.h>


class PrintProtocol : public ProtocolBase
{

public:

    PrintProtocol();

    ~PrintProtocol();

    inline const std::string& typeSimpleName() const { return CS_ProtocolTypeSimpleName_Print; }

    void toContext(CxISetSkv& context) const ;

    void fromContext(const CxIGetSkv& context);

    void describeSelf(CxDescribeUnit* oDescribeUnit) const;

    int print(const char * sText, int iLength) { return writeData(sText, iLength); }

protected:
    void toReport(std::vector<std::string>& sReports) const;

    void protocolTimerTimeOut(const CxTimer* oTimer);

    void protocolAfterStart();

    void protocolAfterStop();

    void protocolReceivedData(const uchar* pData, int iLength);

private:


};

class PrintProtocolFactory : public ProtocolFactoryBase
{

    GM_SINGLETON_DECLARATION(PrintProtocolFactory)

public:
    std::string factoryName(){
        return CS_ProtocolTypeSimpleName_Print;
    }

    ProtocolBase* createObject(){
        return new PrintProtocol();
    }

};

#endif // PRINT_PROTOCOL_H
