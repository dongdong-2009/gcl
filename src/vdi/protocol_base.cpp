#include "protocol_base.h"

#include "monsb_address_worker.h"
#include "ycadd_address_worker.h"
#include "straw_address_worker.h"

//void* ProtocolFactoryManager::s_factorys = NULL;

ProtocolBase::ProtocolBase()
{
    _protocolId = -1;
    _protocolName = "";
    _channel = NULL;
    _isStart = false;
    _isAutoCreateWorkerAtDeal = false;
    _protocolRole = ProtocolRole_0;
    _receivedProtocolDataTime = CxTime::currentSystemTime();
    _outLineInterval = 60000;
    _protocolTitle = "";

    _caseses = new std::vector<CasesBase*>();
    _cycles = new std::vector<CycleBase*>();
    _workerTables = new std::vector<WorkerTableBase*>();

    _checkProtocolTm = new CxTimer();
    _checkProtocolTm->setTimerTimeOut(this);
    _checkProtocolTm->setInterval(1000);

    MonsbManager::addObserver(this);
    YcaddManager::addObserver(this);


    _bConnect = false;
    _lastTime = CxTime::currentSystemTime();
    //输出周期
    _qDataEm.clear();

    _outInterval = 100; //100ms
    _sendTm = new CxTimer();
    _sendTm->setInterval(_outInterval);
    _sendTm->setTimerTimeOut(this);

    _workInterval = 50;//50ms
    _workTm = new CxTimer();
    _workTm->setInterval(_workInterval);
    _workTm->setTimerTimeOut(this);


    _mqIp = "127.0.0.1";

    _bufLen = GCL_LEN_DATA_EM;
    _pBuffer = new char[_bufLen];
    memset(_pBuffer,0,_bufLen);
    _failTimes = 0;
    //设置连接状态
    setConnect(false);
}

ProtocolBase::~ProtocolBase()
{
    unLoadChannel();

    MonsbManager::removeObserver(this);
    YcaddManager::removeObserver(this);
    delete _checkProtocolTm;

    GM_PLISTO_DELETEALL(_caseses, CasesBase);
    delete _caseses;

    GM_PLISTO_DELETEALL(_cycles, CycleBase);
    delete _cycles;

    GM_PLISTO_DELETEALL(_workerTables, WorkerTableBase);
    delete _workerTables;

    _qDataEm.clear();
    delete _sendTm;
    delete _workTm;
}

CasesBase * ProtocolBase::cases(const std::string &sTypeSimpleName) const
{
    GM_PLISTO_FIND(_caseses, CasesBase, typeSimpleName(), sTypeSimpleName)
}

WorkerTableBase * ProtocolBase::workerTable(const std::string &sTableFullName) const
{
    GM_PLISTO_FIND(_workerTables, WorkerTableBase, tableName(), sTableFullName)
}


void ProtocolBase::toContext(CxISetSkv& context) const
{
    context.setValue(CS_EntryProtocolName, _protocolName);
    context.setValue(CS_EntryProtocolId, _protocolId);
    if (_channel) {
        std::string sChannelTypeSimpleName = _channel->typeSimpleName();
        context.setValue(CS_EntryChannel, sChannelTypeSimpleName);
    }
    context.setValue(CS_EntryProtocolRole, _protocolRole);

    context.setValue(CS_EntryMQPort, _mqPort);
    context.setValue(CS_EntryMQIP, _mqIp);
    context.setValue(CS_EntryProtocolTitle,_protocolTitle);
//    context.setValue(CS_EntryOutInterval, _outInterval);
}


void ProtocolBase::fromContext(const CxIGetSkv& context)
{
    _protocolName = context.getValue(CS_EntryProtocolName, _protocolName);
    _protocolId = context.getValue(CS_EntryProtocolId, _protocolId);
    _protocolRole = (ProtocolRoleEnum)context.getValue(CS_EntryProtocolRole, _protocolRole);

    _mqPort = CS_MQPortBase+_protocolId;
    _mqPort = context.getValue(CS_EntryMQPort, _mqPort);
    _mqIp   = context.getValue(CS_EntryMQIP, _mqIp);

    _protocolTitle = _protocolName;
    _protocolTitle = context.getValue(CS_EntryProtocolTitle,_protocolTitle);

//    _outInterval   = context.getValue(CS_EntryOutInterval, _outInterval);
}

