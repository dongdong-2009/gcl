#include "psm_protocol.h"
#include "protocol_config.h"

PsmProtocolFactory* fn_regPsmProtocolFactory()
{
    ProtocolFactoryManager::createAndRegister<PsmProtocolFactory>();
    return NULL;
}

PsmProtocolFactory* PsmProtocolFactory::s_instance = fn_regPsmProtocolFactory();


using namespace std;



#define DealReplyType_Error_Data (-1)


PsmProtocol::PsmProtocol()
{
    _psmExplainer.registerPsmExplainNotify(this);
    _psmExplainer.registerPsmExplainWrite(this);

    _heartJumpTimer = new CxTimer();
    _heartJumpTimer->setTimerTimeOut(this);

    createAndRegisterCases<PsmProtocol, SendMessageCases>(this);
    createAndRegisterCases<PsmProtocol, SendFileCases>(this);

    _cfgSentReason = 0;
    _cfgSentContainerId = 0;
    _cfgSentSourceId = 0;
    _cfgSentTargetId = 0;
    _cfgSentTag = 0;

    _cfgHeartJumpInterval = 3000;
    _cfgSendYxInterval = 21000;
    _cfgSendYcInterval = 22000;
    _cfgSendYwInterval = 23000;

}

PsmProtocol::~PsmProtocol()
{
    _psmExplainer.stop();
    _psmExplainer.registerPsmExplainNotify(NULL);

    _psmExplainer.registerPsmExplainWrite(NULL);

    delete _heartJumpTimer;
}

void PsmProtocol::toContext(CxISetSkv& context) const
{
    ProtocolBase::toContext(context);

    context.setValue(CS_EntryPsmSentReason, _cfgSentReason);
    context.setValue(CS_EntryPsmSentContainerId, _cfgSentContainerId);
    context.setValue(CS_EntryPsmSentSourceId, _cfgSentSourceId);
    context.setValue(CS_EntryPsmSentTargetId, _cfgSentTargetId);
    context.setValue(CS_EntryPsmSentTag, _cfgSentTag);

    context.setValue(CS_EntryPsmHeartJumpInterval, _cfgHeartJumpInterval);
    context.setValue(CS_EntryPsmYxSendInterval, _cfgSendYxInterval);
    context.setValue(CS_EntryPsmYcSendInterval, _cfgSendYcInterval);
    context.setValue(CS_EntryPsmYwSendInterval, _cfgSendYwInterval);
}

void PsmProtocol::fromContext(const CxIGetSkv& context)
{
    ProtocolBase::fromContext(context);

    _cfgSentReason = context.getValue(CS_EntryPsmSentReason, _cfgSentReason);
    _cfgSentContainerId = context.getValue(CS_EntryPsmSentContainerId, _cfgSentContainerId);

    if (context.hasEntry(CS_EntryPsmSentSourceId))
        _cfgSentSourceId = context.getValue(CS_EntryPsmSentSourceId, _cfgSentSourceId);
    else
        _cfgSentSourceId = context.getValue(CS_EntryPsmStationNumSource, _cfgSentSourceId);
    if (_cfgSentSourceId <= 0 && _terminalId > 0)
    {
        _cfgSentSourceId = _terminalId;
    }
    if (context.hasEntry(CS_EntryPsmSentTargetId))
        _cfgSentTargetId = context.getValue(CS_EntryPsmSentTargetId, _cfgSentTargetId);
    else
        _cfgSentTargetId = context.getValue(CS_EntryPsmStationNumTarget, _cfgSentTargetId);
    _cfgSentTag = context.getValue(CS_EntryPsmSentTag, _cfgSentTag);

    cxDebug() << _cfgSentSourceId;

    _cfgHeartJumpInterval = context.getValue(CS_EntryPsmHeartJumpInterval, _cfgHeartJumpInterval);
    _cfgSendYxInterval = context.getValue(CS_EntryPsmYxSendInterval, _cfgSendYxInterval);
    _cfgSendYcInterval = context.getValue(CS_EntryPsmYcSendInterval, _cfgSendYcInterval);
    _cfgSendYwInterval = context.getValue(CS_EntryPsmYwSendInterval, _cfgSendYwInterval);

    _defaultAttach.reason = _cfgSentReason;
    _defaultAttach.containerId = _cfgSentContainerId;
    _defaultAttach.sourceId = _cfgSentSourceId;
    _defaultAttach.targetId = _cfgSentTargetId;
    _defaultAttach.tag = _cfgSentTag;


}

