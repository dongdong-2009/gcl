#include "rfid_protocol.h"


#include "plc_manager.h"


RfidProtocolFactory* RfidProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<RfidProtocolFactory>();


RfidProtocol::RfidProtocol()
{
    memset( _rfidBuffer, 0, sizeof(_rfidBuffer) );
    _rfidLen = 0;

    memset(_rfidTmp,0,RFID_LEN_BUF);
    _tmpLen = 0;

    memset(_rfidTmp2,0,RFID_LEN_BUF);
    _tmpLen2 = 0;

    _rfidMode = RFID_WAITING;
    _scanerInterval =2000; //not use yet!
//    _timeScaner = new CxTimer();
//    _timeScaner ->setTimerTimeOut( this );
    _started = FALSE;

   _lastTimeUpdateRfid = CxTime::currentSystemTime();

   _bSucced = false;
}

RfidProtocol::~RfidProtocol()
{
//    _timeScaner->Stop();
//    delete _timeScaner;
}

void RfidProtocol::toContext(CxISetSkv& context) const
{
    ProtocolBase::toContext(context);
}

void RfidProtocol::fromContext(const CxIGetSkv& context)
{
    ProtocolBase::fromContext(context);
}

void RfidProtocol::toReport(std::vector<std::string> &sReports) const
{
//    sReports.push_back(CxString::format("Control Word : %d", _controlWord));
}

void RfidProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
//    oDescribeUnit->SetFuncDescribeValuesCalc(DescribeValuesCalc);
    //    CxDescribeInteger* oControlWordDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CSEntryCdtControlWord          ); oControlWordDescribe->InitValue(_controlWord                    ); oControlWordDescribe->SetIsHexDisplay(true)    ; oControlWordDescribe->SetRang(0, 255)    ;
}

long long RfidProtocol::query()
{
    if ( initRFID() == 0 )
    {
        return CxTime::currentSystemTime();
    }
    else
    {
        return 0;
    }
}

void RfidProtocol::protocolAfterStart()
{
}

void RfidProtocol::protocolAfterStop()
{
}

void RfidProtocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
//    if(oTimer == _timeScaner)
//    {
//        initRFID();
//        cxDebug()   <<"initRFID.";
//    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lulianbin 20140826 begin
int RfidProtocol::initRFID()
{
    _bSucced = false;
    _lastRfid.clear();
    _lastBarcode.clear();
   //有可能为异步传输
    if(sendQueryPower()>0)return 0;
    else return -1;
}


void RfidProtocol::protocolReceivedData(const uchar *pData, int iLength)
{
    receivedProtocolDataNow();

    outProtocolDebug() << "RfidProtocol Receive: " << CxString::toHexstring(pData,iLength);

    if((_tmpLen2+iLength)<=RFID_LEN_BUF)
    {
        memcpy( _rfidTmp2+_tmpLen2, pData, iLength );
        _tmpLen2 += iLength;
    }
    else
    {
        _tmpLen2 = 0;
         memcpy( _rfidTmp2+_tmpLen2, pData, iLength );
         _tmpLen2 += iLength;
    }

    if(_tmpLen2<2) return;

    //缓冲区处理
    int i = 0;
    for(i=0;i<_tmpLen2-1;i++)
    {
        if((_rfidTmp2[i] == 0x56) && (_rfidTmp2[i+1] == 0x56))
        {
            i++;
            _rfidTmp[_tmpLen++] = 0x55;
        }
        else if((_rfidTmp2[i] == 0x56) && (_rfidTmp2[i+1] == 0x57))
        {
            i++;
            _rfidTmp[_tmpLen++] = 0x56;
        }
        else
        {
            _rfidTmp[_tmpLen++] = _rfidTmp2[i];
        }
    }
    //最后一个处理
    if(i==_tmpLen2-1)
    {
        //最后一个
        if(_rfidTmp2[i] != 0x56)
        {
            _rfidTmp[_tmpLen++] = _rfidTmp2[i];
            _tmpLen2 = 0;
            memset(_rfidTmp2,0,RFID_LEN_BUF);

        }else{
            memset(_rfidTmp2,0,RFID_LEN_BUF);
            _tmpLen2 = 1;
            _rfidTmp2[0] =  0x56;
        }
    }else{
        _tmpLen2 = 0;
        memset(_rfidTmp2,0,RFID_LEN_BUF);
    }

    dealDataEx();
}