void ProtocolBase::describeSelf(CxDescribeUnit* oDescribeUnit) const
{
    assert(oDescribeUnit);
    CxDescribeString* oProtocolNameDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_EntryProtocolName);
    oProtocolNameDescribe->setIsReadOnly(true);
    oProtocolNameDescribe->initValue(_protocolName);
    CxDescribeInteger* oProtocolIdDescribe = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryProtocolId);
    oProtocolIdDescribe->setIsReadOnly(true);
    oProtocolIdDescribe->initValue(_protocolId);
    CxDescribeEnum* oProtocolRoleDescribe = oDescribeUnit->createAndRegister<CxDescribeEnum>(CS_EntryProtocolRole);
    oProtocolRoleDescribe->initValue(_protocolRole);
    oProtocolRoleDescribe->setEnumCopeFromZero(CE_MaxProtocolRole);
}

std::vector<std::string> ProtocolBase::reportSelf() const
{
    std::vector<std::string> sReports;
    sReports.push_back("*** Protocol Report ***");
    sReports.push_back(std::string("typeSimpleName: " + CxString::toString(typeSimpleName())));
    sReports.push_back(std::string("protocolId: " + CxString::toString(_protocolId)));
    toReport(sReports);
    if (_channel)
    {
        CxContainer::append(sReports, _channel->reportSelf());
    }
    return sReports;
}

int ProtocolBase::writeData(const char *pData, int iLength)
{
    int r = -1;
//    if (_channel && _bConnect)
    if (_channel)
    {
        r =  _channel->sendData(pData, iLength);
        if(r<=0) _bConnect = false;
        else  _bConnect = true;
    }
    return r;
}

int ProtocolBase::writeData(const uchar *pData, int iLength)
{
    int r = -1;
//    if (_channel && _bConnect)
    if (_channel)
    {
        r = _channel->sendData((char *)pData, iLength);
        if(r<=0) _bConnect = false;
        else  _bConnect = true;
    }
    return r;
}

int ProtocolBase::writeBytes(const std::vector<uchar> &bytes)
{
    int r = -1;
//    if (_channel && _bConnect)
    if (_channel)
    {
        r =  _channel->sendBytes(bytes);
        if(r<=0) _bConnect = false;
        else  _bConnect = true;
    }
     return r;
}

int ProtocolBase::writeText(const std::string &sText)
{
    int r = -1;
//    if (_channel && _bConnect)
    if (_channel)
    {
        r =  _channel->sendText(sText);
        if(r<=0) _bConnect = false;
        else  _bConnect = true;
    }
    return r;
}

void ProtocolBase::unLoadChannel()
{
    if (_channel) {
        _channel->removeObserver(this);
        CxChannelManager::recoveChanrnel(_channel);
        _channel = NULL;
    }
}

void ProtocolBase::setChannel(CxChannelBase *oChannel)
{
    unLoadChannel();
    if (oChannel){
        _channel = oChannel;
        _channel->addObserver(this);
    }
}

void ProtocolBase::channel_beforeDelete(const CxChannelBase *oChannel)
{
    if (oChannel == _channel){
        unLoadChannel();
    }
}

void ProtocolBase::loadTable(const std::string &sTableName, const std::string &sFilePath)
{
    if (_isStart)
        return;
    WorkerTableBase* oWorkerTable = workerTable(sTableName);
    if ( oWorkerTable ) oWorkerTable->loadFromFile(sFilePath);
}

void ProtocolBase::saveTable(const std::string &sTableName, const std::string &sFilePath) const
{
    WorkerTableBase* oWorkerTable = workerTable(sTableName);
    if ( oWorkerTable ) oWorkerTable->saveToFile(sFilePath);
}

void ProtocolBase::channelFromContext(const CxIGetSkv &context)
{
    if (_channel)
    {
        if (_channel->connected())
            return;
        _channel->fromContext(context);
    }
}

