#include "plc_protocol.h"

#include "plc_manager.h"

using namespace std;

const std::string CSEntryReqInterval_ASM570_FINS = "ReqInterval";

PlcProtocolFactory* PlcProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<PlcProtocolFactory>();

PlcProtocol::PlcProtocol()
{
    _yxTable  = createAndRegisterWorkerTable<MonsbAddressWorkerTable, PlcProtocol>("YxTable", MonsbType_rtu_state);
    _ycTable  = createAndRegisterWorkerTable<YcaddAddressWorkerTable, PlcProtocol>("YcTable", YcaddType_rtu_value);

    _callUpInterval =5000;
    _timeCallUp = new CxTimer();
    _timeCallUp ->setTimerTimeOut(this);
    _timeSend = new CxTimer();
    _timeSend->setTimerTimeOut(this);
    _bWork = false;

    _sendFrm.setICF(ASM570_FINS_SEND); //80
    _sendFrm.setGCT(0x02); //02
    GCLFINSAddr dst(0x00,0x01,0x00); //000100
    _sendFrm.setDstAddr(dst);
    GCLFINSAddr src(0x00,0xef,0x00); //00ef00
    _sendFrm.setSrcAddr(src);
    memset(_pPlcbuffer, 0, sizeof(_pPlcbuffer));
    _plcLen = 0;
}

PlcProtocol::~PlcProtocol()
{
}

void PlcProtocol::dealDataResp()
{
//    if(_sentFrms.size()<1) return;

//    bool bRet = false;
//    while(_sentFrms.size()>0)
//    {

//        std::vector<char> buffer = _sentFrms.front();
//        _sentFrms.pop();

//        if(_sendTmp.fromBuff((uchar*)buffer.data(),buffer.size()))
//        {
//            if(_sendTmp.SID()==_recvFrm.SID())
//            {
//                bRet = true;
//                break;
//            }
//        }
//    }
//    if(!bRet) return;

    //判断发送帧是否等于接收帧序号
    if(_sendLast.SID()!=_recvFrm.SID()) return;

    //响应码
    if(_recvFrm.ICF() != 0xC0) return;
    //成功标志
    if(_recvFrm.endCode()!=0x00)
    {
        return;
    }
    //读返回
    if((_recvFrm.MRC() == 0x01) && (_recvFrm.SRC() == 0x01))
    {
        uchar *pdata = _sendLast.getData();
        if(pdata==NULL || _sendLast.getDateLen()<6) return;

        int pos = 0;
        uchar areaCode = pdata[pos++];
        uint16 addrMain = 0;
        CxBuffer::fromBuf(pdata+pos,sizeof(uint16),addrMain,false);
        pos+=sizeof(uint16);
        uchar addrSub = pdata[pos++];
        uint16 num = 0;
        CxBuffer::fromBuf(pdata+pos,sizeof(uint16),num,false);

        switch(areaCode)
        {
        case 0x30://IR
        case 0x31://WR
        case 0x32://HR
            dealYX(areaCode - 0x30 +1,addrMain,addrSub,num,_recvFrm.getData(),_recvFrm.getDateLen());
            break;
        case 0x82://DM
            dealYC(4,addrMain,addrSub,num,_recvFrm.getData(),_recvFrm.getDateLen());
            break;
        default:
            break;
        }
    }

}