void RfidProtocol::dealDataEx()
{
    uint16 wLen = _tmpLen;
    //寻找头部
    int nStart = FindHeader(_rfidTmp,_tmpLen);
    //未找到头
    if(nStart<0)
    {        
        return;
    }
    //帧检查
    int nFrmLen = 0;
    //帧OK
    if(CheckFrame(_rfidTmp+nStart,wLen-nStart,nFrmLen))
    {
        memset(_rfidBuffer,0,RFID_LEN_BUF);
        memcpy(_rfidBuffer,_rfidTmp+nStart,nFrmLen);
        _rfidLen = nFrmLen;
        cxDebug()   <<"RfidProtocol DealData: "<<nStart<<nFrmLen;
        dealData2();
    }
    else
    {
       return;
    }
    //多帧
    if((nStart+nFrmLen) < _tmpLen) //移除
    {
        int iRemain = _tmpLen - (nStart+nFrmLen);
        memmove(_rfidTmp, _rfidTmp+nStart+nFrmLen,iRemain);
        _tmpLen = iRemain;
        dealDataEx();
    }
    else
    {
        _tmpLen = 0;
    }
}
void RfidProtocol::dealData2()
{
    bool bRet = _recvFrm.fromBuff(_rfidBuffer,_rfidLen);

     cxDebug()   <<"RfidProtocol dealData2:"<<bRet<<_recvFrm.getCmd();

    if(!bRet) return;

    switch( _recvFrm.getCmd() )
    {
    case RFID_YWG_CMD_POEWR_QUERY: //功率查询
        dealPower();
        break;
    case RFID_YWG_CMD_GM_READ: //通用读
        dealReadGM();
        break;
    default:
        break;
    }
}
//功率查询解析
void RfidProtocol::dealPower()
{
    int len = 0;
    uchar *pData = _recvFrm.getData(len);
    if(pData==NULL || len<1) return;

    //功率查询成功
    if(pData[0] == ASM570_RFID_ERR_SUCCED)
    {
        sendReadGM();
    }
}
//通用读解析
//指令长度	命令字	操作成功	天线端口	EPC长度	EPC数据	TID长度	TID数据	用户数据区长度	标签用户数据	CRC
//(2字节)	97H	    00H	   01H～04H	1字节	(N字节)	1字节	(N字节)	   1字节	    (N字节)	   (2字节)
void RfidProtocol::dealReadGM()
{
    int len = 0;
    uchar *pData = _recvFrm.getData(len);

//    cxDebug()   <<"RfidProtocol dealReadGM start: "<<pData<<len;

    if(pData==NULL || len<1) return;

    //通用读成功
    if(pData[0] == ASM570_RFID_ERR_SUCCED)
    {
        RfidYWGReadGM readGM;
        if(readGM.fromBuff(pData+1,len-1))
        {
            _lastRfid.clear();
            _lastBarcode.clear();
            _lastEPC.clear();

            _lastBarcode="0000000000";//无效明码

            bool bRet = false;
            //TID区
            if(readGM.tidLen()==RFID_LEN_TID)
            {
               _lastRfid =  CxString::toHexstring(readGM.tid(),readGM.tidLen(),false);
               _lastRfid =  CxString::toUpper(_lastRfid);//转换为大写


               //用户区
               int userDataLen = readGM.userDataLen();
               uchar *userData = readGM.userDta();

               //解密
               if(userDataLen>=RFID_LEN_MM) //大于明码长度 目前只解密明码部分
               {
                   uchar tmp[RFID_LEN_TMP] = {0};
                   if(memcmp(userData,tmp,RFID_LEN_MM)!=0)//不为空
                   {
                       int i = 0;
                       int tidLen = readGM.tidLen();
                       uchar *tid = readGM.tid();

                       uchar decode[RFID_LEN_TMP]={0};

                       //循环解密，KEY为TID区,目前只解密明码部分
                       for(i=0;i<RFID_LEN_MM;i++)
                       {
                           decode[i] = userData[i]^tid[(i*2)%RFID_LEN_TID];
                       }
                       _lastBarcode = CxString::toHexstring(decode,RFID_LEN_MM,false);
                       _lastBarcode = CxString::toUpper(_lastBarcode); //
                   }
               }
                _lastTimeUpdateRfid = CxTime::currentSystemTime();

                _bSucced = true;

#ifdef NANCHANG_GUANGMING
                //南昌光明读取EPC区前12个字节
                _lastEPC.clear();
                if(readGM.epcLen()>=RFID_LEN_TID)
                {
                    _lastEPC  = QtCommonString::ToHexstring(readGM.epc(),RFID_LEN_TID,false);
                    _lastEPC  =  _lastEPC.toUpper();//转换为大写

                }
                _lastRfid = _lastEPC;
#endif
                 cxDebug()   <<"RfidProtocol dealReadGM: "<<_lastRfid<<_lastBarcode;
            }
        }
    }
    else //读失败
    {
        cxDebug() <<"RfidProtocol dealReadGM Fail!!!";
    }
}
//发送通用读
//指令长度	命令字	天线端口	  Q值	操作类型	TID长度	用户数据区起始地址	用户数据区长度	 CRC
//(2字节)	97H	   01H～04H	（1字节）	00/01	(1字节)	EVB	            1字节	     (2字节)
int RfidProtocol::sendReadGM()
{
    memset(_sendBuf,0,RFID_LEN_BUF);
    int pos = 0;

    _sendBuf[pos++] = 0x01; //天线编号
    _sendBuf[pos++] = 0x00;//Q值
    _sendBuf[pos++] = 0x00;//操作类型
    _sendBuf[pos++] = 0x06;//TID长度 12 双字
    _sendBuf[pos++] = 0x00; // EVB
    _sendBuf[pos++] = 0x08; // 用户数据区长度 16 双字
    _sendFrm.setData(RFID_YWG_CMD_GM_READ,_sendBuf,pos);
    return setUpFrame();
}
//发送功率查询
//上位机查询系统参数指令:
//指令长度	命令字	参数类型	保留域	CRC
//(2字节)	62H	   (1字节)	(1字节)	(2字节)
int RfidProtocol::sendQueryPower()
{
    memset(_sendBuf,0,RFID_LEN_BUF);
    int pos = 0;
    _sendBuf[pos++] = 0x03; //功率查询
    _sendBuf[pos++] = 0x00;//保留

    _sendFrm.setData(RFID_YWG_CMD_POEWR_QUERY,_sendBuf,pos);

    return setUpFrame();
}
//关闭
int RfidProtocol::sendPoweroff()
{
    _sendFrm.setData(RFID_YWG_CMD_POEWR_OFF,NULL,0);
    return setUpFrame();
}