void PsmProtocol::toReport(std::vector<std::string> &sReports) const
{
    ProtocolCommon::toReport(sReports);
    sReports.push_back(CxString::format("%s=%d", CS_EntryPsmSentReason.c_str(), _cfgSentReason));
    sReports.push_back(CxString::format("%s=%d", CS_EntryPsmSentContainerId.c_str(), _cfgSentContainerId));
    sReports.push_back(CxString::format("%s=%d", CS_EntryPsmSentSourceId.c_str(), _cfgSentSourceId));
    sReports.push_back(CxString::format("%s=%d", CS_EntryPsmSentTargetId.c_str(), _cfgSentTargetId));
    sReports.push_back(CxString::format("%s=%d", CS_EntryPsmSentTag.c_str(), _cfgSentTag));
    sReports.push_back(CxString::format("%s=%d", CS_EntryPsmHeartJumpInterval.c_str(), _cfgHeartJumpInterval));
    sReports.push_back(CxString::format("%s=%d", CS_EntryPsmYxSendInterval.c_str(), _cfgSendYxInterval));
    sReports.push_back(CxString::format("%s=%d", CS_EntryPsmYcSendInterval.c_str(), _cfgSendYcInterval));
    sReports.push_back(CxString::format("%s=%d", CS_EntryPsmYwSendInterval.c_str(), _cfgSendYwInterval));
}

void PsmProtocol::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    ProtocolBase::describeSelf(oDescribeUnit);
    CxDescribeInteger* oSentReasonDescribe      = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryPsmSentReason ); oSentReasonDescribe->initValue(_cfgSentReason  );
    CxDescribeInteger* oSentContainerIdDescribe      = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryPsmSentContainerId ); oSentContainerIdDescribe->initValue(_cfgSentContainerId  );
    CxDescribeInteger* oSentSourceIdDescribe      = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryPsmSentSourceId ); oSentSourceIdDescribe->initValue(_cfgSentSourceId  );
    CxDescribeInteger* oSentTargetIdDescribe      = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryPsmStationNumTarget ); oSentTargetIdDescribe->initValue(_cfgSentTargetId  );
    CxDescribeInteger* oSentTagDescribe      = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryPsmStationNumTarget ); oSentTagDescribe->initValue(_cfgSentTag  );

    CxDescribeInteger* oHeartJumpIntervalDescribe     = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryPsmHeartJumpInterval); oHeartJumpIntervalDescribe->initValue(_cfgHeartJumpInterval);
    CxDescribeInteger* oYxSendIntervalDescribe        = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryPsmYxSendInterval);    oYxSendIntervalDescribe->initValue(_cfgSendYxInterval);
    CxDescribeInteger* oYcSendIntervalDescribe        = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryPsmYcSendInterval);    oYcSendIntervalDescribe->initValue(_cfgSendYcInterval);
    CxDescribeInteger* oYwSendIntervalDescribe        = oDescribeUnit->createAndRegister<CxDescribeInteger>(CS_EntryPsmYwSendInterval);    oYwSendIntervalDescribe->initValue(_cfgSendYwInterval);
}

void PsmProtocol::protocolReceivedData(const uchar *pData, int iLength)
{
    if (_psmExplainer.receivedData(pData, iLength) > 0)
    {
        //连接为真
        setConnect(true);
        //接收时间
        receivedProtocolDataNow();
    }
}

int PsmProtocol::dealYK_01(const std::string & sParam)
{
    if(_ykRecTable==NULL) return -1;


    std::map<string, string> map1 =  CxString::splitToMap(sParam,'=','\n');

    std::string sKey = "mode";
    int mode    =  CxString::toUint32(CxContainer::value(map1,sKey));

    sKey = "address";
    int addr    =  CxString::toUint32(CxContainer::value(map1,sKey));

    if(mode == 1)
    {
        MonsbMeasure *oMeasure = _ykRecTable->measureSonByAddress(addr);
        if(oMeasure!=NULL)
        {
            outProtocolPrompt()<<"YK succeed:"<<addr<<oMeasure->measureId()<<cxEndLine;
            sendMsg(GCL_CMD_SYS_YK,oMeasure->measureId(),0,NULL);
        }else{
            outProtocolPrompt()<<"YK Fail:YK Table is not found:"<<addr<<cxEndLine;
        }
    }
    return 0;
}
//标度化值
int PsmProtocol::dealYT_01(const std::string & sParam)
{
    if(_ycRecTable==NULL) return -1;

    std::map<string, string> map1 =  CxString::splitToMap(sParam,'=','\n');

    std::string sKey = "mode";
    int mode    =  CxString::toUint32(CxContainer::value(map1,sKey));

    sKey = "address";
    int addr    =  CxString::toUint32(CxContainer::value(map1,sKey));

    sKey = "tovalue";
    int toValue = CxString::fromString(CxContainer::value(map1,sKey), ci_int_zero);

    if(mode == 1)
    {
        YcaddMeasure *oMeasure= _ycRecTable->measureSonByAddress(addr);
        if(oMeasure!=NULL)
        {
            outProtocolPrompt()<<"YT succeed:"<<addr<<oMeasure->measureId()<<cxEndLine;
            sendMsg(GCL_CMD_SYS_YK,oMeasure->measureId(),sizeof(int),(char*)&toValue);
        }else{
            outProtocolPrompt()<<"YK Fail:YK Table is not found:"<<addr<<cxEndLine;
        }
    }
    return 0;
}