void PlcProtocol::dealYX(uchar type, uint16 addr0, uint8 addr1, uint16 num,uchar *pBuf,uint16 len)
{
    if(pBuf==NULL) return;
    if(len<num) return;

    uint16 addrMain = addr0;
    uint8  addrSub = addr1;

    std::map<int, int> addressValues;

    int addr = 0;
    for(int i=0;i<num;i++)
    {
        addr = type * 100000 * 100 + addrMain * 100 + addrSub ;
        addressValues[addr] = pBuf[i];

        addrSub++;
        if(addrSub>0x0f)
        {
            addrMain++;
            addrSub = 0x00;
        }
    }

    if (addressValues.size()>0){
        _yxTable->receiveAddressValues(addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}

void PlcProtocol::dealYC(uchar type, uint16 addr0, uint8 addr1, uint16 num,uchar *pBuf,uint16 len)
{
    if(pBuf==NULL) return;
    if(len<num*sizeof(uint16)) return;

    std::map<int, double> addressValues;

    int addr = 0;
    int32 value = 0;
    uint16 value0=0;
    uint16 value1=0;

    for(int i=0;i<num;i++)
    {
        value = 0;
        addr = type * 100000 * 100 + (addr0+i) * 100;

        CxBuffer::fromBuf(pBuf+i*sizeof(uint16),sizeof(uint16),value0,false);
        CxBuffer::fromBuf(pBuf+i*sizeof(uint16)+2,sizeof(uint16),value1,false);
        value =(int32) ((value1<<16)|value0);
        addressValues[addr] = value;
    }
    if (addressValues.size()>0){
         _ycTable->receiveAddressValues(addressValues, protocolId(), 0, isAutoCreateWorkerAtDeal());
    }
}

void PlcProtocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
    if (oTimer == _timeCallUp)
    {
        CallUp();
    } else if (oTimer == _timeSend)
    {
        doSendOneToPlc();
    }
}

void PlcProtocol::toContext(CxISetSkv& context) const
{
    ProtocolBase::toContext(context);
    context.setValue(CSEntryReqInterval_ASM570_FINS, _callUpInterval);
}

void PlcProtocol::fromContext(const CxIGetSkv& context)
{
    ProtocolBase::fromContext(context);
    int iVal = 0;
    int iRetVal = 0;
    iRetVal = context.getValue(CSEntryReqInterval_ASM570_FINS ,iVal);
    if (iRetVal != 0 && iRetVal > 100) {
        _callUpInterval = iRetVal;
    }
}

void PlcProtocol::toReport(std::vector<std::string> &sReports) const
{
    sReports.push_back(CxString::format("CallUpInterval : %d",_callUpInterval));
}

void PlcProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
//    oDescribeUnit->SetFuncDescribeValuesCalc(DescribeValuesCalc);
    //    CxDescribeInteger* oControlWordDescribe           = oDescribeUnit->createAndRegister<CxDescribeInteger>(CSEntryCdtControlWord          ); oControlWordDescribe->InitValue(_controlWord                    ); oControlWordDescribe->SetIsHexDisplay(true)    ; oControlWordDescribe->SetRang(0, 255)    ;
}

void PlcProtocol::protocolAfterStart()
{
    if(_timeCallUp!=NULL)
    {
        _timeCallUp->setInterval(_callUpInterval);
        _timeCallUp->start(); //扫描PLC -- 接受处理PLC上来的信息
        cxDebug()   <<"_timeCallUp";
    }
    if(_timeSend!=NULL)
    {
        _timeSend->setInterval(100);
        _timeSend->start(); //定时发送数据
        cxDebug()   <<"_timeSend";
    }
    if(!_bWork)
    {
        login();
        cxDebug()   <<"login()";
    }
}

void PlcProtocol::protocolAfterStop()
{
    _timeCallUp->stop();
}

/* ---------------------------------------------------------------------------
*function: protocolReceivedData
*brief:    数据解析，
*Prameter: [in] pData:数据缓冲区
*          [in] iLength:数据缓冲区长度
*return：   无
*author:    luliangbin
*date:      2013/09/04
*modify:
*need:      假设数据没有问题,应该有循环缓冲区
---------------------------------------------------------------------------*/
void PlcProtocol::protocolReceivedData(const uchar *pData, int iLength)
{
    receivedProtocolDataNow();
    if((_plcLen +iLength)<=PLC_BUF_LEN)
    {
        memcpy( _pPlcbuffer+_plcLen, pData, iLength );
        _plcLen += iLength;
        DealData();
    }else{
        _plcLen = 0;
        memset(_pPlcbuffer,0,PLC_BUF_LEN);
    }
}

