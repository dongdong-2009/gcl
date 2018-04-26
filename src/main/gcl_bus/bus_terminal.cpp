#include "bus_terminal.h"


#include "bus_data_deal.h"
#include "bus_measure_publish.h"
#include "bus_measure_control.h"
#include "bus_general_access.h"

#include <common/gcl_resource.h>


using namespace std;

static const std::string CS_SectionBusManager = "BusConfigs";
static const std::string CS_EntryBusUdpPort = "UdpServerPort";
static const std::string CS_EntryBusTcpPort = "TcpServerPort";


static map<int, int> f_sourceIdPorts;


static string f_fileSavePath;


//25362689
//25362945
//25363201


BusManager * fn_oBusManager()
{
    CxApplication::registStartFunction(BusManager::init);
    CxApplication::registStopFunction(BusManager::uninit);

    CxInterinfoIn::addObserver(BusManager::interinfoIn_line);
    CxInterinfoIn::addObserver(BusManager::interinfoIn_cmd);

    static BusManager m;
    return &m;
}

static BusManager * f_oBusManager = fn_oBusManager();

static map<int, int> f_targetSourceIds;
static map<CxChannelRoad *, BusTerminal*> f_oRoadTerminals;
static map<int, BusTerminal*> f_sourceTerminals;



//* Thread Receive File
class BusTransferFileThread : public CxJoinableThread
{
private:
    volatile int _status;
    volatile int _count;
    CxMutex _lock;
    std::vector<CxFileSystem::PathInfo > _bufferFileInfos1;
    std::vector<CxFileSystem::PathInfo > _bufferFileInfos2;
    std::vector<std::vector<std::string> > _bufferFileDatas1;
    std::vector<std::vector<std::string> > _bufferFileDatas2;
    std::vector<PsmAttach> _bufferFileAttachs1;
    std::vector<PsmAttach> _bufferFileAttachs2;
    std::vector<CxFileSystem::PathInfo > * volatile _pushBufferFileInfos;
    std::vector<CxFileSystem::PathInfo > * volatile _popBufferFileInfos;
    std::vector<std::vector<std::string> > * volatile _pushBufferFileDatas;
    std::vector<std::vector<std::string> > * volatile _popBufferFileDatas;
    std::vector<PsmAttach> * volatile _pushBufferFileAttachs;
    std::vector<PsmAttach> * volatile _popBufferFileAttachs;

public:
    void init()
    {
        _status = 1;
        _count = 0;
        _pushBufferFileInfos = & _bufferFileInfos1;
        _popBufferFileInfos = & _bufferFileInfos2;
        _pushBufferFileDatas = & _bufferFileDatas1;
        _popBufferFileDatas = & _bufferFileDatas2;
        _pushBufferFileAttachs = & _bufferFileAttachs1;
        _popBufferFileAttachs = & _bufferFileAttachs2;
    }

    inline void stop() { _status = 0; join(); }

