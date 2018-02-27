#include "protocol_common.h"

ProtocolCommon::ProtocolCommon()
{
    _yxRecTable  = createAndRegisterWorkerTable<MonsbAddressWorkerTable, ProtocolCommon>("YxReceiveTable", MonsbType_rtu_state);
    _ykRecTable  = createAndRegisterWorkerTable<MonsbAddressWorkerTable, ProtocolCommon>("YkReceiveTable", MonsbType_rtu_state);
    _ycRecTable  = createAndRegisterWorkerTable<YcaddAddressWorkerTable, ProtocolCommon>("YcReceiveTable", YcaddType_rtu_value);
    _ywRecTable  = createAndRegisterWorkerTable<StrawAddressWorkerTable, ProtocolCommon>("YwReceiveTable", StrawType_rtu_value);

    _yxSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, ProtocolCommon>("YxSendTable", MonsbType_rtu_state);
    _ykSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, ProtocolCommon>("YkSendTable", MonsbType_rtu_state);
    _ycSendTable = createAndRegisterWorkerTable<YcaddAddressWorkerTable, ProtocolCommon>("YcSendTable", YcaddType_rtu_value);
    _ywSendTable = createAndRegisterWorkerTable<StrawAddressWorkerTable, ProtocolCommon>("YwSendTable", StrawType_rtu_value);

    createAndRegisterCases<ProtocolCommon, SendMonsbControlCases>(this);
    createAndRegisterCases<ProtocolCommon, SendCustomDataCases>(this);

    _sendMonsbCycle   = createAndRegisterCycle<ProtocolCommon, SendMonsbCycle>(this);
    _sendYcaddCycle   = createAndRegisterCycle<ProtocolCommon, SendYcaddCycle>(this);


    _timeCallUp       = new CxTimer();
    _timeCallUp      ->setTimerTimeOut(this);

    _callUpInterval = 5000;

    _started = false;
    _ykInitiative = true;
    _bInit = false;
}

ProtocolCommon::~ProtocolCommon()
{
    delete _timeCallUp;

}

void ProtocolCommon::toContext(CxISetSkv& context) const
{
    ProtocolBase::toContext(context);
    context.setValue(CS_EntryReqInterval, _callUpInterval);
    context.setValue(CS_EntrySendInterval,_outInterval);
    context.setValue(CS_EntryUrlPath,_urlPath);
}

void ProtocolCommon::fromContext(const CxIGetSkv& context)
{
    ProtocolBase::fromContext(context);
    int iInterval = 0;
    iInterval = context.getValue(CS_EntryReqInterval ,iInterval);
    if (iInterval != 0 && iInterval >= 10) {
        _callUpInterval = iInterval;
    }
    _outInterval = context.getValue(CS_EntrySendInterval,_outInterval);
    _urlPath     = context.getValue(CS_EntryUrlPath,_urlPath);
}

void ProtocolCommon::toReport( std::vector<std::string>& sReports) const
{
    sReports.push_back(std::string("CallUpInterval : ") + CxString::toString(_callUpInterval));
}

void ProtocolCommon::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
}

void ProtocolCommon::protocolTimerTimeOut(const CxTimer *oTimer)
{
}

void ProtocolCommon::channelAfterOpen(const CxChannelBase* oChannel)
{
    _started = false;
}

void ProtocolCommon::channelAfterClose(const CxChannelBase* oChannel)
{
    _started = false;
}

void ProtocolCommon::dealData()
{    //收到数据
    receivedProtocolDataNow();
}

void ProtocolCommon::dealYX ( )
{
    if(_yxRecTable == NULL || _yxRecTable->addressCount()<1) return;
}


void ProtocolCommon::dealYC ( )
{
    if(_ycRecTable==NULL || _ycRecTable->addressCount()<1) return;
}

void ProtocolCommon::dealYK()
{
    //收表为空
    if(_ykRecTable==NULL || _ykRecTable->addressCount()<1) return;

}

void ProtocolCommon::protocolAfterStart()
{
    _started = false;

    _timeCallUp->setInterval(_callUpInterval);
    _timeCallUp->start();
    //重启发送与任务
    startSendTm(_outInterval);
}