void PlcProtocol::DealData()
{
    uint16 wLen = _plcLen;
    int nStart = FindHeader(_pPlcbuffer,_plcLen);
    //未找到头
    if(nStart<0)
    {
        _plcLen = 0;
        return;
    }
    //帧检查
    int nFrmLen = 0;
    //帧OK
    if( CheckFrame(_pPlcbuffer+nStart,wLen-nStart,nFrmLen) ) //nStart = 0x46494e53开始的位置
    {
        //起始位置

        //成功
        if(_recvHead.fromBuff(_pPlcbuffer+nStart,wLen-nStart))
        {
            if(_recvHead.error()==ASM570_FINS_ERR_SUCCED) //0x00000000
            {
                switch(_recvHead.cmd())
                {
                    case ASM570_FINS_CMD_REG_RESP: //0x00000001
                    {
                         cxDebug()   <<"login succeed: "<<_recvHead.cmd();
                         _bWork = true;
                    }
                        break;
                    case ASM570_FINS_CMD_WORK: //0x00000002
                    {
                        if(_recvFrm.fromBuff(_pPlcbuffer+nStart,wLen-nStart))
                        {
                            dealDataResp();
                        }
                     }
                        break;
                    default:
                        break;
                }
            }
            else//有错误
            {
               cxDebug()   <<"response error: "<<_recvHead.error();
               _bWork  = false;
            }
        }
        //多帧
        if((nStart+nFrmLen) < _plcLen) //移除
        {
            int iRemain = _plcLen - (nStart+nFrmLen);
            memmove(_pPlcbuffer, _pPlcbuffer+nStart+nFrmLen, iRemain);
            _plcLen = iRemain;
            DealData();
        }
        else
        {
            _plcLen = 0;
            memset(_pPlcbuffer,0,PLC_BUF_LEN);
        }
    }
    else //清空
    {
        _plcLen = 0;
        memset(_pPlcbuffer,0,PLC_BUF_LEN);
    }
}

void PlcProtocol::doSendOneToPlc()
{
    if (_sendBuffers.size()>0)
    {
        std::vector<char> buffer = _sendBuffers.front();
        _sendBuffers.pop();

        if(writeData(&buffer.front(), buffer.size())>0)
        {
           _sendLast.fromBuff((uchar*)&buffer.front(), buffer.size());
//            _sentFrms.push(buffer);
        }
    }

}

//寻找头
int PlcProtocol::FindHeader(uchar *pData, int iLength)
{
    if((pData==NULL)||(iLength<ASM570_FINS_FRM_LEN_MIN)) //16
    {
        return -1;
    }

    int nFind = -1;
    uint32 head;
    uint32 DataLen = 0;

    iLength -= (sizeof(uint32)*2);

    for(int i=0;i<iLength;i++) //一个一个搜
    {
        head = 0;
        CxBuffer::fromBuf(pData+i,iLength-i,head,false);
        if(head==ASM570_FINS_FRM_LEADFlAG) //0x46494e53
        {            
            if(CxBuffer::fromBuf(pData+i+sizeof(uint32),iLength-i-sizeof(uint32),DataLen,false))//4
            {  //长度错
                if((DataLen<=iLength)&&(DataLen>=8))
                {
                    nFind = i;
                    break;
                }
            }
        }
    }
    return nFind;
}
//帧检查
bool PlcProtocol::CheckFrame(uchar *pData, int iLength,int &nFrmLen)
{
    nFrmLen = 0;
    if((pData==NULL)||(iLength<ASM570_FINS_FRM_LEN_MIN)) //16
    {
        return false;
    }

    uint32 DataLen = 0;

    CxBuffer::fromBuf(pData+ASM570_FINS_POS_DATA_LEN,iLength-ASM570_FINS_POS_DATA_LEN,DataLen,false); //4

    //长度错
    if((ASM570_FINS_FRM_LEN_FIX+DataLen)>iLength)
    {
        return false; //长度错误
    }

    nFrmLen = DataLen+ASM570_FINS_FRM_LEN_FIX;

    return true;
}

