#include "iccard_protocol.h"


#include "iccard_manager.h"


IccardProtocolFactory* IccardProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<IccardProtocolFactory>();


IccardProtocol::IccardProtocol()
{
    memset( _pCardbuffer, 0, sizeof(_pCardbuffer) );
    _cardLen = 0;
}

IccardProtocol::~IccardProtocol()
{
}

void IccardProtocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
}

void IccardProtocol::toContext(CxISetSkv& context) const
{
    ProtocolBase::toContext(context);
}

void IccardProtocol::fromContext(const CxIGetSkv& context)
{
    ProtocolBase::fromContext(context);
}

void IccardProtocol::toReport(std::vector<std::string> &sReports) const
{
    //    sReports.append(std::string("Control Word : %1"            ,_controlWord          ));
}

void IccardProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
    //    oDescribeUnit->SetFuncDescribeValuesCalc(DescribeValuesCalc);
    //    CxDescribeInteger* oControlWordDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CSEntryCdtControlWord          ); oControlWordDescribe->InitValue(_controlWord                    ); oControlWordDescribe->SetIsHexDisplay(true)    ; oControlWordDescribe->SetRang(0, 255)    ;
}

void IccardProtocol::protocolAfterStart()
{
}

void IccardProtocol::protocolAfterStop()
{
}

void IccardProtocol::protocolReceivedData(const uchar *pData, int iLength)
{
    memcpy( _pCardbuffer+_cardLen, pData, iLength );
    _cardLen += iLength;
    DealData();
}

void IccardProtocol::DealData()
{
    uchar desBuffer[100] = { 0 };
    for( int i=0; i<_cardLen-2; i++ )
    {
        if( ((uchar)_pCardbuffer[i] == 0x0D) && ((uchar)_pCardbuffer[i+1] == 0x0A) )
        {
            istoASCs( desBuffer, (uchar *)_pCardbuffer, _cardLen );
            std::string response( (char *)desBuffer );
            outProtocolDebug() << response << " Done" << cxEndLine;
            memset( _pCardbuffer, 0, sizeof(_pCardbuffer) );
            _cardLen = 0;
            _lastIccard = response;
            _lastTimeUpdateIccard = CxTime::currentSystemTime();
            IccardManager::setLastIccard(_lastIccard);
        }
    }
}

//convert EBCD into string
//example: 0x79,0x85,0x00,0x99--->79850099
unsigned char IccardProtocol::istoASCs(uchar* desBuffer, uchar* srcBuffer,int len)
{
    int i;
    char ch;
    for(i=0;i<len;i++)
    {
        ch = srcBuffer[i] >> 4;
        if((ch >= 0)&&(ch <= 9))
        {
            desBuffer[2*i] = ch +'0';
        }
        else if((ch >=10)&&(ch <= 15))
        {
            desBuffer[2*i] = ch + 55;
        }
        else
            desBuffer[2*i] = '*';
        ch = srcBuffer[i] & 0x0F;
        if((ch >= 0)&&(ch <= 9))
        {
            desBuffer[2*i+1] = ch +'0';
        }
        else if((ch >=10)&&(ch <= 15))
        {
            desBuffer[2*i+1] = ch + 55;
        }
        else
            desBuffer[2*i+1] = '*';
    }
    desBuffer[2*i] = '\0';
    return 0;
}
