#include "print_protocol.h"



PrintProtocolFactory* PrintProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<PrintProtocolFactory>();


PrintProtocol::PrintProtocol()
{

}

PrintProtocol::~PrintProtocol()
{

}

void PrintProtocol::protocolTimerTimeOut(const CxTimer *oTimer)
{

}

void PrintProtocol::toContext(CxISetSkv& context) const
{
    ProtocolBase::toContext(context);
}

void PrintProtocol::fromContext(const CxIGetSkv& context)
{
    ProtocolBase::fromContext(context);
}

void PrintProtocol::toReport(std::vector<std::string> &sReports) const
{
//    sReports.append(std::string("Control Word : %1"            ,_controlWord          ));
}

void PrintProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
//    oDescribeUnit->SetFuncDescribeValuesCalc(DescribeValuesCalc);
//    CxDescribeInteger* oControlWordDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CSEntryCdtControlWord          ); oControlWordDescribe->InitValue(_controlWord                    ); oControlWordDescribe->SetIsHexDisplay(true)    ; oControlWordDescribe->SetRang(0, 255)    ;
}

void PrintProtocol::protocolAfterStart()
{
}

void PrintProtocol::protocolAfterStop()
{
}

void PrintProtocol::protocolReceivedData(const uchar *pData, int iLength)
{
}
