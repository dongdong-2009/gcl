#include "hkvs_dec_protocol.h"
//共享内存
#include "../gcsharemem/GCRTShareMemWorker.h"

HKVSDECProtocolFactory* HKVSDECProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<HKVSDECProtocolFactory>();

HKVSDECProtocol::HKVSDECProtocol()
{
    _yxRecTable  = createAndRegisterWorkerTable<MonsbAddressWorkerTable, HKVSDECProtocol>("YxReceiveTable", MonsbType_rtu_state);
    _ykRecTable  = createAndRegisterWorkerTable<MonsbAddressWorkerTable, HKVSDECProtocol>("YkReceiveTable", MonsbType_rtu_state);
    _ycRecTable  = createAndRegisterWorkerTable<YcaddAddressWorkerTable, HKVSDECProtocol>("YcReceiveTable", YcaddType_rtu_value);

    _yxSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, HKVSDECProtocol>("YxSendTable", MonsbType_rtu_state);
    _ykSendTable = createAndRegisterWorkerTable<MonsbAddressWorkerTable, HKVSDECProtocol>("YkSendTable", MonsbType_rtu_state);
    _ycSendTable = createAndRegisterWorkerTable<YcaddAddressWorkerTable, HKVSDECProtocol>("YcSendTable", YcaddType_rtu_value);

    _sendMcontrol= createAndRegisterCases<HKVSDECProtocol, SendMcontrolCases>(this);

    _timeCallUp       = new CxTimer();
    _timeCallUp      ->setTimerTimeOut(this);

    _callUpInterval = 50000;

    _started = false;
    _ykInitiative = true;

    _pSin->AddInfo("HKVSDECProtocol::HKVSDECProtocol",protocolName());
}

HKVSDECProtocol::~HKVSDECProtocol()
{
    delete _timeCallUp;

}

void HKVSDECProtocol::toContext(CxISetSkv& context) const
{
    ProtocolBase::toContext(context);
    context.setValue(CS_EntryReqInterval, _callUpInterval);

    _pSin->AddInfo("HKVSDECProtocol::ToContext",protocolName());
}

void HKVSDECProtocol::fromContext(const CxIGetSkv& context)
{
    ProtocolBase::fromContext(context);
    int iInterval = 0;
    iInterval = context.getValue(CS_EntryReqInterval ,iInterval);
    if (iInterval != 0 && iInterval > 1000) {
        _callUpInterval = iInterval;
    }
    _pSin->AddInfo("HKVSDECProtocol::FromContext",protocolName());

}

void HKVSDECProtocol::toReport( std::vector<std::string>& sReports) const
{
    sReports.push_back(std::string("CallUpInterval : ") + CxString::toString(_callUpInterval));
}

void HKVSDECProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
}

int HKVSDECProtocol::sendControl(const MonsbMeasure *oMonsb, int fromValue, int toValue, SendControlArgvs argvs)
{
    if (! _ykInitiative) return SendControlFail_Unknow;
     return -1;
}

bool HKVSDECProtocol::provideSendControl(int iMeasureSonType, const MonsbMeasure *oMonsb)
{
    if (iMeasureSonType == MonsbType_rtu_state) {
        if (oMonsb) {
            return _ykSendTable->workerByMeasure(oMonsb);
        } else {
            return true;
        }
    } else {
        return false;
    }
}

void HKVSDECProtocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
    if (oTimer == _timeCallUp)
    {
        if (! _started) {
            clock();
            _started = true;
        } else {
            callUp();
        }
    }
}

void HKVSDECProtocol::protocolReceivedData(const uchar *pData, int iLength)
{

}

void HKVSDECProtocol::channelAfterOpen(const CxChannelBase* oChannel)
{
    _started = false;
}

void HKVSDECProtocol::channelAfterClose(const CxChannelBase* oChannel)
{
    _started = false;
}

void HKVSDECProtocol::callUp()
{
}



int HKVSDECProtocol::sendYK( uchar funcode, ushort reason, ushort stationId, int address, bool gotoHe, bool isPerform)
{
    return 0;
}

void HKVSDECProtocol::dealData()
{
    //收到数据
    receivedProtocolDataNow();
}

void HKVSDECProtocol::resetCalcer ( )
{
}

void HKVSDECProtocol::dealYX ( )
{
    if(_yxRecTable == NULL || _yxRecTable->addressCount()<1) return;
}


void HKVSDECProtocol::dealYC ( )
{
    if(_ycRecTable==NULL || _ycRecTable->addressCount()<1) return;
}

void HKVSDECProtocol::dealYK()
{
    //收表为空
    if(_ykRecTable==NULL || _ykRecTable->addressCount()<1) return;

}

void HKVSDECProtocol::dealYK ( int stationId, int address, uchar ation)
{
}

void HKVSDECProtocol::dealCallUp ( ushort wStationId )
{
}
void HKVSDECProtocol::protocolAfterStart()
{
    _started = false;
    _bRtShMStatus = false;
#ifdef GMAC_SHARE_MEMORY
    //打开共享内存区
    int nRet = GCRTShareMemWorker::open(GC_RTSHM_MODE_OPEN);
    if(nRet == GCL_SUCCED)
    {
        //获取共享内存区信息
        nRet = GCRTShareMemWorker::getCtrlArea(protocolName());
        if(nRet == GCL_SUCCED)
        {   //获取位置信息
            GCRTShareMemWorker::getMonsbPosById(_yxSendTable,protocolName());
            GCRTShareMemWorker::getMonsbPosById(_yxRecTable,protocolName());
            GCRTShareMemWorker::getYcaddPosById(_ycRecTable,protocolName());
            GCRTShareMemWorker::getYcaddPosById(_ycSendTable,protocolName());
        }
    }
    if(nRet == GCL_SUCCED)
    {
        _bRtShMStatus = true;
    }
    cxDebug()<<"GCRTShareMemWorker Init:"<< nRet <<cxEndLine;

#endif

    _timeCallUp->setInterval(_callUpInterval);
    _timeCallUp->start();

    clock();
    //注册
    regToRTDB();
}

void HKVSDECProtocol::protocolAfterStop()
{
    _timeCallUp->stop();
}

void HKVSDECProtocol::monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag)
{
}

void HKVSDECProtocol::ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag)
{
}

void HKVSDECProtocol::doTask(GCCommon::GCDataElement dt)
{
    switch(dt.Type())
    {
    case GCL_CMD_SYS_CLOCK://校时
        break;
    case GCL_CMD_SYS_POWER://电源控制
        break;
    case GCL_CMD_SYS_INFO://信息
        break;
    default:
        break;
    }
}

void HKVSDECProtocol::clock()
{
}