    int pushFile(const CxFileSystem::PathInfo & pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
    {
        CxMutexScope lock(_lock);
        _pushBufferFileInfos->push_back(pathInfo);
        _pushBufferFileDatas->push_back(fileData);
        if (oAttach)
        {
            _pushBufferFileAttachs->push_back(* oAttach);
        }
        else
        {
            _pushBufferFileAttachs->push_back(PsmAttach());
        }
        _count++;
        return TRUE;
    }

 protected:
    void run() {
        _status = 1;
        while ( _status )
        {
            if (_count>0)
            {
                CxMutexScope lock(_lock);
                std::vector<CxFileSystem::PathInfo > * tmpInfos = _pushBufferFileInfos;
                _pushBufferFileInfos = _popBufferFileInfos;
                _popBufferFileInfos = tmpInfos;
                std::vector<std::vector<std::string> > * tmpDatas = _pushBufferFileDatas;
                _pushBufferFileDatas = _popBufferFileDatas;
                _popBufferFileDatas = tmpDatas;
                std::vector<PsmAttach> * tmpAttachs = _pushBufferFileAttachs;
                _pushBufferFileAttachs = _popBufferFileAttachs;
                _popBufferFileAttachs = tmpAttachs;
                _count=0;
            }

            for (size_t i = 0; i < _popBufferFileInfos->size(); ++i)
            {
                CxFileSystem::PathInfo & fileInfo = _popBufferFileInfos->at(i);
                std::vector<std::string> & fileData = _popBufferFileDatas->at(i);
                PsmAttach & fileAttach = _popBufferFileAttachs->at(i);

                std::string sNow = CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0);


                std::string fileName = fileInfo.fileName;
                //判断文件名是否含有yyyymmdd
                if(fileInfo.fileName.find(sNow.substr(0,8))== string::npos)
                {
                    fileInfo.fileName = CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0)+"_"+ fileInfo.fileName;
                }

                std::string sToday = CxTime::toStringDay(CxTime::currentSepoch(),'-');
                std::string sFullPath = CxFileSystem::mergeFilePath(f_fileSavePath,sToday);
                CxFileSystem::createDirMultiLevel(sFullPath);

                std::string sFilePath = CxFileSystem::mergeFilePath( sFullPath , fileInfo.fileName);
                bool bSave = CxFile::save(sFilePath, fileData, std::string());
                int iSaveStatus = bSave ? TRUE : -1;
                if (bSave) fileInfo.path = sFullPath;

                FileInfoShareMemory::memoryManager()->pushFileInfo(fileInfo, "", fileAttach.sourceId, fileAttach.targetId, iSaveStatus);
            }
            _popBufferFileInfos->clear();
            _popBufferFileDatas->clear();
            _popBufferFileAttachs->clear();

            CxThread::sleep(30);
        }

    }

    void exit() {
        _status = 0;
        CxJoinableThread::exit();
    }

};

BusTransferFileThread * volatile f_oBusTransferFileThread = NULL;











//* BusTerminal
BusTerminal::BusTerminal(CxChannelRoad *value) :
    _psmExplainer(),
    _sendPacket(c_psm_head, 1, CxCrc::calcCRC16),
    _sendCacheSize(1024 * 1024),
    _channelRoad(value),
    _lastSendDataTime(0),
    _lastReceivedDataTime(0),
    _sourceTerminalId(0),
    _originalTerminalId(0)
{
    _psmExplainer.setSentSourceId(0);
    _psmExplainer.registerPsmExplainNotify(this);
    _psmExplainer.registerPsmExplainWrite(this);
    _psmExplainer.start();

    _remoteIsLocal = _channelRoad ? _channelRoad->remoteIpAddress().isLocalIpAddress() : false;
}

//BusTerminal::BusTerminal(int iTerminalId) :
//    _psmExplainer(),
//    _sendPacket(c_psm_head, 1, CxCrc::calcCRC16),
//    _sendCacheSize(1024 * 1024),
//    _channelRoad(NULL),
//    _lastSendDataTime(0),
//    _lastReceivedDataTime(0),
//    _sourceTerminalId(iTerminalId),
//    _originalTerminalId(iTerminalId)
//{
//    _psmExplainer.setSentSourceId(0);
//    _psmExplainer.registerPsmExplainNotify(this);
//    _psmExplainer.registerPsmExplainWrite(this);
//    _psmExplainer.start();

//    _remoteIsLocal = _channelRoad ? _channelRoad->remoteIpAddress().isLocalIpAddress() : false;
//}

BusTerminal::~BusTerminal()
{
    _psmExplainer.registerPsmExplainNotify(NULL);
    _psmExplainer.registerPsmExplainWrite(NULL);
    _psmExplainer.stop();
}

void BusTerminal::setTerminalId(int iSourceId, int iOriginalId)
{
    _sourceTerminalId = iSourceId;
    _originalTerminalId = iOriginalId;
    if (_channelRoad)
    {
        _channelRoad->setSourceId(iSourceId);
        int iTargetPort = 0;
        iTargetPort = CxContainer::value(f_sourceIdPorts, _sourceTerminalId, iTargetPort);
        if (CxString::isValidPort(iTargetPort))
        {
            _channelRoad->updateRemote(iTargetPort);
        }
    }
}

void BusTerminal::receivedData(const char *pData, int iLength)
{
    if (_psmExplainer.receivedData((const uchar *)pData, iLength) > 0)
    {
        _lastReceivedDataTime = CxTime::currentSystemTime();
    }
}