void ProtocolBase::describeWorkerTable(CxDescribeUnit *oDescribeUnit, WorkerTableBase* oWorkerTable) const
{
    if (oWorkerTable)
        oWorkerTable->describeSelf(oDescribeUnit);
}

void ProtocolBase::timer_timeOut(const CxTimer *oTimer)
{
    if(oTimer == _workTm) //工作
    {
//        if(_qMsg.size()>0)
//        {
//            int nCount = _qMsg.size();

//            for(int i=0;i<nCount;i++)
//            {
//                if(_qMsg.pop(_dtOut))
//                {
//                    doTask(_dtOut);
//                }
//            }
//        }
    }
    else if(oTimer == _sendTm) //发送
    {
        //输出
          if(_qDataEm.size()>0)
          {
              if(_qDataEm.pop(_dtOut))
              {
                  writeData((uchar*)_dtOut.buffer(),(int)_dtOut.dataLenth());
              }
          }

    }
    else  if (oTimer == _checkProtocolTm)
    {
        if (!isOnline())
        {
            protocolOutLine();
        }
        checkProtocol();
//报告终端状态
        reportTerminalStatus();
    }

    protocolTimerTimeOut(oTimer);
}

//MonsbManagerSubject
void ProtocolBase::measures_prepareChange(const std::map<const MonsbMeasure *, MeasureChangeTypeEnum> &monsbChangeTypes, int iMeasureSonType)
{
    const std::vector<WorkerTableBase*>* oWorkerTables = _workerTables;
    for (size_t i = 0; i < oWorkerTables->size(); ++i) {
        WorkerTableBase* oWorkerTable = oWorkerTables->at(i);
        if (oWorkerTable->typeSimpleName() == MonsbAddressWorker::TYPESIMPLENAME()) {
            MonsbAddressWorkerTable* oMonsbAddressWorkerTable = reinterpret_cast<MonsbAddressWorkerTable*>(oWorkerTable);
            std::vector<const MonsbMeasure*> monsbs = CxContainer::keys( monsbChangeTypes, MeasureChangeType_Delete);
            oMonsbAddressWorkerTable->measuresBeforeDelete(monsbs);
        }
    }
}

void ProtocolBase::measures_valueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, int iMeasureSonType, int iSourceId, int iTag)
{
    if (iSourceId != protocolId()) {
        monsbsValueChanged(monsbOldValues, g_fn_monsbTypeEnum(iMeasureSonType), iSourceId, iTag);
    }
}

//YcaddManagerSubject
void ProtocolBase::measures_prepareChange(const std::map<const YcaddMeasure *, MeasureChangeTypeEnum> &ycaddChangeTypes, int iMeasureSonType)
{
    const std::vector<WorkerTableBase*>* oWorkerTables = _workerTables;
    for (size_t i = 0; i < oWorkerTables->size(); ++i) {
        WorkerTableBase* oWorkerTable = oWorkerTables->at(i);
        if (oWorkerTable->typeSimpleName() == YcaddAddressWorker::TYPESIMPLENAME()) {
            YcaddAddressWorkerTable* oYcaddAddressWorkerTable = reinterpret_cast<YcaddAddressWorkerTable*>(oWorkerTable);
            std::vector<const YcaddMeasure*> ycadds = CxContainer::keys( ycaddChangeTypes, MeasureChangeType_Delete);
            oYcaddAddressWorkerTable->measuresBeforeDelete(ycadds);
        }
    }
}

void ProtocolBase::measures_valueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, int iMeasureSonType, int iSourceId, int iTag)
{
    if (iSourceId != protocolId()) {
        ycaddsValueChanged(ycaddOldValues, g_fn_ycaddTypeEnum(iMeasureSonType), iSourceId, iTag);
    }
}