//组帧
int RfidProtocol::setUpFrame()
{
    _sendLen = 0;
    memset(_sendBuf,0,RFID_LEN_BUF);
    if(_sendFrm.setUpFrame(&_sendBuf[0],_sendLen))
    {
        cxDebug()   <<"RfidProtocol send: "<< CxString::toHexstring(_sendBuf,_sendLen);
        return writeData(_sendBuf,_sendLen); //发送数据
    }
    return -1;
}
//寻找头
int RfidProtocol::FindHeader(uchar *pData, int iLength)
{
    if((pData==NULL)||(iLength<RFID_YWG_FRM_LEN_MIN )) //6
    {
        return -1;
    }

    int nFind = -1;
    for(int i=0;i<iLength-3;i++) //
    {
        //规则 起始字符 0x55 长度>0 &&<0x55
        if((pData[i]==RFID_YWG_FRM_LEADFlAG) && (pData[i+1]==0) && (pData[i+2]>0) && (pData[i+2]<0x55)) //0x55
        {
            nFind = i;
            break;
        }
    }
    return nFind;
}
//帧检查
bool RfidProtocol::CheckFrame(uchar *pData, int iLength,int &nFrmLen)
{
    nFrmLen = 0;
    if((pData==NULL)||(iLength<RFID_YWG_FRM_LEN_MIN)) //16
    {
        return false;
    }

    uint16 DataLen = 0;
    CxBuffer::fromBuf(pData+RFID_YWG_POS_DATA_LEN,iLength-RFID_YWG_POS_DATA_LEN,DataLen,false); //1
    //长度错
    if((RFID_YWG_FRM_LEN_FIX+DataLen)>iLength)
    {
        return false; //长度错误
    }

    nFrmLen = DataLen+RFID_YWG_FRM_LEN_FIX;

    return true;
}
// lulianbin 20140826 end
/////////////////////////////////////////////////////////////////////////////////////