int BusTerminal::pushPacketData(const psm_packet_body_t & body, const char * pData, int iDataLength)
{
    //以下是非缓存方式
    if (_channelRoad && _sendPacket.setPack(body, pData, iDataLength) > 0)
    {
        return _channelRoad->localChannel()->sendData(_sendPacket.data(), _sendPacket.size(), _channelRoad);
    }
    return -1;
//以下是用缓存方式
//    if (_sendPacket.totalSize() > _sendCacheSize)
//    {
//        _sendPacket.clear();
//    }
//    _sendPacket.push(body, pData, iDataLength);
//    return sendNextPacket();
}

void BusTerminal::appendSendPacket(BusTerminal *oPsm)
{
//    PsmSendPacket & other = oPsm->_sendPacket;
//    while (other.count()>0)
//    {
//        _sendPacket.push(other.pop());
//    }
}

int BusTerminal::send_data(const char *pData, int iLength, int iTargetId)
{
    if (iTargetId>0)
    {
        _lastSendDataTime = CxTime::currentSystemTime();
        CxChannelRoad * oChannelRoad = CxChannelRoadManager::find(iTargetId);
        if (! oChannelRoad)
        {
            int iTargetId2 = CxContainer::value(f_targetSourceIds, iTargetId, ci_int_minus_one);
            if (iTargetId2 != ci_int_minus_one)
            {
                oChannelRoad = CxChannelRoadManager::find(iTargetId2);
            }
        }
        if (oChannelRoad)
        {
            return oChannelRoad->sendData(pData, iLength);
        }
        else
        {
//            cxDebug() << "BusTerminal::send_data can not find iTargetId=" << iTargetId;
            return -1;
        }
    }
//    cxDebug() << "BusTerminal::send_data targetId error : iTargetId=" << iTargetId;
    return -1;
}

string BusTerminal::reportSelf()
{
    std::string sResult = CxString::format("PsmExplainer: {%s}, \n", _psmExplainer.reportSelf().c_str());
    if (_channelRoad)
        sResult += CxString::format("ChannelRoad: {%s}, \n", _channelRoad->reportSelf().c_str());
    else
        sResult += CxString::format("ChannelRoad: {%s}, \n", "nil");

    string sLastSendTime = CxTime::toString(_lastSendDataTime);
    string sLastReceivedTime = CxTime::toString(_lastReceivedDataTime);
    sResult += CxString::format("sourceTerminalId: 0x%02x, originalTerminalId: 0x%02x, lastReceivedDataTime: %s, _lastSendDataTime: %s, remoteIsLocal: %d, ",
                                           _sourceTerminalId, _originalTerminalId, sLastSendTime.c_str(), sLastReceivedTime.c_str(), _remoteIsLocal);

    return sResult;
}

