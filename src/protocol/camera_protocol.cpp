#include "camera_protocol.h"


#include <time.h>


CameraProtocolFactory* CameraProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<CameraProtocolFactory>();


#define CAMERA_SEND 1
#define CAMERA_RECV 2
#define CAMERA_CLEAN_BUF 3
#define CAMERA_TAKE_PIC 4
#define CAMERA_GET_LEN 5
#define CAMERA_SAVE_PIC 6
#define CAMERA_RESET 7
#define CAMERA_WAITING 8

CameraProtocol::CameraProtocol()
{
    _lastTimeUpdatePictruePath = 0;

    memset( _picture, 0, sizeof(_picture) );
    _iBegin = 0;
    _iEnd = 0;
    _nLength = 0;
    _beginaddr = "00000000";
    _endaddr = "00000000";
}

CameraProtocol::~CameraProtocol()
{
}

void CameraProtocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
}

void CameraProtocol::toContext(CxISetSkv& context) const
{
    ProtocolBase::toContext(context);
}

void CameraProtocol::fromContext(const CxIGetSkv& context)
{
    ProtocolBase::fromContext(context);
}

void CameraProtocol::toReport(std::vector<std::string> &sReports) const
{
    //    sReports.append(std::string("Control Word : %1"            ,_controlWord          ));
}

void CameraProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
    //    oDescribeUnit->SetFuncDescribeValuesCalc(DescribeValuesCalc);
    //    CxDescribeInteger* oControlWordDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CSEntryCdtControlWord          ); oControlWordDescribe->InitValue(_controlWord                    ); oControlWordDescribe->SetIsHexDisplay(true)    ; oControlWordDescribe->SetRang(0, 255)    ;
}

int CameraProtocol::takePhoto()
{
    return initCamera();
}

void CameraProtocol::protocolAfterStart()
{
}

void CameraProtocol::protocolAfterStop()
{
}

/* ---------------------------------------------------------------------------
*function: protocolReceivedData
*brief:    数据解析，
*Prameter: [in] pData:数据缓冲区
*          [in] iLength:数据缓冲区长度
*return：   无
*author:    wangchengtao
*date:      2014/05/14
*modify:
*need:      假设数据没有问题,应该有循环缓冲区
---------------------------------------------------------------------------*/
void CameraProtocol::protocolReceivedData(const uchar *pData, int iLength)
{
    memset(desBuffer, 0, sizeof(desBuffer));

    if( (_cameraMode == CAMERA_SAVE_PIC) ) //4096
    {
        memcpy( _picture+_nLength, (char *)pData, iLength );
        _nLength += iLength;
        if( _nLength>1000 )
        {
            for(int i=0; i<_nLength; i++ )
            {
                if( (uchar)_picture[i] == 0xFF && (uchar)_picture[i+1] == 0xD8 )
                {
                    _bPictrueHead = true;
                    _iBegin = i;
                    break;
                }
            }

            if( !_bPictrueHead )
            {
                return;
            }

            for(int i=1000; i<_nLength-1; i++ )
            {
                if( (uchar)_picture[i] == 0xFF && (uchar)_picture[i+1] == 0xD9 )
                {
                    _iEnd = i;
                    istoASCs( desBuffer, (uchar *)_picture, _nLength );
                    int iRet = SavePicture( (uchar *)_picture );
                    {
                        if( iRet == 0 )
                        {
                            outProtocolDebug() << "Take picture succeed!" << cxEndLine;
                        }
                        else
                        {
                            outProtocolDebug() << "Take picture failed!" << cxEndLine;
                        }
                    }

                    std::string srcdata( (char *)desBuffer );
                    outProtocolDebug() << srcdata << _iBegin << _iEnd << cxEndLine;
                    _cameraMode = CAMERA_RESET;
                    _nLength = 0;
                }
            }
        }
    }

    if( _cameraMode != CAMERA_SAVE_PIC ) //4096
    {
        //数据压榨
        memcpy( _picture +_nLength, (char *)pData, iLength );
        _nLength += iLength;

        //数据处理
        DealData( (uchar *)_picture , _nLength );
    }
}

/*
设置拍照图片大小指令：
560031050401001911 //320*240
7600310000

复位指令
56002600
76002600+DSP信息
*/

int CameraProtocol::initCamera()
{
    int len = 5;
    uchar cmd[256] = { 0x56,0x00,0x36,0x01,0x03 }; //Clear picture
    int iRet = this->writeData( cmd, len );
    //    Asm570::MyLoging( cmd, len, CAMERA_SEND );
    _cameraMode = CAMERA_CLEAN_BUF;
    _bPictrueHead = false;
    return iRet;
}