int PsmProtocol::dealYS_01(const std::string & sParam)
{
    if(_ywRecTable==NULL) return -1;

    std::map<string, string> map1 =  CxString::splitToMap(sParam,'=','\n');
    std::string sKey = "mode";
    int mode    =  CxString::toUint32(CxContainer::value(map1,sKey));

    sKey = "address";
    int addr    =  CxString::toUint32(CxContainer::value(map1,sKey));

    sKey = "tovalue";
    string toValue = CxContainer::value(map1,sKey);

    if(mode == 1)
    {
        StrawMeasure *oMeasure= _ywRecTable->measureSonByAddress(addr);
        if(oMeasure!=NULL)
        {
            outProtocolPrompt()<<"YW succeed:"<<addr<<oMeasure->measureId()<<cxEndLine;
            sendMsg(GCL_CMD_SYS_YK,oMeasure->measureId(),toValue.length(),(char*)toValue.data());
        }else{
            outProtocolPrompt()<<"YK Fail:YK Table is not found:"<<addr<<cxEndLine;
        }
    }
    return 0;
}

int PsmProtocol::dealYX_01(const std::string & sParam)
{
    map<MonsbAddressWorker*, int> workerValues;
    std::map<string, string> yxes =  CxString::splitToMap(sParam,'=','\n');
    for (map<string, string>::const_iterator it = yxes.begin(); it != yxes.end(); ++it)
    {
        MonsbAddressWorker * oWorker = _yxRecTable->workerByCode(it->first);
        if (oWorker)
        {
            int iState = CxString::fromString(it->second, ci_int_minus_one);
            workerValues[oWorker] = iState;
        }
    }
    _yxRecTable->receiveWorkerValues(workerValues, protocolId(), 0);
    return workerValues.size();
}

int PsmProtocol::dealYC_01(const std::string & sParam)
{
    map<YcaddAddressWorker*, double> workerValues;
    std::map<string, string> yces =  CxString::splitToMap(sParam,'=','\n');
    for (map<string, string>::const_iterator it = yces.begin(); it != yces.end(); ++it)
    {
        YcaddAddressWorker * oWorker = _ycRecTable->workerByCode(it->first);
        if (oWorker)
        {
            double dValue = CxString::fromString(it->second, cd_double_zero);
            workerValues[oWorker] = dValue;
        }
    }
    _ycRecTable->receiveWorkerValues(workerValues, protocolId(), 0);
    return workerValues.size();
}

int PsmProtocol::dealYM_01(const std::string & sParam)
{
    return -1;
}

int PsmProtocol::dealVTL_01(const std::string & sParam)
{
    return -1;
}

int PsmProtocol::dealMessage_01(const std::string & sParam)
{
    return -1;
}

int PsmProtocol::sendYK(uint32 id)
{
    return -1;
}

int PsmProtocol::sendSC(uint32 id)
{
    return -1;
}

int PsmProtocol::containerId(const char * sContainerCode)
{
    //musttodo
    return 0;
}

std::string PsmProtocol::containerCode(int iContainerId)
{
    //musttodo
    return "";
}

int PsmProtocol::postMessageCommand(int iTargetId, const char *sCommand, const char *sParam, int iParamLength)
{
    PsmAttach attach(_defaultAttach);
    attach.targetId = iTargetId;
    return _psmExplainer.postMessageCommand(sCommand, sParam, iParamLength, &attach);
}

int PsmProtocol::postMessageCommand(const char *sCommand, const char *sParam, int iParamLength, const PsmAttach * oAttach)
{
    if (oAttach)
    {
        return _psmExplainer.postMessageCommand(sCommand, sParam, iParamLength, oAttach);
    }
    else
    {
        return _psmExplainer.postMessageCommand(sCommand, sParam, iParamLength, &_defaultAttach);
    }
}

int PsmProtocol::postFile(int iTargetId, const string &sFilePath)
{
    PsmAttach attach(_defaultAttach);
    attach.targetId = iTargetId;
    return _psmExplainer.postFile(sFilePath,  &attach);
}

int PsmProtocol::postFile(const std::string &sFilePath, const PsmAttach * oAttach)
{
    //    outProtocolPrompt()<<"postFile"<<sFilePath<<cxEndLine;
    if(oAttach)
    {
        return _psmExplainer.postFile(sFilePath, oAttach);
    }
    else
    {
        return _psmExplainer.postFile(sFilePath,  &_defaultAttach);
    }
}