int BusTerminal::received_packet(const PsmExplainer *oPsm, int oTag)
{
    assert(oPsm!=NULL);
    assert(oPsm==&_psmExplainer);
    const PsmReceivePacket & receivePacket = _psmExplainer.receivePacket();
    const std::vector<PsmPacketData> & packets = receivePacket.packets();
    for (size_t i = 0; i < packets.size(); ++i)
    {
        const PsmPacketData & packet = packets.at(i);
        const psm_packet_body_t & body = packet.body;

        //刚解释出第一帧时
        //找到要迁移的ChannelRoad
        int iOriginalId = body.sourceOriginal;
        int iSourceId = body.sourceAddress;
        int iTargetId = body.targetAddress;
        if (iSourceId <= 0 || (iSourceId==iTargetId))
        {
            cxDebug() << _channelRoad->remoteIpAddress().ip() << " : " << _channelRoad->remoteIpAddress().port() << " f_transferTerminal - fail SourceId=" << iSourceId << ", TargetId=" << iTargetId;
            continue;
        }
        if (_sourceTerminalId != iSourceId)
        {
            //*001
            CxChannelRoad * oChannelRoadDelete = NULL;
            for (map<CxChannelRoad *, BusTerminal*>::iterator it = f_oRoadTerminals.begin(); it != f_oRoadTerminals.end(); ++it)
            {
                CxChannelRoad * oChannelRoad = it->first;
                BusTerminal * oBusTerminal = it->second;
                //找到要迁移的BusSub
                if (oChannelRoad->sourceId() == iSourceId && oBusTerminal != this)
                {
                    //对方还在线时，新的不理会
                    if (oChannelRoad->isInline())
                    {
                        cxDebug() << _channelRoad->remoteIpAddress().ip() << " : " << _channelRoad->remoteIpAddress().port() << " f_transferTerminal - fail : has other terminalId inline!!!";
                        return false;
                    }
                    appendSendPacket(oBusTerminal);
                    //删除Sub 与 Terminal
                    oChannelRoadDelete = oChannelRoad;
                    CxChannelRoadManager::erase(oChannelRoad);
//                    f_oRoadTerminals.erase(it);
                    delete oBusTerminal;
                    cxDebug() << _channelRoad->remoteIpAddress().ip() << " : " << _channelRoad->remoteIpAddress().port() << " f_transferTerminal - success : appendSendPacket";
                    break;
                }
            }
            if (oChannelRoadDelete)
            {
                f_oRoadTerminals.erase(oChannelRoadDelete);
            }

            //*002
            BusTerminal * oBusTerminal = BusManager::findSourceTerminal(iSourceId);
            if (oBusTerminal)
            {
                appendSendPacket(oBusTerminal);
                //删除Sub 与 Terminal
                CxContainer::remove(f_sourceTerminals, iSourceId);
                delete oBusTerminal;
                cxDebug() << _channelRoad->remoteIpAddress().ip() << " : " << _channelRoad->remoteIpAddress().port() << " f_transferTerminal - success : appendSendPacket";
            }

            setTerminalId(iSourceId, iOriginalId);
        }

        //*003
        //找目标，过数
        if (iTargetId > 0 && ((body.command != gct_file_information_write) && (body.command != gct_file_content_write)))
        {
            BusManager::transferPacketDataPost(body, packet.data, packet.length, iTargetId);
//            if (body.command == gct_file_information_write || body.command == gct_file_content_write)
//            {
//                cxDebug().space() << "BusTerminal -> pushPacketData from:" << _channelRoad->sourceIpAddress().ip() << _channelRoad->sourceIpAddress().port() << "to:" << oBusTerminal->_channelRoad->sourceIpAddress().ip() << oBusTerminal->_channelRoad->sourceIpAddress().port() << "; command=" << body.controlWord <<"; command=" << body.command << "; length=" << packet.length;
//            }
        }
        else
        {
//            if (body.command != gct_channel_hand)
//            {
//                cxDebug().space() << _channelRoad->remoteIpAddress().ip() << _channelRoad->remoteIpAddress().port() << body.command << packet.length;
//            }
            _psmExplainer.dealPacketData(packet);
        }
    }
    return TRUE;
}

void BusTerminal::received_message_command(const PsmExplainer *oPsm, const char *sCommand, const char *sParam, int iParamLength, const PsmAttach *oAttach)
{
    BusDataDeal::dealMessageCommand(_sourceTerminalId, sCommand, sParam, iParamLength, oAttach);
}

void BusTerminal::received_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    BusDataDeal::dealFile(_sourceTerminalId, pathInfo, fileData, oAttach);
}

void BusTerminal::received_realtime_data_request(const PsmExplainer *oPsm, const PsmAttach *oAttach)
{
    BusDataDeal::dealRealtimeDataRequest(_sourceTerminalId, oAttach);
}

void BusTerminal::received_realtime_data_post(const PsmExplainer *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    BusDataDeal::dealRealtimeDataPost(_sourceTerminalId, iParamType, pData, iDataLength, iParamCount, oAttach);
}

void BusTerminal::result_file_write(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const PsmAttach *oAttach)
{

}

int BusTerminal::sendNextPacket()
{
//    if (_channelRoad)
//    {
//        string sSendData = _sendPacket.pop();
//        return _channelRoad->localChannel()->sendData(sSendData.data(), sSendData.size(), _channelRoad);
//    }
    return -1;
}



























void BusManager::init()
{
    f_oBusManager->start();
}

void BusManager::uninit()
{
    f_oBusManager->stop();
}

BusTerminal * BusManager::findRoadTerminal(CxChannelRoad * oChannelRoad)
{
    map<CxChannelRoad *, BusTerminal*>::iterator it = f_oRoadTerminals.find(oChannelRoad);
    if (it != f_oRoadTerminals.end())
    {
        return it->second;
    }
    return NULL;
}

