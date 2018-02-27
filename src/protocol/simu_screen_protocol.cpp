#include "simu_screen_protocol.h"


SimuScreenProtocolFactory* SimuScreenProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<SimuScreenProtocolFactory>();


const std::string CS_EntrySimuScreenYxSendInterval    = "YxSendInterval";
const std::string CS_EntrySimuScreenYcSendInterval    = "YcSendInterval";
const std::string CS_EntrySimuScreenLedType = "LedType";


SimuScreenProtocol::SimuScreenProtocol()
{
    _yxSendInterval = 2000;

    _yxSendTimer = new CxTimer();
    _yxSendTimer->setTimerTimeOut(this);

    _ycSendInterval = 2000;

    _ycSendTimer = new CxTimer();
    _ycSendTimer->setTimerTimeOut(this);

    _ledType = DIGIT_5;
}

SimuScreenProtocol::~SimuScreenProtocol()
{
    delete _yxSendTimer;
    delete _ycSendTimer;
}

void SimuScreenProtocol::toContext(CxISetSkv& context) const
{
    ProtocolCommon::toContext(context);
    context.setValue(CS_EntrySimuScreenYxSendInterval, _yxSendInterval);
    context.setValue(CS_EntrySimuScreenYcSendInterval, _ycSendInterval);
    context.setValue(CS_EntrySimuScreenLedType, _ledType);
}

void SimuScreenProtocol::fromContext(const CxIGetSkv & context)
{
    ProtocolCommon::fromContext(context);
    _ledType = context.getValue(CS_EntrySimuScreenLedType, DIGIT_5);
    _yxSendInterval = context.getValue(CS_EntrySimuScreenYxSendInterval, 0);
    _ycSendInterval = context.getValue(CS_EntrySimuScreenYcSendInterval, 0);
}

void SimuScreenProtocol::toReport(std::vector<std::string>& sReports) const
{
    sReports.push_back(CxString::format("Timer Send YX Interval : %d", _yxSendInterval));
    sReports.push_back(CxString::format("Timer Send YC Interval : %d", _ycSendInterval));
    sReports.push_back(CxString::format("LedType : %d",_ledType));
}

void SimuScreenProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolCommon::describeSelf(oDescribeUnit);

    CxDescribeInteger* oLedTypeDescribe      = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntrySimuScreenLedType ); oLedTypeDescribe->initValue(_ledType  );
    CxDescribeInteger* oYxSendIntervalDescribe        = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntrySimuScreenYxSendInterval);    oYxSendIntervalDescribe->initValue(_yxSendInterval);
    CxDescribeInteger* oYcSendIntervalDescribe        = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntrySimuScreenYcSendInterval);    oYcSendIntervalDescribe->initValue(_ycSendInterval);
}

void SimuScreenProtocol::protocolTimerTimeOut(const CxTimer* oTimer)
{
    if (oTimer == _yxSendTimer)
    {
         doSendAllYX();
    }
    if (oTimer == _ycSendTimer)
    {
        doSendAllYC();
    }
}

void SimuScreenProtocol::doSendAll()
{
    doSendAllYX();
    doSendAllYC();
}

/* ---------------------------------------------------------------------------
*function: ProtocolReceivedData
*brief:    数据解析，
*Prameter: [in] pData:数据缓冲区
*          [in] iLength:数据缓冲区长度
*return：   无
*author:    luliangbin
*date:      2013/09/04
*modify:
*need:      假设数据没有问题,应该有循环缓冲区
---------------------------------------------------------------------------*/
void SimuScreenProtocol::protocolReceivedData(const uchar *pData, int iLength)
{
    //收到数据
    receivedProtocolDataNow();
    //数据处理
    dealData();
}

void SimuScreenProtocol::dealData()
{
}