int PsmProtocol::postRealtimeDataRequest(int iTargetId)
{
    PsmAttach attach(_defaultAttach);
    attach.targetId = iTargetId;
    return _psmExplainer.postData(gct_realtime_data_request, 0, 0, 0, 0, &attach);
}

int PsmProtocol::postRealtimeDataRequest(const PsmAttach * oAttach)
{
    if(oAttach)
    {
        return _psmExplainer.postData(gct_realtime_data_request, 0, 0, 0, 0, oAttach);
    }
    else
    {
        return _psmExplainer.postData(gct_realtime_data_request, 0, 0, 0, 0, &_defaultAttach);
    }
}

int PsmProtocol::postRealtimeDataPost(int iTargetId, int iParamType, const char *pData, int iDataLength, int iParamCount)
{
    PsmAttach attach(_defaultAttach);
    attach.targetId = iTargetId;
    return _psmExplainer.postData(gct_realtime_data_post, iParamType, pData, iDataLength, iParamCount, &attach);
}

int PsmProtocol::postRealtimeDataPost(int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach)
{
    if(oAttach)
    {
        return _psmExplainer.postData(gct_realtime_data_post, iParamType, pData, iDataLength, iParamCount, oAttach);
    }
    else
    {
        return _psmExplainer.postData(gct_realtime_data_post, iParamType, pData, iDataLength, iParamCount, &_defaultAttach);
    }
}

int PsmProtocol::send_data(const char *pData, int iLength, int iTargetId)
{
    return writeData(pData, iLength);
}

void PsmProtocol::monsbsValueChanged(const std::map<const MonsbMeasure *, int> &monsbOldValues, MonsbTypeEnum eMonsbType, int iSource, int iTag)
{
    if (monsbOldValues.size() > 0 && eMonsbType == MonsbType_rtu_state) {
        //        std::map<int, int> addressValues = _yxSendTable->addressValues(monsbOldValues, true);
        //        SendStates2(addressValues, FunCode_YX_BianWei);
    }
}

void PsmProtocol::ycaddsValueChanged(const std::map<const YcaddMeasure *, double> &ycaddOldValues, YcaddTypeEnum eYcaddType, int iSource, int iTag)
{
    if (ycaddOldValues.size() > 0 && eYcaddType == YcaddType_rtu_value){
        //        std::map<int, double> addressValues = _ycSendTable->addressValues(ycaddOldValues, true);
        //        SendAnalog2(addressValues, FunCode_YC_BianWei);
    }
}

void PsmProtocol::checkProtocol()
{
    ProtocolCommon::checkProtocol();
}

int PsmProtocol::sendHeartJump()
{
    //设置连接标志
    setConnect(true);
    //发送心跳
    return _psmExplainer.postHeartJump(&_defaultAttach);
}

void PsmProtocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
    ProtocolCommon::protocolTimerTimeOut(oTimer);
    if (oTimer == _heartJumpTimer)
    {
        if ( CxTime::milliSecondDifferToNow(receivedProtocolDataTime()) > _cfgHeartJumpInterval - 200 )
        {
            sendHeartJump();
        }
    }
}

void PsmProtocol::protocolAfterStart()
{
    ProtocolCommon::protocolAfterStart();

    _psmExplainer.setIsProcessQueue(channel()->isProcessQueue());
    _psmExplainer.start();

    _heartJumpTimer->setInterval(_cfgHeartJumpInterval);
    _heartJumpTimer->start();

    _sendMonsbCycle->setSendInterval(_cfgSendYxInterval);
    _sendMonsbCycle->start();
    _sendYcaddCycle->setSendInterval(_cfgSendYcInterval);
    _sendYcaddCycle->start();


}

void PsmProtocol::protocolAfterStop()
{
    _psmExplainer.stop();
    _heartJumpTimer->stop();
    ProtocolCommon::protocolAfterStop();
}
//////////////////////////////////////////////////////////////////////////////////////
//发送所有遥测
int PsmProtocol::sendAllYcadd(YcaddTypeEnum eYcaddType, double * value, SendMeasureArgvs argvs)
{
    if (eYcaddType != YcaddType_rtu_value)
    {
        return  SendMeasureFail_Noprovide;
    }
    if(_bConnect)
    {
        return sendAllYC();
    }
    else return 0;
}

int PsmProtocol::sendAllMonsb(MonsbTypeEnum eMonsbType, int * value, SendMeasureArgvs argvs)
{
    if (eMonsbType != MonsbType_rtu_state) {
        return  SendMeasureFail_Noprovide;
    }
    if(_bConnect)
    {
        int num = senddAllYX();
        num += senddAllYb();
        return num;
    }
    else return 0;

}