BusTerminal * BusManager::findRoadTerminal(int iSourceId)
{
    for (map<CxChannelRoad *, BusTerminal*>::iterator it = f_oRoadTerminals.begin(); it != f_oRoadTerminals.end(); ++it)
    {
        BusTerminal * oTerminal = it->second;
        if (oTerminal->terminalId() == iSourceId)
        {
            return oTerminal;
        }
    }
    return NULL;
}

BusTerminal * BusManager::allocateRoadTerminal(CxChannelRoad * oChannelRoad)
{
    BusTerminal * oTerminal = findRoadTerminal(oChannelRoad);
    if (!oTerminal)
    {
        oTerminal = new BusTerminal(oChannelRoad);
        f_oRoadTerminals[oChannelRoad] = oTerminal;
    }
    return oTerminal;
}

void BusManager::deleteRoadTerminal(CxChannelRoad *oChannelRoad)
{
    map<CxChannelRoad *, BusTerminal*>::iterator it = f_oRoadTerminals.find(oChannelRoad);
    if (it != f_oRoadTerminals.end())
    {
        BusTerminal * oBusTerminal = it->second;
        delete oBusTerminal;
        f_oRoadTerminals.erase(it);
    }
}

BusTerminal * BusManager::findSourceTerminal(int iSourceId)
{
    map<int, BusTerminal*>::iterator it = f_sourceTerminals.find(iSourceId);
    if (it != f_sourceTerminals.end())
    {
        return it->second;
    }
    return NULL;
}

BusTerminal * BusManager::findTargetTerminal(int iTargetTermainlId, int iT2t)
{
    //先找 iTargetTermainlId 是否上线
    BusTerminal * oTerminal = findRoadTerminal(iTargetTermainlId);
    if (oTerminal)
    {
        return oTerminal;
    }
    else
    {
        //如果与 iTargetTermainlId 一样就不找到一遍
        if (iT2t > 0 && iTargetTermainlId != iT2t)
        {
            oTerminal = findRoadTerminal(iT2t);
        }
        if (oTerminal)
        {
            return oTerminal;
        }
        //如果 oTerminal 没找到，再找 SourceTerminal 的
        return findSourceTerminal(iTargetTermainlId);
    }
}

void BusManager::deleteSourceTerminal(int iSourceId)
{
    map<int, BusTerminal*>::iterator it = f_sourceTerminals.find(iSourceId);
    if (it != f_sourceTerminals.end())
    {
        BusTerminal * oBusTerminal = it->second;
        delete oBusTerminal;
        f_sourceTerminals.erase(it);
    }
}

int BusManager::transferMessage(int iSourceTerminalId, const char *pCommand, const char *pParam, int iParamLength, int iTargetTerminalId)
{
    PsmAttach attach(0, 0, iSourceTerminalId, iTargetTerminalId, 0);
    if (iTargetTerminalId==BusGeneralAccess::getSourceId())
    {
        if (iSourceTerminalId<1) attach.sourceId = BusGeneralAccess::getSourceId();
        attach.targetId = 0;
        return BusGeneralAccess::postMessage(pCommand, pParam, iParamLength, &attach);
    }
    else
    {
        int iT2t = CxContainer::value(f_targetSourceIds, iTargetTerminalId, ci_int_minus_one);
        if (iT2t == BusGeneralAccess::getSourceId())
        {
            if (iSourceTerminalId<1) attach.sourceId = BusGeneralAccess::getSourceId();
            return BusGeneralAccess::postMessage(pCommand, pParam, iParamLength, &attach);
        }
        else
        {
            BusTerminal * oTerminal = findTargetTerminal(iTargetTerminalId, iT2t);
            if (oTerminal)
                return oTerminal->psmExplainter()->postMessageCommand(pCommand, pParam, iParamLength, &attach);
            else
                return -1;
        }
    }
}