//StrawManagerSubject
void ProtocolBase::measures_prepareChange(const std::map<const StrawMeasure *, MeasureChangeTypeEnum> &strawChangeTypes, int iMeasureSonType)
{
    const std::vector<WorkerTableBase*>* oWorkerTables = _workerTables;
    for (size_t i = 0; i < oWorkerTables->size(); ++i) {
        WorkerTableBase* oWorkerTable = oWorkerTables->at(i);
        if (oWorkerTable->typeSimpleName() == StrawAddressWorker::TYPESIMPLENAME()) {
            StrawAddressWorkerTable* oStrawAddressWorkerTable = reinterpret_cast<StrawAddressWorkerTable*>(oWorkerTable);
            std::vector<const StrawMeasure*> straws = CxContainer::keys( strawChangeTypes, MeasureChangeType_Delete);
            oStrawAddressWorkerTable->measuresBeforeDelete(straws);
        }
    }
}

void ProtocolBase::measures_valueChanged(const std::map<const StrawMeasure *, std::string> &strawOldValues, int iMeasureSonType, int iSourceId, int iTag)
{
    if (iSourceId != protocolId()) {
        strawsValueChanged(strawOldValues, g_fn_strawTypeEnum(iMeasureSonType), iSourceId, iTag);
    }
}

//开始
void ProtocolBase::start()
{
    //消息队列初始化  调用顺序有问题？？？  百思不得 20140311
//    if (_mqPort > 0)
//    {
//        _qMsg.init(_mqPort);
//    }

    for (size_t i = 0; i < _workerTables->size(); ++i) {
        WorkerTableBase* oWorkerTable = _workerTables->at(i);
        oWorkerTable->refreshStatisticses();
    }
    for (size_t i = 0; i < _caseses->size(); ++i) {
        CasesBase* oCases = _caseses->at(i);
        oCases->start();
    }
    for (size_t i = 0; i < _cycles->size(); ++i) {
        CycleBase* oCycle = _cycles->at(i);
        oCycle->start();
    }
    _isStart = true;
    _checkProtocolTm->start();

    protocolAfterStart();

    if (_channel) {
        _channel->setAutoOpenInterval(10000);
        _channel->open();
    }

    _sendTm->setInterval(_outInterval);
    _sendTm->start();

    _workTm->setInterval(_workInterval);
    _workTm->start();
}

void ProtocolBase::stop()
{
    _checkProtocolTm->stop();
    _sendTm->stop();
    _workTm->stop();

    for (size_t i = 0; i < _caseses->size(); ++i)
    {
        CasesBase* oCases = _caseses->at(i);
        oCases->stop();
    }

    for (size_t i = 0; i < _cycles->size(); ++i)
    {
        CycleBase* oCycle = _cycles->at(i);
        oCycle->stop();
    }

    if (_channel)
    {
        _channel->setAutoOpenInterval(0);
        _channel->close();
    }

    _isStart = false;

    protocolAfterStop();
}

void ProtocolBase::push(const uchar *pData, int iLength)
{
    if((iLength<1)||(pData==NULL)) return;

    if(iLength<=GCL_LEN_DATA_EM)
    {
        _dtIn.setBuffer((uint32)iLength, (char*)pData);
        _qDataEm.push(_dtIn);
    }
    else
    {
        int num  = iLength / GCL_LEN_DATA_EM;
        int res  = iLength % GCL_LEN_DATA_EM;
        if(num>0)
        {
            for(int i=0;i<num;++i)
            {
               push(pData + i * GCL_LEN_DATA_EM, GCL_LEN_DATA_EM);
            }
        }
        if(res>0) push(pData + num * GCL_LEN_DATA_EM, res);
    }
}
void ProtocolBase::push(const char *pData, int iLength)
{
    if((iLength<1)||(pData==NULL)) return;

    if(iLength<=GCL_LEN_DATA_EM)
    {
        _dtIn.setBuffer((uint32)iLength,(char*)pData);
        _qDataEm.push(_dtIn);
    }
    else
    {
        int num  = iLength/GCL_LEN_DATA_EM;
        int res  = iLength%GCL_LEN_DATA_EM;
        if(num>0)
        {
            for(int i=0;i<num;++i)
            {
               push(pData+i*GCL_LEN_DATA_EM,GCL_LEN_DATA_EM);
            }
        }
        if(res>0) push(pData+num*GCL_LEN_DATA_EM,res);
    }
}