void  ProtocolCommon::regMeasureToTRDB()
{
    regMonsbTable(_yxRecTable,GCL_MEAS_YX_REV);
    regMonsbTable(_yxSendTable,GCL_MEAS_YX_SEND);
    regMonsbTable(_ykRecTable,GCL_MEAS_YK_REV);
    regMonsbTable(_ykSendTable,GCL_MEAS_YK_SEND);

    regYcaddTable(_ycRecTable,GCL_MEAS_YC_REV);
    regYcaddTable(_ycSendTable,GCL_MEAS_YC_SEND);

    regStrawTable(_ywRecTable,GCL_MEAS_SC_REV);
    regStrawTable(_ywSendTable,GCL_MEAS_SC_SEND);
}
void ProtocolCommon::regMonsbTable(MonsbAddressWorkerTable *pTable,int type)
{
    if(pTable == NULL || pTable->addressCount()<1) return;


    const std::vector<MonsbAddressWorker*>* oWorkers = pTable->workers();
    if (oWorkers->size() < 1) return;

    memset(_pBuffer,0,_bufLen);

    int nCount = 0;
    int maxNum = _bufLen/sizeof(uint32);

    for (size_t i = 0; i < oWorkers->size(); ++i)
    {
        const MonsbAddressWorker* oWorker = oWorkers->at(i);
        const MonsbMeasure *oMeasure = oWorker->measure();
        if(oMeasure!=NULL)
        {
            memcpy(_pBuffer+nCount*sizeof(uint32),&oMeasure->measureId(),sizeof(uint32));
            nCount++;
        }
        //发送缓冲区满
        if(nCount>=maxNum)
        {
            outProtocolDebug()<<"regMonsbMeasue"<<type<<nCount<<cxEndLine;
            sendMsg(GCL_CMD_SYS_MEAS_REG,type,maxNum*sizeof(uint32),_pBuffer);
            nCount = 0;
            CxThread::sleep(10);
        }
    }
    if(nCount>0)
    {
        sendMsg(GCL_CMD_SYS_MEAS_REG,type,nCount*sizeof(uint32),_pBuffer);
        outProtocolDebug()<<"regMonsbMeasue"<<type<<nCount<<cxEndLine;
        CxThread::sleep(10);
    }
}

void ProtocolCommon::regYcaddTable(YcaddAddressWorkerTable *pTable,int type)
{
    if(pTable == NULL || pTable->addressCount()<1) return;


    const std::vector<YcaddAddressWorker*>* oWorkers = pTable->workers();
    if (oWorkers->size() < 1) return;

    memset(_pBuffer,0,_bufLen);

    int nCount = 0;
    int maxNum = _bufLen/sizeof(uint32);

    for (size_t i = 0; i < oWorkers->size(); ++i)
    {
        const YcaddAddressWorker* oWorker = oWorkers->at(i);
        const YcaddMeasure *oMeasure = oWorker->measure();
        if(oMeasure!=NULL)
        {
            memcpy(_pBuffer+nCount*sizeof(uint32),&oMeasure->measureId(),sizeof(uint32));
            nCount++;
        }
        //发送缓冲区满
        if(nCount>=maxNum)
        {
            outProtocolDebug()<<"regYcaddMeasue"<<type<<nCount<<cxEndLine;
            sendMsg(GCL_CMD_SYS_MEAS_REG,type,maxNum*sizeof(uint32),_pBuffer);
            nCount = 0;
            CxThread::sleep(10);
        }
    }
    if(nCount>0)
    {
        sendMsg(GCL_CMD_SYS_MEAS_REG,type,nCount*sizeof(uint32),_pBuffer);
        outProtocolDebug()<<"regYcaddMeasue"<<type<<nCount<<cxEndLine;
        CxThread::sleep(10);

    }
}
void ProtocolCommon::regStrawTable(StrawAddressWorkerTable *pTable,int type)
{
    if(pTable == NULL || pTable->addressCount()<1) return;


    const std::vector<StrawAddressWorker*>* oWorkers = pTable->workers();
    if (oWorkers->size() < 1) return;

    memset(_pBuffer,0,_bufLen);

    int nCount = 0;
    int maxNum = _bufLen/sizeof(uint32);

    for (size_t i = 0; i < oWorkers->size(); ++i)
    {
        const StrawAddressWorker* oWorker = oWorkers->at(i);
        const StrawMeasure *oMeasure = oWorker->measure();
        if(oMeasure!=NULL)
        {
            memcpy(_pBuffer+nCount*sizeof(uint32),&oMeasure->measureId(),sizeof(uint32));
            nCount++;
        }
        //发送缓冲区满
        if(nCount>=maxNum)
        {
            outProtocolDebug()<<"regStrawMeasue"<<type<<nCount<<cxEndLine;
            sendMsg(GCL_CMD_SYS_MEAS_REG,type,maxNum*sizeof(uint32),_pBuffer);
            nCount = 0;
        }
    }
    if(nCount>0)
    {
        sendMsg(GCL_CMD_SYS_MEAS_REG,type,nCount*sizeof(uint32),_pBuffer);
        outProtocolDebug()<<"regStrawMeasue"<<type<<nCount<<cxEndLine;
    }
}

void ProtocolCommon::protocolAfterStop()
{
    _timeCallUp->stop();
}

void ProtocolCommon::monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag)
{
    if (monsbOldValues.size() > 0 && eMonsbType == MonsbType_rtu_state)
    {

        static int yxChange = 0;
//        outProtocolDebug()<<"monsbsValueChanged:"<<yxChange++<<cxEndLine;
//        sendMe
//                std::map<int, int> addressValues = _yxSendTable->addressValues(monsbOldValues, true);
//                SendStates2(addressValues, FunCode_YX_BianWei);
    }
}

void ProtocolCommon::ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag)
{
    static int ycChange = 0;
//    outProtocolDebug()<<"ycaddsChanged:"<<ycChange++<<cxEndLine;


}

void ProtocolCommon::doTask(GCCommon::GCDataElement dt)
{
    switch(dt.type())
    {
    case GCL_CMD_SYS_DEBUG://调试
        if(dt.pram()>0) CxChannelManager::setPacketInterinfoOut(true);
        else  CxChannelManager::setPacketInterinfoOut(false);
        break;
    default:
        break;

    }

}