int BusManager::transferFileStream(int iSourceTerminalId, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, int iTargetTerminalId)
{
    PsmAttach attach(0, 0, iSourceTerminalId, iTargetTerminalId, 0);
    if (iTargetTerminalId==BusGeneralAccess::getSourceId())
    {
        if (iSourceTerminalId<1) attach.sourceId = BusGeneralAccess::getSourceId();
        attach.targetId = 0;
        return BusGeneralAccess::postFile(pathInfo, fileData, &attach);
    }
    else
    {
        int iT2t = CxContainer::value(f_targetSourceIds, iTargetTerminalId, ci_int_minus_one);
        if (iT2t == BusGeneralAccess::getSourceId())
        {
            if (iSourceTerminalId<1) attach.sourceId = BusGeneralAccess::getSourceId();
            return BusGeneralAccess::postFile(pathInfo, fileData, &attach);
        }
        else
        {
            BusTerminal * oTerminal = findTargetTerminal(iTargetTerminalId, iT2t);
            if (oTerminal)
            {
                if (oTerminal->remoteIsLocal() && f_oBusTransferFileThread)
                    return f_oBusTransferFileThread->pushFile(pathInfo, fileData, &attach);
                else
                    return oTerminal->psmExplainter()->postFile(pathInfo, fileData, &attach);
            }
            else
            {
                return -1;
            }
        }
    }
}

int BusManager::transferRealtimeDataPost(int iSourceTerminalId, int iParamType, const char *pData, int iDataLength, int iParamCount, int iTargetTerminalId)
{
    PsmAttach attach(0, 0, iSourceTerminalId, iTargetTerminalId, 0);
    if (iTargetTerminalId==BusGeneralAccess::getSourceId())
    {
        if (iSourceTerminalId<1) attach.sourceId = BusGeneralAccess::getSourceId();
        attach.targetId = 0;
        return BusGeneralAccess::postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, &attach);
    }
    else
    {
        int iT2t = CxContainer::value(f_targetSourceIds, iTargetTerminalId, ci_int_minus_one);
        if (iT2t == BusGeneralAccess::getSourceId())
        {
            if (iSourceTerminalId<1) attach.sourceId = BusGeneralAccess::getSourceId();
            return BusGeneralAccess::postRealtimeDataPost(iParamType, pData, iDataLength, iParamCount, &attach);
        }
        else
        {
            BusTerminal * oTerminal = findTargetTerminal(iTargetTerminalId, iT2t);
            if (oTerminal)
                return oTerminal->psmExplainter()->postRealtimeDataPost(iParamType, pData,iDataLength,iParamCount,&attach);
            else
                return -1;
        }
    }
}

int BusManager::transferPacketDataPost(const psm_packet_body_t &body, const char *pData, int iDataLength, int iTargetTerminalId)
{
    if (iTargetTerminalId==BusGeneralAccess::getSourceId())
    {
        return BusGeneralAccess::postPacketData(body, pData, iDataLength);
    }
    else
    {
        int iT2t = CxContainer::value(f_targetSourceIds, iTargetTerminalId, ci_int_minus_one);
        if (iT2t == BusGeneralAccess::getSourceId())
        {
            return BusGeneralAccess::postPacketData(body, pData, iDataLength);
        }
        else
        {
            BusTerminal * oTerminal = BusManager::findTargetTerminal(iTargetTerminalId, iT2t);
            if (oTerminal)
                return oTerminal->pushPacketData(body, pData, iDataLength);
            else
                return -1;
        }
    }
}


/***
 bus -update ssid=1;name=xxx;ttid=2

*/
int BusManager::interinfoIn_line(const string &sInfo, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    return 0;
}

int BusManager::interinfoIn_cmd(const string &sCommand, const std::map<string, string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag)
{
    if (sCommand == "bus")
    {
        if (CxContainer::contain(sParams, std::string("update")))
        {
            std::string sUpdateValue = CxContainer::value(sParams, std::string("update"));
            string sFilePath = CxApplication::configExtendFilePath();
            if (sUpdateValue.size() > 0)
            {
                if (CxFileSystem::isExist(sUpdateValue))
                {
                    sFilePath = sUpdateValue;
                }
                else
                {
                    BusManager::update(sParams);
                    MessageTransferStrategy::update(sParams);
                    FileNameTransferStrategy::update(sParams);
                    RealdataTransferStrategy::update(sParams);
                    BusMeasurePublisher::update(sParams);
                    BusMeasureControl::update(sParams);

                    cxPrompt() << "bus update success : update by contents - " << sParams;

                    return CxInterinfo::InDeal_Ed;
                }
            }
            if (CxFileSystem::isExist(sFilePath))
            {
                vector<map<string, string> > rows;
                CxTinyXml::loadTable4Level(sFilePath, rows, "", CS_TableName_Bus);
                UpdateTemplate<BusManager>::update(rows);
                UpdateTemplate<MessageTransferStrategy>::update(rows);
                UpdateTemplate<FileNameTransferStrategy>::update(rows);
                UpdateTemplate<RealdataTransferStrategy>::update(rows);
                UpdateTemplate<BusMeasurePublisher>::update(rows);
                UpdateTemplate<BusMeasureControl>::update(rows);

                cxPrompt() << "bus update success : update by filePath - " << sFilePath;

                return CxInterinfo::InDeal_Ed;
            }
            else
            {
                cxPrompt() << "bus update fail : update by filePath - " << sFilePath;
            }
        }
        if (CxContainer::contain(sParams, std::string("report")))
        {
            reportAllToFile();
            return CxInterinfo::InDeal_Ed;
        }
        if (CxContainer::contain(sParams, std::string("exit")) && CxContainer::contain(sParams, std::string("test135246")))
        {
            CxApplication::raiseExit();
            return CxInterinfo::InDeal_Ed;
        }
        return CxInterinfo::InDeal_Ed;
    }
    return 0;
}