void PsmProtocol::doTask(GCCommon::GCDataElement dt)
{
    ProtocolCommon::doTask(dt);

    switch(dt.type())
    {
    case GCL_CMD_SYS_INFO://信息
    {
        if(!_bConnect) break;
        GCCommon::GCData mData;
        if(mData.fromBuffer(dt.buffer(),dt.dataLenth()))
        {
            std::string sInfo = CxString::format("level=%d\ntime=%d\ntype=%d\nmessage=%s",mData.level(),mData.tmTag(),mData.type(),mData.buffer());
            postMessageCommand("send.message.1",sInfo.c_str(),sInfo.length(),NULL);
        }
    }
        break;
    case GCL_CMD_SYS_INFO_TXT://信息
    {
        if(_bConnect)postMessageCommand("send.message.1",dt.buffer(),dt.dataLenth(),NULL);
    }
        break;
    case GCL_CMD_SYS_YX_S://遥信刷新
        if(_bConnect)sendYX(dt.pram());
        break;
    case GCL_CMD_SYS_YC_S:
        if(_bConnect)sendYC(dt.pram());
        break;
    case GCL_CMD_SYS_SC_S:
        if(_bConnect)sendSTR(dt);
        break;
    case GCL_CMD_SYS_SC_W2:
        postMessageCommand("send.yk.str2",dt.buffer(),dt.dataLenth());
        outProtocolDebug()<<protocolName()<<dt.type()<<"send.yk.str2"<<std::string(dt.buffer(),dt.dataLenth())<<cxEndLine;
        break;
    case GCL_CMD_ASM570_REQ_BOTTLE:
        postMessageCommand("req.rfid.1",dt.buffer(),dt.dataLenth());
        outProtocolDebug()<<protocolName()<<dt.type()<<"req.rfid.1"<<std::string(dt.buffer(),dt.dataLenth())<<cxEndLine;
        break;
    case GCL_CMD_SYS_SEND_FILE:
    {
        std::string sFile = std::string(dt.buffer(),dt.dataLenth());
        int nLen = postFile(sFile,NULL);
    }
        break;
    case GCL_CMD_SYS_SEND_FILE_CMD: //0x31,以命令的方式，发送缓冲区内容
    {
        outProtocolDebug()<<string(dt.buffer(), dt.dataLenth())<<cxEndLine;

        std::map<string,string> map1 = CxString::splitToMap(string(dt.buffer(), dt.dataLenth()), '=', '\n');

        string sKey = "command";
        string sCommand; sCommand = CxContainer::value(map1,sKey,sCommand);
        sCommand = CxString::trim(sCommand);
        if(sCommand.empty()) break;

        sKey = "file";
        string sFile; sFile = CxContainer::value(map1,sKey,sFile);
        sFile = CxString::trim(sFile);
        if(sFile.empty()) break;

        sKey = "mode";
        int mode = 0;
        string sMode; sMode = CxContainer::value(map1,sKey,sMode);
        sMode = CxString::trim(sMode);
        if(!sMode.empty())
        {
            mode=CxString::toUint32(sMode);
        }

        string sParam;
        if(mode==0) //相对temp路径
        {
            if(CxFile::load(CxFileSystem::mergeFilePath(GclConfig::tempPath(),sFile), sParam))
            {
                postMessageCommand(sCommand.data(), sParam.data(),sParam.length());
                outProtocolDebug()<<sCommand<<sParam<<cxEndLine;
            }
        }
        else if(mode == 1)//app路径
        {
            if(CxFile::load(CxFileSystem::mergeFilePath(GclConfig::deployPath(),sFile), sParam))
            {
                postMessageCommand(sCommand.data(), sParam.data(),sParam.length());
                outProtocolDebug()<<sCommand<<sParam<<cxEndLine;
            }
        }
        else if(mode == 2)//绝对路径
        {
            if(CxFile::load(sFile,sParam))
            {
                postMessageCommand(sCommand.data(), sParam.data(),sParam.length());
                outProtocolDebug()<<sCommand<<sParam<<cxEndLine;
            }
        }else
        {
            outProtocolDebug()<<"no support mode:"<<mode<<cxEndLine;
        }

    }
        break;
    case GCL_CMD_SYS_MSG_TRAN://消息命令转发
    {
        char *pBuf = dt.buffer();
        int nLen = dt.dataLenth();
        if(nLen>sizeof(uint16))
        {
            uint16 cmdLen = pBuf[0]|(pBuf[1]<<8);
            int pos = sizeof(uint16);
            char cTmp[GCL_LEN_DT];
            memset(cTmp,0,GCL_LEN_DT);

            if( cmdLen<=(nLen-pos) && cmdLen<GCL_LEN_DT)
            {
                memcpy(cTmp,pBuf+pos,cmdLen);

                if((cmdLen+pos)<nLen)
                {
                    postMessageCommand(cTmp,pBuf+pos+cmdLen,nLen-pos-cmdLen,NULL);
                }
                else
                {
                    postMessageCommand(cTmp,NULL,0,NULL);
                }
            }
        }
    }
        break;
    default:
        break;
    }

}