int CameraProtocol::SavePicture( uchar *pData )
{
    //写文件 JPEG
    FILE *fp;
    char tmp1[64] = {0};
    time_t t = time( 0 );
    memset( tmp1, 0, sizeof(tmp1) );
    strftime( tmp1, sizeof(tmp1), "CAMERA_%H_%M_%S.JPEG", localtime(&t) );
    std::string sPath = CxFileSystem::mergeFilePath(GclConfig::tempPath(),tmp1); //exe路径下的路径

    fp = fopen( sPath.c_str(),"wb" );
    if( fp == NULL )
    {
        outProtocolDebug() << "Open file failed!!" << sPath << cxEndLine;
        return -1;
    }

    for( int i=_iBegin; i<_iEnd+2; i++ )
    {
        fprintf( fp, "%c", (uchar)pData[i] );
    }

    fflush( fp );
    fclose( fp );

    _lastPictruePath = sPath;
    _lastTimeUpdatePictruePath = CxTime::currentSystemTime();

    return 0;
}

void CameraProtocol::DealData(const uchar *pData, int iLength)
{
    std::string strcommand;
    //FILE *fp;
    int len = 0;
    char buf[1024*20] = { 0 };
    uchar cmd[1024*20] = { 0 };
    uchar desBuffer[1024*20] = { 0 };
    istoASCs( desBuffer, (uchar *)pData, iLength );
    std::string response( (char *)desBuffer );

    switch( _cameraMode )
    {
    case CAMERA_CLEAN_BUF:
        if( response.compare("7600360000") )
        {
            return;
        }

        _nLength = 0;
        //        _revBuffer.Clear();
        len = 5;
        memcpy( buf, "5600360100", len*2 ); //Take picture
        StrToEBCD( buf, cmd, len*2 );
        //        Asm570::MyLoging( cmd, len, CAMERA_SEND );
        this->writeData( cmd, len );
        _cameraMode = CAMERA_TAKE_PIC;
        break;
    case CAMERA_TAKE_PIC:
        if( response.compare("7600360000") )
        {
            return;
        }

        _nLength = 0;
        //        _revBuffer.Clear();
        len = 5;
        memcpy( buf, "5600340100", len*2 ); //Get picture length
        StrToEBCD( buf, cmd, len*2 );
        //        Asm570::MyLoging( cmd, len, CAMERA_SEND );
        this->writeData( cmd, len );
        _cameraMode = CAMERA_GET_LEN;
        break;
    case CAMERA_GET_LEN:
        if( response.length() == 18 )
        {
            _beginaddr = "00000000";
            _endaddr = response.substr(response.size()-4);
        }
        else
        {
            return;
        }

        _nLength = 0;
        //        _revBuffer.Clear(); //"5600320c000a" + _beginaddr + "0000" + _endaddr + "00FF"
        strcommand =  "5600320C000A";
        strcommand.append( _beginaddr );
        strcommand.append( "0000" );
        strcommand.append( _endaddr );
        strcommand.append( "00FF" );

        outProtocolDebug() << strcommand << cxEndLine;

        memcpy( buf, strcommand.data(), strcommand.size() ); //Get picture data
        StrToEBCD( buf, cmd, len );
        //        Asm570::MyLoging( cmd, len/2, CAMERA_SEND );
        this->writeData( cmd, len/2 );
        _cameraMode = CAMERA_SAVE_PIC;
        break;
    case CAMERA_RESET:
        len = 4;
        memcpy( buf, "56002600", len*2 ); //Reset camera
        StrToEBCD( buf, cmd, len*2 );
        //        Asm570::MyLoging( cmd, len, CAMERA_SEND );
        this->writeData( cmd, len );
        _cameraMode = CAMERA_WAITING;
        _nLength = 0;
        break;
    }
}

//convert EBCD into string
//example: 0x79,0x85,0x00,0x99--->79850099
unsigned char CameraProtocol::istoASCs(uchar* desBuffer, uchar* srcBuffer,int len)
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


//convert string into EBCD
//89860099--->0x89,0x86,0x00,0x99
int CameraProtocol::StrToEBCD( char *buf, uchar *ucBuffer, int BufLen )
{
    unsigned char temp1,temp2;
    int Len=BufLen/2,i;

    for( i=0; i<Len; i++ )
    {
        temp1=buf[i*2];
        if( temp1>='a' )
            temp1=temp1 - 'a' + 10;
        if( temp1>='A' )
            temp1=temp1 - 'A' + 10;
        if( temp1>='0' )
            temp1=temp1-'0';

        temp2=buf[i*2 + 1];
        if( temp2>='a' )
            temp2=temp2 - 'a' + 10;
        if( temp2>='A' )
            temp2=temp2 - 'A' + 10;
        if( temp2>='0' )
            temp2=temp2-'0';
        ucBuffer[i]=((temp1&0x0f)<<4)|(temp2&0x0f);
    }
    return 0;
}