void ProtocolBase::regToRTDB()
{
}

void ProtocolBase::sendMsg(uint32 cmd,uint32 fc,uint32 len,char *pBuf)
{
    GCCommon::GCDataElement dt(protocolId(),cmd,fc,len,pBuf);

    memset(_pBuffer,0,_bufLen);
    if(dt.toBuffer(_pBuffer))
    {
//        _qMsg.writeData(_pBuffer,dt.size(),GclConfig::mqIp(),GclConfig::mqPort());
    }
}

void ProtocolBase::sendInfo(int level,std::string sInfo,int type)
{
//    const char *s = QTextCodec::codecForName("system")->fromUnicode(sInfo).constData();

    GCCommon::GCData mData(level,type,sInfo.length(),sInfo.c_str());

    memset(_pBuffer,0,_bufLen);
    if(mData.toBuffer(_pBuffer))
    {
        sendMsg(GCL_CMD_SYS_INFO,0,mData.size(),_pBuffer);
    }
}


void ProtocolBase::reset(int maxTimes)
{
    _failTimes++;
    if(_failTimes>maxTimes)
    {
        _revBuffer.clear();
        _failTimes = 0;
    }
}
//卢良斌 add 20161125
void ProtocolBase::measureFromContext(const CxIGetSkv &context)
{
    _iMonsbIdBegin = 0;
    _iMonsbCount = 0;
    _iYcaddIdBegin = 0;
    _iYcaddCount = 0;
    _iStrawIdBegin = 0;
    _iStrawCount = 0;
    _iMonsbBeginFix = 0;
    _iYcaddBeginFix = 0;
    _iStrawBeginFix = 0;

    _iMonsbIdBegin  = context.getValue(CS_EntryMonsbBegin, _iMonsbIdBegin);
    _iMonsbCount    = context.getValue(CS_EntryMonsbCount, _iMonsbCount);
    _iYcaddIdBegin  = context.getValue(CS_EntryYcaddBegin, _iYcaddIdBegin);
    _iYcaddCount    = context.getValue(CS_EntryYcaddCount, _iYcaddCount);
    _iStrawIdBegin  = context.getValue(CS_EntryStrawBegin, _iStrawIdBegin);
    _iStrawCount    = context.getValue(CS_EntryStrawCount, _iStrawCount);
    _iMonsbBeginFix = context.getValue(CS_EntryMonsbBeginFix, _iMonsbBeginFix);
    _iYcaddBeginFix = context.getValue(CS_EntryYcaddBeginFix, _iYcaddBeginFix);
    _iStrawBeginFix = context.getValue(CS_EntryStrawBeginFix, _iStrawBeginFix);

}
//状态报告
void ProtocolBase::reportTerminalStatus()
{
    setFixMonsbs();
    setFixYcadds();
    setFixStraws();
}

void ProtocolBase::setFixMonsbs()
{
    if (_iMonsbIdBegin<0x01000000 || _iMonsbCount<=0)
        return;

#ifdef GM_MEASURE_MANAGER
    int iTid = terminalId();
    MonsbMeasure::beginSetValues(0, iTid);
    const std::vector<MonsbMeasure*> * oMeasures = MonsbManager::measures();
    MonsbMeasure * oMeasure;
    if ((_iMonsbBeginFix+GCL_Measure_Index_HeartJump) < oMeasures->size())
    {
        oMeasure = oMeasures->at(_iMonsbBeginFix+GCL_Measure_Index_HeartJump);
        oMeasure->setValue((CxTime::currentSepoch()%2)+1,iTid); //1,2
        oMeasure->setQuality(GCL_Q_GOOD);
    }
    if ((_iMonsbBeginFix+GCL_Measure_Index_OnLine) < oMeasures->size())
    {
        oMeasure = oMeasures->at(_iMonsbBeginFix+GCL_Measure_Index_OnLine);
        oMeasure->setValue((int)(isOnline())+1,iTid); //1,2
        oMeasure->setQuality(GCL_Q_GOOD);
    }
    MonsbMeasure::endSetValues();
#else
    int iResult = 0;

    vector<gcd_address_int32_vqt_t> vv;

    msepoch_t dtNow = CxTime::currentMsepoch();

    gcd_address_int32_vqt_t v;

    v.address = _iMonsbIdBegin +_iMonsbBeginFix+ GCL_Measure_Index_HeartJump;
    v.value = CxTime::currentSepoch()%2+1;
    v.quality = GCL_Q_GOOD;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = _iMonsbIdBegin +_iMonsbBeginFix+ GCL_Measure_Index_OnLine;
    v.value = (int)(isOnline())+1;
    v.quality = GCL_Q_GOOD;
    v.datetime = dtNow;
    vv.push_back(v);

    iResult += GeneralAccess::postRealtimeDataPost(ci_yx_address_int32_vqt_t, (char*)(&vv.front()), vv.size()*sizeof(gcd_address_int32_vqt_t), vv.size(), GeneralAccess::defaultAttach());
//    cxDebug() << CxString::format("send yx by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, yxs.size(), yxs.size()*sizeof(gcd_address_int32_vqt_t));
#endif
}