void SimuScreenProtocol::doInit()
{
    _sendYxAddValues.clear();
    const std::vector<int>& stationIds = _yxSendTable->stationIds();
    _sendYxValues.resize(stationIds.size());
//    uint32 * data = _sendYxValues.data();
//    for (size_t i = 0; i < stationIds.size(); ++i)
//    {
//        _sendYxAddValues[stationIds.at(i)] = data; data++;
//    }
    for (size_t i = 0; i < stationIds.size(); ++i)
    {
        _sendYxAddValues[stationIds.at(i)] = i;
    }
//    const std::vector<MonsbAddressWorker*> & oWorkers = _yxSendTable->workers();
//    for (size_t i = 0; i < oWorkers.size(); ++i)
//    {
//        MonsbAddressWorker * oWorker = oWorkers.at(i);
//        uint32 value = getCalVal(oWorker);
//        uint8 address = oWorker->stationId();
//        led_btn_set(address, value);
//    }

//    if(_pSin!=NULL)
//    {
//        string szDesc = "";
//        //YX
//        GCCommon::GCObjQueue _qObjYX;
//        if(_pSin->GetReObjQueue(_xmlFile.toAscii().data(),"YGCT;CONFIG;YX",_qObjYX,szDesc))
//        {
//            qDebug()<<"GetYXVector Success:"<<QString::fromStdString(szDesc)<<_qObjYX.size();
//            int nSize = _qObjYX.size();
//            for(int i=0;i<nSize;++i)
//            {
//                GCCommon::GCCommonObj obj;
//                if(_qObjYX.pop(obj))
//                {
//                    GCSimuScreenDataYX m(obj);
//                    _vObjYX.push_back(m);
//                    //遥信值表
//                    if(!_hshValYX.contains(m.addr()))
//                    {
//                        uint32 *pVal = new uint32(1);
//                        *pVal = 0;
//                        _hshValYX.insert(m.addr(),pVal);
//                    }
//                }
//            }
//        }else{
//            qDebug()<<"GetYXQueue Fail";
//        }
//        //YC
//        GCCommon::GCObjQueue _qObjYC;
//        if(_pSin->GetReObjQueue(_xmlFile.toAscii().data(),"YGCT;CONFIG;YC",_qObjYC,szDesc))
//        {
//            qDebug()<<"GetYCVector Success:"<<QString::fromStdString(szDesc)<<_qObjYC.size();
//            int nSize = _qObjYC.size();
//            for(int i=0;i<nSize;++i)
//            {
//                GCCommon::GCCommonObj obj;
//                if(_qObjYC.pop(obj))
//                {
//                    GCSimuScreenDataYC m(obj);
//                    _vObjYC.push_back(m);
//                }
//            }
//        }else{
//            qDebug()<<"GetYCQueue Fail";
//        }
//    }
}

uint32 SimuScreenProtocol::getCalVal(const MonsbAddressWorker *oWorker)
{
    assert(oWorker!=NULL);
    int val = (oWorker->measure()) ? oWorker->measure()->value() : 0;
    if(val<0||val>3) return 0;
//        return val<<(_pos*8+_bit);
    return val << (oWorker->address()*8 + oWorker->port());
}

/**************************************************************************
 *  \fn trendLed_Set
 *   \向潮流显示器发送数据
 *   \param adr        : 设备地址
 *   \param mode       : 数据的类型
 *   \param dat        : 数据
 *   \param decimalPos : 小数点位置
**************************************************************************/
void SimuScreenProtocol::trend_led_set(uint8 adr, uint8 mode, int32 dat, uint8 decimal_pos,uint8 led_type)
{
    char buf[8];
    memset(_sendBuff,0,GCL_LEN_DATA_EM);

    //同步头
    int pos =0;
    uint16 *dw = (uint16 *)&_sendBuff[pos];
    *dw = SYN_FLAG_TREND_LED;
    pos+= sizeof(uint16);
    //addr
    _sendBuff[pos++] = adr;
    //mode
    _sendBuff[pos] = 0;
    if ((decimal_pos >= 2) && (decimal_pos <= 5))
    {
        _sendBuff[pos] |= decimal_pos;
    }    

    if(led_type == DIGIT_4)
    {
        sprintf(buf, "%04d", dat);
        _sendBuff[pos] |= (mode << 4)&0x70;
    }
    else if(led_type == DIGIT_5)
    {
        sprintf(buf, "%05d", dat);
        _sendBuff[pos] |= (ASC_2_HEX(buf[4])<<4);
    }
    else
    {
        _sendBuff[pos] |= 0;
    }
    pos++;

    //值
    if (buf[0] == '-')
    {
        _sendBuff[pos] = 0xb0|(ASC_2_HEX(buf[1]));
    }
    else
    {
        _sendBuff[pos] = (ASC_2_HEX(buf[0])<< 4)|(ASC_2_HEX(buf[1]));
    }    
    pos++;

    _sendBuff[pos++] = (ASC_2_HEX(buf[2])<<4)|(ASC_2_HEX(buf[3]));

    //校验
    _sendBuff[pos++] = CxCrc::calcCRC8(&_sendBuff[2], 4);
//    _sendBuff[pos++] = crc8(&_sendBuff[2], 5);

    pos+=2; //补00

    push(_sendBuff,pos);
}