void BusManager::update(const std::map<string, string> &row)
{
    if (CxContainer::contain(row, CS_Fetch_Port))
    {
        int sourceTerminalId = 0;
        int iPort = 0;

        sourceTerminalId = CxContainer::valueTo(row, CS_SourceTerminalId, sourceTerminalId);
        iPort = CxContainer::valueTo(row, CS_Fetch_Port, iPort);

        update( sourceTerminalId, iPort );
    }
    else if (CxContainer::contain(row, CS_TargetTerminalId1))
    {
        int ttid1 = 0;
        int ttid2 = 0;

        ttid1 = CxContainer::valueTo(row, CS_TargetTerminalId1, ttid1);
        ttid2 = CxContainer::valueTo(row, CS_TargetTerminalId2, ttid2);

        updateT2t( ttid1, ttid2 );
    }
}

void BusManager::update(int ssid,int iPort)
{
    GM_INVALID_RETURE(ssid>0);
    GM_INVALID_RETURE(iPort>0);
    f_sourceIdPorts[ssid] = iPort;
    CxChannelRoadManager::updateRemote(ssid, iPort);
}

void BusManager::updateT2t(int ttid1, int ttid2)
{
    GM_INVALID_RETURE(ttid1>0);
    GM_INVALID_RETURE(ttid2>0);
    f_targetSourceIds[ttid1] = ttid2;
}

std::vector<string> BusManager::reportSelf()
{
    vector<string> sReports;
    sReports.push_back("*** Bus Manager Report ***");
    string sSourceidPorts = CxContainer::joinToString(f_sourceIdPorts, "=", "; ");
    sReports.push_back(CxString::format("TerminalId-Port Count[%d] : %s", f_sourceIdPorts.size(), sSourceidPorts.c_str()));
    sReports.push_back(CxString::format("BusTerminal Count[%d] :", f_oRoadTerminals.size()));
    int iIndex = 0;
    for (map<CxChannelRoad *, BusTerminal*>::iterator it = f_oRoadTerminals.begin(); it != f_oRoadTerminals.end(); ++it)
    {
        BusTerminal * oTerminal = it->second;
        sReports.push_back(CxString::format("BusTerminal %03d : ",iIndex) + oTerminal->reportSelf());
        iIndex++;
    }
    return sReports;
}

void BusManager::reportAllToFile()
{
    string sReportFilePath = CxFileSystem::mergeFilePath( CxApplication::applicationDeployPath(), "report" );
    sReportFilePath = CxFileSystem::mergeFilePath( sReportFilePath, CxApplication::applicationTargetName() + ".report.txt" );
    vector<string> sReports;

    CxContainer::append(sReports, f_oBusManager->_udpServer->reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, f_oBusManager->_tcpServer->reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, CxChannelRoadManager::reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, BusManager::reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, MessageTransferStrategy::reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, FileNameTransferStrategy::reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, RealdataTransferStrategy::reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, BusMeasurePublisher::reportSelf());
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");
    CxContainer::append(sReports, BusMeasureControl::reportSelf());

    if (CxFile::save(sReportFilePath, sReports)>0)
    {
        cxPrompt() << "bus report success , and save to file " << sReportFilePath;
    }
    else
    {
        cxPrompt() << "bus report fail!!!" << sReportFilePath;
    }
}
