void ProtocolBase::setFixStraws()
{
    if (_iStrawIdBegin < 0x03000000 || _iStrawCount <= 0)
        return;
    int iTid = terminalId();;
#ifdef GM_MEASURE_MANAGER
    const std::vector<StrawMeasure*> * oStraws = StrawManager::measures();
    StrawMeasure * oStrawInfo = NULL;
    if ((_iStrawBeginFix+GCL_Measure_Index_Info) < oStraws->size())
    {
        oStrawInfo = oStraws->at(_iStrawBeginFix+GCL_Measure_Index_Info);
    }

    if (oStrawInfo && oStrawInfo->value().empty())
    {
        string      sName = protocolName();

        string sInfo = CxString::format("name=%s;tid=%d;",sName.c_str(),iTid);

        StrawMeasure::beginSetValues(0, iTid);
        oStrawInfo->setValue(sInfo,iTid);
        oStrawInfo->setQuality(GCL_Q_GOOD);
        StrawMeasure::endSetValues();
    }
#else
    int iResult = 0;

    vector<gcd_address_vqt_string128_t> yws;

    msepoch_t dtNow = CxTime::currentMsepoch();

    string sName = protocolName();
    string sInfo = CxString::format("name=%s;tid=%d;",sName.c_str(),iTid);

    gcd_address_vqt_string128_t yw;
    yw.address = _iStrawIdBegin + GCL_Measure_Index_Info;
    if (sInfo.size()<128)
    {
        memcpy(yw.value, sInfo.data(), sInfo.size());
        yw.value[sInfo.size()] = 0;
    }
    yw.quality = 1;
    yw.datetime = dtNow;
    yws.push_back(yw);

    iResult += GeneralAccess::postRealtimeDataPost(ci_yw_address_vqt_string128_t, (char*)(&yws.front()), yws.size()*sizeof(gcd_address_vqt_string128_t), yws.size(), GeneralAccess::defaultAttach());
//    cxDebug() << CxString::format("send yc by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, ycs.size(), ycs.size()*sizeof(gcd_address_double_vqt_t));
#endif
}