int PsmProtocol::post_message(const string &sCommand, const string &sParam)
{
    return postMessageCommand(sCommand.c_str(), sParam.data(), sParam.size());
}

int PsmProtocol::post_file(const string &sFilePath)
{
    return postFile(sFilePath);
}

void PsmProtocol::received_message_command(const PsmExplainer *oPsm, const char *sCommand, const char *sParam, int iParamLength, const PsmAttach *oAttach)
{
    for (size_t i = 0; i < _psmProtocolNotifies.size(); ++i)
    {
        IPsmProtocolNotify * oIPsmProtocolNotify = _psmProtocolNotifies.at(i);
        oIPsmProtocolNotify->received_message_command(this, sCommand, sParam, iParamLength, oAttach);
    }
}

void PsmProtocol::received_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    for (size_t i = 0; i < _psmProtocolNotifies.size(); ++i)
    {
        IPsmProtocolNotify * oIPsmProtocolNotify = _psmProtocolNotifies.at(i);
        oIPsmProtocolNotify->received_file_write(this, pathInfo, fileData, oAttach);
    }
}

void PsmProtocol::received_realtime_data_request(const PsmExplainer *oPsm, const PsmAttach *oAttach)
{
    for (size_t i = 0; i < _psmProtocolNotifies.size(); ++i)
    {
        IPsmProtocolNotify * oIPsmProtocolNotify = _psmProtocolNotifies.at(i);
        oIPsmProtocolNotify->received_realtime_data_request(this, oAttach);
    }
}

void PsmProtocol::received_realtime_data_post(const PsmExplainer *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    for (size_t i = 0; i < _psmProtocolNotifies.size(); ++i)
    {
        IPsmProtocolNotify * oIPsmProtocolNotify = _psmProtocolNotifies.at(i);
        oIPsmProtocolNotify->received_realtime_data_post(this, iParamType, pData, iDataLength, iParamCount, oAttach);
    }
}