void PlcProtocol::CallUp()
{
    if(_bWork)
    {
        if((CxTime::currentSystemTime()-receivedProtocolDataTime())>_callUpInterval)
        {
            CmdQueryStatus();
        }
    }
    else
    {
        login();
    }
}
void PlcProtocol::CallUp(bool bNow)
{
    if(_bWork)
    {
        if(bNow)
        {
            CmdQueryStatus();
        }
        else
        {
            if((CxTime::currentSystemTime()-receivedProtocolDataTime())>_callUpInterval)
            {
                CmdQueryStatus();
            }
        }
    }
    else
    {
        login();
    }
}
//注册包
void PlcProtocol::login()
{
    //needtodo
    GCLFINSHead head(0x0c,0,0);
    memset(_sendBuff,0,sizeof(_sendBuff));
    head.toBuff(_sendBuff); //形成指令
    int lenth = head.length()+ASM570_FINS_FRM_LEN_FIX;
    if(lenth>0)
    {
        push((char*)_sendBuff,lenth); //发送指令
    }
}

//发送请求
void PlcProtocol::SendReq(ushort MSCmd, char *pBuf, int len) //在这里拼装指令!
{
    memset(_sendBuff,0,sizeof(_sendBuff)); //4096
    _sendFrm.changeSID();
    _sendFrm.setData(pBuf,len);
    _sendFrm.setMS(MSCmd);
    _sendFrm.setCmd(ASM570_FINS_CMD_WORK); //00000002
    _sendFrm.setLen(_sendFrm.getSize()-ASM570_FINS_FRM_LEN_FIX); //8

    int lenth = _sendFrm.toBuff(_sendBuff); //all in _data
    if(lenth>0)
    {
        push((char*)_sendBuff,lenth);
    }
}

void PlcProtocol::CmdExecute(int iCmdCode, const std::string & sBuffer)
{
    outProtocolPrompt() << "CmdExecute [CmdCode] [Buffer] : " << CxString::toHexstring(iCmdCode) << sBuffer << cxEndLine;
    vector<uchar> buffer = CxString::fromHexstring(sBuffer);
    SendReq(iCmdCode, (char *)buffer.data(), buffer.size());
}

//<Prop id="00000001" name="YX1" desc="X轴故障 IR2000.00-"  code="0x0101" type="1" level="3" ext="3007D000000F"/>
void PlcProtocol::CmdQueryStatus()
{    
    readAllYX();
    readAllYC();
}

void PlcProtocol::readAllYX()
{
    //All YX
    if(_bWork)
    {
        setUpFrame(0x0101,20010000,32,NULL,0);
        setUpFrame(0x0101,10300000,128,NULL,0);
        setUpFrame(0x0101,30000500,4,NULL,0);
    }
}

void PlcProtocol::readAllYC()
{
    //All YC
    if(_bWork)
    {
        setUpFrame(ASM570_FINS_MS_CPU_RD,40100000,50,NULL,0);
        setUpFrame(ASM570_FINS_MS_CPU_RD,40013000,200,NULL,0);

    }
}


void PlcProtocol::setUpFrame(uint16 cmdCode,uint32 beginAddr,uint16 num,char *val,int len)
{
    if((cmdCode!= 0x0101) && (cmdCode!= 0x0102))
    {
        //不支持的命令字
        return;
    }

    uchar tmpBuff[512] = {'\0'};
    int pos = 0;
// addr 构成 000010001 100.01 ;00300001  3000.01

    uchar areaCode = (uchar)(beginAddr/(100*100000));
    if(areaCode <4)areaCode += (0x30-1);
    else areaCode = 0x82;

    tmpBuff[pos++] = areaCode;

    uint16 addrMain = (uint16)(((uint32)(beginAddr/100))%100000);
    uchar  addrSub  = (uchar)(beginAddr%100);

    CxBuffer::toBuf(tmpBuff+pos,addrMain,false);
    pos+= sizeof(uint16);
    tmpBuff[pos++] = addrSub;

    CxBuffer::toBuf(tmpBuff+pos,num,false);
    pos+= sizeof(uint16);

    if(len>0 && val != NULL)
    {
        memcpy(tmpBuff+pos,val,len);
        pos += len;
    }
    SendReq(cmdCode,(char*)tmpBuff,pos);
}

char PlcProtocol::getYxCommand(const string &sCommandCode, bool *ok)
{
    const MonsbAddressWorker* oWorker = _yxTable->workerByCode(sCommandCode);
    if (oWorker && oWorker->measure())
    {
        int iValue = oWorker->measure()->value();
        if (iValue >= 0 && iValue <= 9)
        {
            if (ok)
                *ok = true;
            return iValue + 0x30;
        }
    }
    if (ok)
        *ok = false;
    return false;
}