void SimuScreenProtocol::led_btn_set(uint8 adr,uint32 val)
{
    memset(_sendBuff,0,GCL_LEN_DATA_EM);

    int pos =0;
    uint16 *dw = (uint16 *)&_sendBuff[pos];
    *dw = SYN_HEAD_LED_BTN;
    pos+= sizeof(uint16);
    _sendBuff[pos++] = adr;

    uint32 *dw2 = (uint32 *)&_sendBuff[pos];
    *dw2 = val;

    pos += sizeof(uint32);

    _sendBuff[pos++] = CxCrc::calcCRC8(&_sendBuff[2], 5);

    pos+=2; //补00

    push(_sendBuff,pos);
}

//发送所有遥信
void SimuScreenProtocol::doSendAllYX()
{
    for (std::map<uint8, uint32>::iterator it = _sendYxAddValues.begin(); it != _sendYxAddValues.end(); ++it)
    {
        it->second = 0;
    }
    const std::vector<MonsbAddressWorker*> * oWorkers = _yxSendTable->workers();
    for (size_t i = 0; i < oWorkers->size(); ++i)
    {
        MonsbAddressWorker * oWorker = oWorkers->at(i);
//        MonsbMeasure * oMonsb = oWorker->measure();
//        if (! oMonsb) continue;
        std::map<uint8,uint32>::iterator it = _sendYxAddValues.find(oWorker->stationId());
        if (it != _sendYxAddValues.end())
        {
            uint32 pVal = it->second;
            pVal |= getCalVal(oWorker);
            it->second = pVal;
        }
    }
    for (std::map<uint8, uint32>::const_iterator it = _sendYxAddValues.begin(); it != _sendYxAddValues.end(); ++it)
    {
        led_btn_set(it->first, it->second);
    }
//    for(int i=0; i<_vObjYX.size(); ++i)
//    {
//        GCSimuScreenDataYX obj = _vObjYX.at(i);
//        uint8 addr = obj.addr();
//        if(addr==0)continue;
//        if(!_hshValYX.contains(addr))continue;

//        MonsbMeasure *pMonsb = _yxSendTable->MeasureSonByMeasID(obj.Code());
//        if(pMonsb==NULL)continue;

//        uint32 *pVal = _hshValYX[addr];
//        *pVal |= obj.CalVal(pMonsb->Value());
//    }
    //int
//    QList<uint8> lstKey  = _hshValYX.keys();
//    for(int i=0;i<lstKey.size();++i)
//    {
//        led_btn_set(lstKey.at(i),*(_hshValYX[lstKey.at(i)]));
//    }
}

//发送所有遥测
void SimuScreenProtocol::doSendAllYC()
{
    const std::vector<YcaddAddressWorker*> * oWorkers = _ycSendTable->workers();
    for (size_t i = 0; i < oWorkers->size(); ++i)
    {
        YcaddAddressWorker * oWorker = oWorkers->at(i);
        uint8 adr = oWorker->stationId();
        uint8 mode = oWorker->address();
        int dat = oWorker->getMeasureValue();
        uint8 decimal_pos = oWorker->port();
        uint8 led_type = _ledType;
        trend_led_set(adr, mode, dat, decimal_pos, led_type);
    }
//    for(int i=0;i<_vObjYC.size();++i)
//    {
//        GCSimuScreenDataYC obj = _vObjYC.at(i);
//        if(obj.addr()==0)continue;
//        YcaddMeasure *pYcadd = _ycSendTable->MeasureSonByMeasID(obj.Code());
//        if(pYcadd==NULL)continue;

//        uint16 val =  pYcadd->Value();

//        trend_led_set(obj.addr(),obj.mode(),val,obj.dec_pos(),obj.led_type());
//    }
}

void SimuScreenProtocol::protocolAfterStart()
{
    if(_yxSendTimer!=NULL && _yxSendInterval > 10)
    {
        _yxSendTimer->setInterval(_yxSendInterval);
        _yxSendTimer->start();
    }
    if(_ycSendTimer!=NULL && _ycSendInterval > 10)
    {
        _ycSendTimer->setInterval(_ycSendInterval);
        _ycSendTimer->start();
    }
    doInit();
}

void SimuScreenProtocol::protocolAfterStop()
{
    _yxSendTimer->stop();
    _ycSendTimer->stop();
}

void SimuScreenProtocol::monsbsValueChanged(const std::map<const MonsbMeasure *, int> & monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag)
{
}

void SimuScreenProtocol::ycaddsValueChanged(const std::map<const YcaddMeasure *, double> & ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag)
{
}

void SimuScreenProtocol::checkProtocol()
{
}