BusManager::BusManager()
{
    _udpServer = NULL;
    _tcpServer = NULL;
}

BusManager::~BusManager()
{
    stop();
}

void BusManager::start()
{
    cxDebug() << "bus load config ...";

    string sConfigFilePath = CxApplication::configExtendFilePath();
    if (CxFileSystem::isExist(sConfigFilePath))
    {
        vector<map<string, string> > rows;
        CxTinyXml::loadTable4Level(sConfigFilePath, rows, "", CS_TableName_Bus, "");
        UpdateTemplate<BusManager>::update(rows);
        UpdateTemplate<MessageTransferStrategy>::update(rows);
        UpdateTemplate<FileNameTransferStrategy>::update(rows);
        UpdateTemplate<RealdataTransferStrategy>::update(rows);
        UpdateTemplate<BusMeasurePublisher>::update(rows);
        UpdateTemplate<BusMeasureControl>::update(rows);

        cxDebug() << "bus load config finish.";
    }
    else
    {
        cxDebug() << "file : " << sConfigFilePath << " don't exist!!";
    }

    f_fileSavePath = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), "temp");

    if (! f_oBusTransferFileThread)
    {
        f_oBusTransferFileThread = new BusTransferFileThread();
        f_oBusTransferFileThread->init();
        f_oBusTransferFileThread->start();
    }

    if (! _udpServer)
    {
        _udpServer = new CxChannelUdp();
        _udpServer->setIsServer(true);
        _udpServer->addObserver(f_oBusManager);
        _udpServer->setAutoOpenInterval(3000);
        _udpServer->setIsMultiRoad(true);
        _udpServer->setIsSendQueue(true);
    }
    if (! _tcpServer)
    {
        _tcpServer = new CxChannelTcpserver();
        _tcpServer->addObserver(f_oBusManager);
        _tcpServer->setAutoOpenInterval(3000);
        _tcpServer->setIsMultiRoad(true);
        _tcpServer->setIsSendQueue(true);
    }

    int iUdpPort = 6621;
    int iTcpPort = 6631;
    iUdpPort = CxApplication::findConfig(CS_SectionBusManager, CS_EntryBusUdpPort, iUdpPort);
    iTcpPort = CxApplication::findConfig(CS_SectionBusManager, CS_EntryBusTcpPort, iTcpPort);

    _udpServer->setLocalIp("127.0.0.1");
    _udpServer->setLocalPort(iUdpPort);
    _udpServer->open();

    _tcpServer->setLocalIp("127.0.0.1");
    _tcpServer->setLocalPort(iTcpPort);
    _tcpServer->open();

    int iCount = 0;
    while (! _udpServer->connected())
    {
        if (iCount > 3000)
            break;
        CxApplication::waiting(1);
        ++iCount;
    }

    iCount = 0;
    while (! _tcpServer->connected())
    {
        if (iCount > 3000)
            break;
        CxApplication::waiting(1);
        ++iCount;
    }

    reportAllToFile();
}

bool BusManager::isStarted()
{
    return _udpServer && _tcpServer && _udpServer->connected() && _tcpServer->connected();
}

void BusManager::stop()
{
    if (_udpServer)
    {
        _udpServer->setAutoOpenInterval(0);
        _udpServer->removeObserver(this);
        _udpServer->close();
        GM_DELETEANDNULL_OBJECT(_udpServer);
    }

    if (_tcpServer)
    {
        _tcpServer->setAutoOpenInterval(0);
        _tcpServer->removeObserver(this);
        _tcpServer->close();
        GM_DELETEANDNULL_OBJECT(_tcpServer);
    }

    if (f_oBusTransferFileThread)
    {
        f_oBusTransferFileThread->stop();
        f_oBusTransferFileThread = NULL;
    }
}

//*001 从通道接收数据，找相关对象来处理
void BusManager::channel_receivedData(const uchar *pData, int iLength, void *oChannelRoad)
{
    assert(oChannelRoad);
    BusTerminal * oTerminal = allocateRoadTerminal((CxChannelRoad*)oChannelRoad);
    assert(oTerminal!=NULL);
    oTerminal->receivedData((const char *)pData, iLength);
}