bool PlcProtocol::getYxCommand(const string &sCommandCode, int &iValue)
{
    iValue = 0;
    bool bRet = false;
    const MonsbAddressWorker* oWorker = _yxTable->workerByCode(sCommandCode);
    if (oWorker && oWorker->measure())
    {
        iValue = oWorker->measure()->value();
        bRet = true;
    }
    return bRet;
}

bool PlcProtocol::setYxCommand(const string &sCommandCode, char value)
{
    const MonsbAddressWorker* oWorker = _yxTable->workerByCode(sCommandCode);
    if (oWorker)
    {
        int iAddress = oWorker->address();
        setUpFrame(0x0102, iAddress, 1, &value, 1);
        return true;
    }
    else
    {
        outProtocolPrompt() << "setYxCommand -CommandCode is not exist:[" << sCommandCode << "]" << cxEndLine;
        return false;
    }
}

string PlcProtocol::getYcCommand(const string &sCommandCode, bool *ok)
{
    const YcaddAddressWorker* oWorker = _ycTable->workerByCode(sCommandCode);
    if (oWorker && oWorker->measure())
    {
        double dValue = oWorker->measure()->value();
        if(ok)
            *ok = true;
        return CxString::toString((int)dValue);
    }
    if(ok)
        *ok = false;

    return "";
}

bool PlcProtocol::getYcCommand(const string &sCommandCode, double &dValue)
{
    dValue = 0.0f;
    bool bRet = false;
    const YcaddAddressWorker* oWorker = _ycTable->workerByCode(sCommandCode);
    if (oWorker && oWorker->measure())
    {
        dValue = oWorker->measure()->value();
        bRet = true;
    }
    return bRet;
}

bool PlcProtocol::getYcCommand(const string &sCommandCode, int &iValue)
{
    iValue = 0.0f;
    bool bRet = false;
    const YcaddAddressWorker* oWorker = _ycTable->workerByCode(sCommandCode);
    if (oWorker && oWorker->measure())
    {
        iValue = (int)oWorker->measure()->value();
        bRet = true;
    }
    return bRet;
}

/*
bool PlcProtocol::setYcCommand(const std::string &sCommandCode, const string &sValue)
{
    const YcaddAddressWorker* oWorker = _ycTable->workerByCode(sCommandCode);
    if (oWorker)
    {
        int iAddress = oWorker->Address();
        setUpFrame(0x0102, iAddress, 1, (char *)sValue.data(), sValue.size());
        return true;
    }
    else
    {
        OutPromptInfo("setYcCommand -CommandCode is not exist!");
        return false;
    }
}
*/

bool PlcProtocol::setYcCommand(const string &sCommandCode, int value)
{
    const YcaddAddressWorker* oWorker = _ycTable->workerByCode(sCommandCode);
    if (oWorker)
    {
        int iAddress = oWorker->address();

        uchar buf[4]={0};
        int pos = 0;
        pos = CxBuffer::toBuf(&buf[pos],(uint16)(value &0xFFFF),false);
        pos += CxBuffer::toBuf(&buf[pos],(uint16)(value>>16),false);

        setUpFrame(ASM570_FINS_MS_CPU_WR, iAddress, 2,(char*)buf, pos);
        return true;
    }
    else
    {
        outProtocolPrompt() << "setYcCommand -CommandCode is not exist:[" << sCommandCode << "]" << cxEndLine;
        return false;
    }
}

bool PlcProtocol::setYcCommand(const string &sCommandCode, short value)
{
    const MonsbAddressWorker* oWorker = _yxTable->workerByCode(sCommandCode);
    if (oWorker)
    {
        int iAddress = oWorker->address();

        uchar buf[2]={0};
        int pos = 0;
        pos = CxBuffer::toBuf(&buf[pos],value,false);

        setUpFrame(ASM570_FINS_MS_CPU_WR, iAddress, 1,(char*)buf, pos);
        return true;
    }
    else
    {
        outProtocolPrompt() << "setYcCommand -CommandCode is not exist:[" << sCommandCode << "]" << cxEndLine;
        return false;
    }
}