void PsmProtocol::result_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const PsmAttach *oAttach)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief PsmProtocol::senddAllYX 发送所有遥信
/// \return
///
int PsmProtocol::senddAllYX()
{
    if(_yxSendTable==NULL) return SendMeasureFail_Nomeasure;

    const std::vector<MonsbAddressWorker*>* oWorkers = _yxSendTable->workers();
    if (oWorkers->size() < 1)
    {
        //        outProtocolPrompt()<< "_yxSendTable is Null.";
        return  SendMeasureFail_Nomeasure;
    }

    int nSum = 0;
    memset(_sendBuffer,0,GCL_LEN_DATA_EM);

    int nMaxLength = GCL_LEN_DATA_EM>=CI_PSM_PACKET_DATA?(CI_PSM_PACKET_DATA-1):GCL_LEN_DATA_EM;
    int maxNum = nMaxLength / sizeof(gcd_address_vqt_realText15_t);

    gcd_address_vqt_intText1_t *oValue = (gcd_address_vqt_intText1_t*)&_sendBuffer[0];
    int nCount = 0;
    std::string szVal;
    int nLength = 0;
    for (size_t i = 0; i < oWorkers->size(); ++i)
    {
        const MonsbAddressWorker* oWorker = oWorkers->at(i);

        oValue->address = oWorker->address();
        const MonsbMeasure *oMeasure = oWorker->measure();
        if(oMeasure!=NULL)
        {
            oValue->datetime =  oMeasure->lastRefreshTime();
            //字符串型
            szVal = CxString::toString(oMeasure->value());
            nLength = (szVal.length()<=7)?szVal.length():7;
            memcpy(oValue->value,szVal.c_str(),nLength);
            oValue->quality  =  oMeasure->quality();
            oValue++;
            nCount++;
        }
        //发送缓冲区满
        if(nCount>=maxNum)
        {
            nSum += postRealtimeDataPost(gcd_address_vqt_intText1_t::getType(),_sendBuffer,maxNum*sizeof(gcd_address_vqt_intText1_t),maxNum);
            nCount = 0;
            memset(_sendBuffer,0,GCL_LEN_DATA_EM);
            oValue = (gcd_address_vqt_intText1_t*)&_sendBuffer[0];
        }
    }
    if(nCount>0)
    {
        nSum += postRealtimeDataPost(gcd_address_vqt_intText1_t::getType(),_sendBuffer,nCount*sizeof(gcd_address_vqt_intText1_t),nCount);
    }

    //    outProtocolDebug()<<CxTime::currentSepochString()<<protocolId()<<"Send YX" << nSum  <<cxEndLine;

    return nSum;
}
//发送所有字符串
int PsmProtocol::senddAllYb()
{

    if(_ywSendTable==NULL) return SendMeasureFail_Nomeasure;

    const std::vector<StrawAddressWorker*>* oWorkers = _ywSendTable->workers();
    if (oWorkers->size() < 1)
    {
        //         outProtocolPrompt()<< "_ybSendTable is Null.";
        return  SendMeasureFail_Nomeasure;
    }

    int nSum = 0;
    memset(_sendBuffer,0,GCL_LEN_DATA_EM);

    int nMaxLength = GCL_LEN_DATA_EM>=CI_PSM_PACKET_DATA?(CI_PSM_PACKET_DATA-1):GCL_LEN_DATA_EM;
    int maxNum = nMaxLength / sizeof(gcd_address_vqt_string128_t);

    gcd_address_vqt_string128_t *oValue = (gcd_address_vqt_string128_t*)&_sendBuffer[0];
    int nCount = 0;
    std::string szVal;
    int nLength = 0;
    for (size_t i = 0; i < oWorkers->size(); ++i)
    {
        const StrawAddressWorker* oWorker = oWorkers->at(i);

        oValue->address = oWorker->address();
        const StrawMeasure *oMeasure = oWorker->measure();
        if(oMeasure!=NULL)
        {
            oValue->datetime =  oMeasure->lastRefreshTime();
            //字符串型
            szVal = CxString::toString(oMeasure->value());
            nLength = (szVal.length()<=128)?szVal.length():128;
            memcpy(oValue->value,szVal.c_str(),nLength);
            oValue->quality  =  oMeasure->quality();
            oValue++;
            nCount++;
        }
        else
        {
            outProtocolPrompt()<< CxTime::currentSepochString() << "measure is null"<<oWorker->address()<<cxEndLine;
        }
        //发送缓冲区满
        if(nCount>=maxNum)
        {
            nSum += postRealtimeDataPost(gcd_address_vqt_string128_t::getType(),_sendBuffer,maxNum*sizeof(gcd_address_vqt_string128_t),maxNum);
            nCount = 0;
            memset(_sendBuffer,0,GCL_LEN_DATA_EM);
            oValue = (gcd_address_vqt_string128_t*)&_sendBuffer[0];
        }
    }
    if(nCount>0)
    {
        nSum += postRealtimeDataPost(gcd_address_vqt_string128_t::getType(),_sendBuffer,nCount*sizeof(gcd_address_vqt_string128_t),nCount);
    }

    outProtocolDebug()<<CxTime::currentSepochString()<<protocolId()<<"Send YX" << nSum  <<cxEndLine;

    return nSum;
}
int PsmProtocol::sendAllYC()
{
    if(_ycSendTable==NULL) return SendMeasureFail_Nomeasure;

    const std::vector<YcaddAddressWorker*>* oWorkers = _ycSendTable->workers();
    if (oWorkers->size() < 1)
    {
        //        outProtocolPrompt()<< "_ycSendTable is Null.";
        return  SendMeasureFail_Nomeasure;
    }

    int nSum = 0;
    memset(_sendBuffer,0,GCL_LEN_DATA_EM);

    int nMaxLength = GCL_LEN_DATA_EM>=CI_PSM_PACKET_DATA?(CI_PSM_PACKET_DATA-1):GCL_LEN_DATA_EM;

    int maxNum = nMaxLength / sizeof(gcd_address_vqt_realText15_t);

    gcd_address_vqt_realText15_t *oValue = (gcd_address_vqt_realText15_t*)&_sendBuffer[0];
    int nCount = 0;
    std::string szVal;
    int nLength = 0;
    for (size_t i = 0; i < oWorkers->size(); ++i)
    {
        const YcaddAddressWorker* oWorker = oWorkers->at(i);

        oValue->address = oWorker->address();
        const YcaddMeasure *oMeasure = oWorker->measure();
        if(oMeasure!=NULL)
        {
            oValue->datetime =  oMeasure->lastRefreshTime();
            //字符串型
            szVal = CxString::toString(oMeasure->value());
            nLength = (szVal.length()<15)?szVal.length():15;
            memcpy(oValue->value,szVal.c_str(),nLength);
            oValue->quality  =  oMeasure->quality();
            oValue++;
            nCount++;
        }
        //发送缓冲区满
        if(nCount>=maxNum)
        {
            nSum += postRealtimeDataPost(gcd_address_vqt_realText15_t::getType(),_sendBuffer,maxNum*sizeof(gcd_address_vqt_realText15_t),maxNum);
            nCount = 0;
            memset(_sendBuffer,0,GCL_LEN_DATA_EM);
            oValue = (gcd_address_vqt_realText15_t*)&_sendBuffer[0];
        }
    }
    if(nCount>0)
    {
        nSum += postRealtimeDataPost(gcd_address_vqt_realText15_t::getType(),_sendBuffer,nCount*sizeof(gcd_address_vqt_realText15_t),nCount);
    }
    outProtocolDebug()<<CxTime::currentSepochString()<<protocolId()<<"Send YC" << nSum  <<cxEndLine;
    return nSum;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//发送遥信
int PsmProtocol::sendYX(uint32 id)
{
    if(_yxSendTable==NULL) return SendMeasureFail_Nomeasure;

    int addr = _yxSendTable->addressByMeasID(id);
    if(addr<0) return SendMeasureFail_Nomeasure;

    MonsbMeasure * oMeasure =   _yxSendTable->MeasureSonByMeasID(id);
    if(oMeasure == NULL) return SendMeasureFail_Nomeasure;
    gcd_address_vqt_intText1_t oValue;

    memset(&oValue,0,sizeof(gcd_address_vqt_intText1_t));
    oValue.address = addr;
    oValue.datetime = oMeasure->lastRefreshTime();
    //字符串型
    std::string szVal = CxString::toString(oMeasure->value());
    int nLength = (szVal.length()<=7)?szVal.length():7;
    memcpy(oValue.value,szVal.c_str(),nLength);
    oValue.quality  =  oMeasure->quality();
    return postRealtimeDataPost(gcd_address_vqt_intText1_t::getType(),(char*)&oValue,sizeof(gcd_address_vqt_intText1_t),1);

    //    outProtocolDebug()<< CxTime::currentSepochString() << "YX Update:" << oMeasure->measureId()<<oValue.address<<oValue.value<<oValue.quality<<cxEndLine;

}
//发送字符串
int PsmProtocol::sendSTR(GCCommon::GCDataElement &dt)
{
    if(_ywSendTable==NULL) return SendMeasureFail_Nomeasure;

    int addr = _ywSendTable->addressByMeasID(dt.pram());
    if(addr<0)
    {
        outProtocolDebug()<< "addr is not exist" <<dt.pram()<<cxEndLine;
        return SendMeasureFail_Nomeasure;
    }

    StrawMeasure * oMeasure =   _ywSendTable->MeasureSonByMeasID(dt.pram());
    if(oMeasure == NULL) return SendMeasureFail_Nomeasure;

    //设定值
    std::string sValue = std::string(dt.buffer(),dt.dataLenth());

    oMeasure->setValue(sValue);

    gcd_address_vqt_string128_t oValue;

    memset(&oValue,0,sizeof(gcd_address_vqt_string128_t));
    oValue.address = addr;
    oValue.datetime = oMeasure->lastRefreshTime();
    //字符串型
    std::string szVal = oMeasure->value();
    int nLength = (szVal.length()<=127)?szVal.length():127;
    memcpy(oValue.value,szVal.c_str(),nLength);
    oValue.quality  =  oMeasure->quality();
    return postRealtimeDataPost(gcd_address_vqt_string128_t::getType(),(char*)&oValue,sizeof(gcd_address_vqt_string128_t),1);

//    outProtocolDebug()<< "STR Update:" << oMeasure->measureId()<<oValue.address<<oValue.value<<oValue.quality<<cxEndLine;
}

int PsmProtocol::sendYC(uint32 id)
{
    if(_ycSendTable==NULL) return SendMeasureFail_Nomeasure;

    int addr = _ycSendTable->addressByMeasID(id);
    if(addr<0) return SendMeasureFail_Nomeasure;

    YcaddMeasure * oMeasure =   _ycSendTable->MeasureSonByMeasID(id);
    if(oMeasure == NULL) return SendMeasureFail_Nomeasure;

    gcd_address_vqt_realText15_t oValue;
    memset(&oValue,0,sizeof(gcd_address_vqt_realText15_t));

    oValue.address = addr;
    oValue.datetime = oMeasure->lastRefreshTime();
    //字符串型
    std::string szVal = CxString::format("%lf",oMeasure->value());
    int nLength = (szVal.length()<=15)?szVal.length():15;
    memcpy(oValue.value,szVal.c_str(),nLength);
    oValue.quality  =  oMeasure->quality();

    return postRealtimeDataPost(gcd_address_vqt_realText15_t::getType(),(char*)&oValue,sizeof(gcd_address_vqt_realText15_t),1);
//    outProtocolDebug()<< CxTime::currentSepochString() << "YC Update:" << oMeasure->measureId()<<oValue.address<<oValue.value<<oValue.quality<<cxEndLine;
}