void ProtocolBase::setFixYcadds()
{
    if (_iYcaddIdBegin<0x02000000 || _iYcaddCount<=0)
        return;

    int64       iReceivedByteCount = 0;
    int64       iSentByteCount = 0;
    sepoch_t   iLastReceivedTime = 0;
    sepoch_t   iLastSentTime = 0;
    const CxChannelBase * oChannel = channel();
    if (oChannel)
    {
        iReceivedByteCount  = oChannel->receivedByteCount();
        iSentByteCount      = oChannel->sentByteCount    ();
        iLastReceivedTime   = oChannel->lastReceivedTime () / 1000;
        iLastSentTime       = oChannel->lastSentTime     () / 1000;
    }

#ifdef GM_MEASURE_MANAGER
    int iTid = terminalId();

    YcaddMeasure::beginSetValues(0, iTid);
    const std::vector<YcaddMeasure*> * oMeasures = YcaddManager::measures();

    YcaddMeasure * oMeasure;

    if ((_iYcaddBeginFix+GCL_Measure_Index_NowTime) < oMeasures->size())
    {
        oMeasure = oMeasures->at(_iYcaddBeginFix+GCL_Measure_Index_NowTime);
        oMeasure->setValue(CxTime::currentSepoch(),iTid);
        oMeasure->setQuality(GCL_Q_GOOD);
    }

    if ((_iYcaddBeginFix+GCL_Measure_Index_LastReceivedTime) < oMeasures->size())
    {
        oMeasure = oMeasures->at(_iYcaddBeginFix+GCL_Measure_Index_LastReceivedTime);
        oMeasure->setValue(iLastReceivedTime,iTid);
        oMeasure->setQuality(GCL_Q_GOOD);
    }
    if ((_iYcaddBeginFix+GCL_Measure_Index_LastSentTime) < oMeasures->size())
    {
        oMeasure = oMeasures->at(_iYcaddBeginFix+GCL_Measure_Index_LastSentTime);
        oMeasure->setValue(iLastSentTime,iTid);
        oMeasure->setQuality(GCL_Q_GOOD);
    }

    if ((_iYcaddBeginFix+GCL_Measure_Index_ReceivedByteCount) < oMeasures->size())
    {
        oMeasure = oMeasures->at(_iYcaddBeginFix+GCL_Measure_Index_ReceivedByteCount);
        oMeasure->setValue(iReceivedByteCount,iTid);
        oMeasure->setQuality(GCL_Q_GOOD);
    }

    if ((_iYcaddBeginFix+GCL_Measure_Index_SentByteCount) < oMeasures->size())
    {
        oMeasure = oMeasures->at(_iYcaddBeginFix+GCL_Measure_Index_SentByteCount);
        oMeasure->setValue(iSentByteCount,iTid);
        oMeasure->setQuality(GCL_Q_GOOD);
    }



    YcaddMeasure::endSetValues();
#else
    int iResult = 0;

    vector<gcd_address_double_vqt_t> vv;

    msepoch_t dtNow = CxTime::currentMsepoch();

    gcd_address_double_vqt_t v;

    v.address = _iYcaddIdBegin +_iYcaddBeginFix+ GCL_Measure_Index_NowTime;
    v.value = CxTime::currentSepoch();
    v.quality = GCL_Q_GOOD;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = _iYcaddIdBegin +_iYcaddBeginFix+ GCL_Measure_Index_HeartJump;
    v.value = CxTime::currentSepoch()%2+1;
    v.quality = GCL_Q_GOOD;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = _iYcaddIdBegin +_iYcaddBeginFix+ GCL_Measure_Index_LastReceivedTime;
    v.value = iLastReceivedTime;
    v.quality = GCL_Q_GOOD;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = _iYcaddIdBegin +_iYcaddBeginFix+ GCL_Measure_Index_LastSentTime;
    v.value = iLastSentTime;
    v.quality = GCL_Q_GOOD;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = _iYcaddIdBegin +_iYcaddBeginFix+ GCL_Measure_Index_ReceivedByteCount;
    v.value = iReceivedByteCount;
    v.quality = GCL_Q_GOOD;
    v.datetime = dtNow;
    vv.push_back(v);

    v.address = _iYcaddIdBegin +_iYcaddBeginFix+ GCL_Measure_Index_SentByteCount;
    v.value = iSentByteCount;
    v.quality = GCL_Q_GOOD;
    v.datetime = dtNow;
    vv.push_back(v);

    iResult += GeneralAccess::postRealtimeDataPost(ci_yc_address_double_vqt_t, (char*)(&vv.front()), vv.size()*sizeof(gcd_address_double_vqt_t), vv.size(), GeneralAccess::defaultAttach());
//    cxDebug() << CxString::format("send yx by GeneralAccess.psm: result=%d, count=%d, size=%d", iResult, yxs.size(), yxs.size()*sizeof(gcd_address_int32_vqt_t));
#endif
}
