#include "psm_explain.h"
#include <ccxx/cxcrc.h>
#include <ccxx/cxapplication.h>

using namespace std;


#define DealReplyType_Error_Data (-1)


const int ci_psm_process_result_message_command        = 0x101;
const int ci_psm_process_result_file_write             = 0x102;
const int ci_psm_process_result_realtime_data_request  = 0x103;
const int ci_psm_process_result_realtime_data_post     = 0x104;

const int ci_psm_process_received_message_command        = 0x201;
const int ci_psm_process_received_file_write             = 0x202;
const int ci_psm_process_received_realtime_data_request  = 0x203;
const int ci_psm_process_received_realtime_data_post     = 0x204;


PsmExplainer::PsmExplainer() :
    _receivePacket(c_psm_head),
    _sendPacket(c_psm_head, 1, CxCrc::calcCRC16)
{
    _psmExplainNotify = NULL;
    _psmExplainWrite = NULL;

    _sendFileTime = 0;
    _sendingFileTime = 0;

    _lastReceivedDataTime = 0;

    _checkProtocolTm = new CxTimer();
    _checkProtocolTm->setTimerTimeOut(this);
    _checkProtocolTm->setInterval(1000);
    _sentSourceId = -1;
    _sentTargetId = -1;
    _receiveFileInfoTargetId = 0;

    _isProcessQueue = false;
}

PsmExplainer::~PsmExplainer()
{
    delete _checkProtocolTm;
}

int PsmExplainer::receivedData(const uchar *pData, int iLength)
{
    int iOldCount = _receivePacket.count();
    int iReceivedPacketCount = 0;
    _receivePacket.push((const char * )pData, iLength);
    if (_receivePacket.count() > 0)
    {
        iReceivedPacketCount = _receivePacket.count() - iOldCount;
        _lastReceivedDataTime = CxTime::currentSystemTime();
        bool bNoDeal = true;
        if (_psmExplainNotify)
        {
            bNoDeal = _psmExplainNotify->received_packet(this, iReceivedPacketCount) <= 0;
        }
        if (bNoDeal)
        {
//            cxPrompt()<<"PsmExplainer::DealData";
            dealData();
        }
        _receivePacket.clear();
    }
    return iReceivedPacketCount;
}

void PsmExplainer::dealData()
{
    const std::vector<PsmPacketData> & packets = _receivePacket.packets();
    for (size_t i = 0; i < packets.size(); ++i)
    {
        const PsmPacketData & packet = packets.at(i);
        dealPacketData(packet);
    }
}

int PsmExplainer::dealNack(const PsmPacketData & packet)
{
    const psm_packet_body_t & body = packet.body;
    switch (body.command)
    {
    case gct_file_information_write:
    case gct_file_content_write:
    {
        if (body.command == gct_file_information_write)
        {
            _sendFileCurrentIndex = -1;
        }
        //        outProtocolDebug() << "size : " << _sendFileTexts.size() << " ; index : " << _sendFileCurrentIndex << cxEndLine;
        ++_sendFileCurrentIndex;
        if (_sendFileCurrentIndex < _sendFileTexts.size())
        {
            const std::string & text = _sendFileTexts.at(_sendFileCurrentIndex);
            int iReason = body.reason;
            int iContainerId = body.container;
            int iSourceID = body.targetAddress;
            int iTargetId = body.sourceAddress;
            int iTag = body.resCommand;
            PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceID, iTargetId, iTag);
            return sendFileData(text, &attach);
        }
        else
        {
            sendFileComplete();
            _sendFileTexts.clear();
            _sendFileCurrentIndex = -1;
            _sendFileTime = 0;
            _sendingFileTime = 0;
            sendNextFilePath();
        }
    }
        break;
    default:
        break;
    }
    return TRUE;
}

int PsmExplainer::dealMessageCommand(const PsmPacketData & packet)
{
    if (packet.data && packet.length > 0)
    {
        char * pch = packet.data;
        size_t iCommand = strlen(pch) + 1;
        int iSize = packet.length - iCommand - sizeof(int);
        if (iSize >= 0)
        {
            const psm_packet_body_t & body = packet.body;
            std::string sCommand = pch; pch += iCommand; //sCommand
            int iParamLength = *(int *)pch; pch += sizeof(int); //iParamLength
            int iReason = body.reason;
            int iContainerId = body.container;
            int iSourceID = body.sourceOriginal;
//            int iSourceID = body.sourceAddress;
            int iTargetId = body.targetAddress;
            int iTag = body.resCommand;
            PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceID, iTargetId, iTag);
            if (iSize >= iParamLength && iParamLength > 0)
            {
                std::string sParam(pch, iParamLength); pch += iParamLength;//sParam
                receivedMessageCommand(sCommand.data(), sParam.data(), sParam.size(), &attach);
            }
            else
            {
                receivedMessageCommand(sCommand.data(), NULL, 0, &attach);
            }
            return DealReplyType_Ack;
        }
    }
    return DealReplyType_Error_Data;
}

int PsmExplainer::dealFileInformationWrite(const PsmPacketData & packet)
{
    _receivePathInfo = pathInfoFromBuffer(packet.data, packet.length);
    if (_receivePathInfo.fileSize > 0)
    {
        _receivedFileSize = 0;
        _receiveFileTexts.clear();
        _receiveFileInfoTargetId = packet.body.targetAddress;
        return DealReplyType_Ack;
    }
    else
    {
        return DealReplyType_Error_Data;
    }
}

int PsmExplainer::dealFileDataWrite(const PsmPacketData & packet)
{
    if (packet.data && packet.length > 0)
    {
        std::string sText((char *)packet.data, packet.length);
        _receiveFileTexts.push_back(sText);
        _receivedFileSize += sText.size();
        if (_receivedFileSize >= _receivePathInfo.fileSize)
        {
            const psm_packet_body_t & body = packet.body;
            int iReason = body.reason;
            int iContainerId = body.container;
            int iSourceID = body.sourceOriginal;
//            int iSourceID = body.sourceAddress;
            int iTargetId = _receiveFileInfoTargetId;//body.targetAddress;
            int iTag = body.resCommand;
            PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceID, iTargetId, iTag);
            receivedFileWrite(_receivePathInfo, _receiveFileTexts, &attach);
        }
    }
    return TRUE;
}

int PsmExplainer::dealRealtimeRequest(const PsmPacketData &packet)
{
    const psm_packet_body_t & body = packet.body;
    int iReason = body.reason;
    int iContainerId = body.container;
    int iSourceID = body.sourceOriginal;
//        int iSourceID = body.sourceAddress;
    int iTargetId = body.targetAddress;
    int iTag = body.resCommand;
    PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceID, iTargetId, iTag);
    receivedRealtimeDataRequest(&attach);
    return DealReplyType_Define;
}

int PsmExplainer::dealRealtimePost(const PsmPacketData &packet)
{
    if (packet.data && packet.length > 0)
    {
        const psm_packet_body_t & body = packet.body;
        if (packet.data && packet.length > 0 && packet.length < CI_PSM_PACKET_DATA)
        {
            int iReason = body.reason;
            int iContainerId = body.container;
            int iSourceID = body.sourceOriginal;
//                int iSourceID = body.sourceAddress;
            int iTargetId = body.targetAddress;
            int iTag = body.resCommand;
            PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceID, iTargetId, iTag);
            receivedRealtimeDataPost(body.paramType, packet.data, packet.length, body.paramCount, &attach);
        }
        return DealReplyType_Ack;
    }
    return DealReplyType_Error_Data;
}

int PsmExplainer::postMessageCommand(const char *sCommand, const char *sParam, int iParamLength, const PsmAttach * oAttach)
{
    if (! sCommand) return -1;

    size_t iCommand = strlen(sCommand) + 1;
    size_t iSize = iCommand; //sCommand
    iSize += sizeof(int) + iParamLength; //iParamLength sParam

    if(iSize>CI_psm_packet_max) return -1; //len is too big

    memset(_sendBuffer,0,CI_psm_packet_max);
    int pos = 0;
    memcpy(_sendBuffer+pos,sCommand,strlen(sCommand));
    pos+=iCommand;

    memcpy(_sendBuffer+pos,&iParamLength,sizeof(int));
    pos+=sizeof(int);

    if (sParam && iParamLength>0)
    {
        memcpy(_sendBuffer+pos,sParam,iParamLength);
        pos += iParamLength;
    }

    return postData(gct_message_command_param, 0, _sendBuffer, pos, 0, oAttach);
}

int PsmExplainer::postFile(const std::string &sFilePath, const PsmAttach * oAttach)
{
    if (oAttach)
    {
        _sendFilePathes[sFilePath] = * oAttach;
    }
    else
    {
        _sendFilePathes[sFilePath] = PsmAttach();
    }

    return sendNextFilePath();
}

int PsmExplainer::postFile(const CxFileSystem::PathInfo & pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    _sendFileSreamInfos.push(pathInfo);
    _sendFileSreamDatas.push(fileData);
    if (oAttach)
    {
        _sendFileSreamAttach.push(* oAttach);
    }
    else
    {
        _sendFileSreamAttach.push(PsmAttach());
    }

    return sendNextFilePath();
}

int PsmExplainer::postRealtimeDataRequest(const PsmAttach * oAttach)
{
    return postData(gct_realtime_data_request, 0, 0, 0, 0, oAttach);
}

int PsmExplainer::postRealtimeDataPost(int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach)
{
    return postData(gct_realtime_data_post, iParamType, pData, iDataLength, iParamCount, oAttach);
}

int PsmExplainer::postHeartJump(const PsmAttach *oAttach)
{
    return postData(gct_channel_hand, 0, 0, 0, 1, oAttach);
}

int PsmExplainer::sendFileData(const std::string & sFileData, const PsmAttach * oAttach)
{
    int r = 0;
    if (sFileData.size() > 0)
    {
        r = postData(gct_file_content_write, 0, sFileData.data(), sFileData.size(), 0, oAttach);
    }
    if (r > 0)
    {
        _sendingFileTime = CxTime::currentMsepoch();
    }
    return r;
}

int PsmExplainer::sendNextFilePath()
{
    int r = 0;
    if (_sendFilePathes.size() > 100)
    {
        _sendFilePathes.clear();
        r = -2;
        return r;
    }
    if (_sendFileSreamInfos.size() > 100)
    {
        CxContainer::clear(_sendFileSreamInfos);
        CxContainer::clear(_sendFileSreamAttach);
        CxContainer::clear(_sendFileSreamDatas);
        r = -2;
        return r;
    }
    if (_sendFileTime == 0 && _sendingFileTime == 0)
    {
        if (_sendFileSreamInfos.size() > 0)
        {
            _sendPathInfo = _sendFileSreamInfos.front();
            _sendFileTexts = _sendFileSreamDatas.front();
            PsmAttach attach = _sendFileSreamAttach.front();
            _sendFileSreamInfos.pop();
            _sendFileSreamDatas.pop();
            _sendFileSreamAttach.pop();
            _sendFileTime = CxTime::currentSystemTime();
            r = sendFileInfo(_sendPathInfo, & attach);
        }
        else if (_sendFilePathes.size() > 0)
        {
            std::map<std::string, PsmAttach>::iterator it = _sendFilePathes.begin();
            std::string sFilePath = it->first;
            PsmAttach attach = it->second;
            _sendFilePathes.erase(it);
            if (CxFile::load(sFilePath, _sendFileTexts, 1024 * 2) > 0)
            {
                _sendFileTime = CxTime::currentSystemTime();
                r = sendFileInfo(sFilePath, & attach);
            }
            else
            {
                r = -3;
            }
        }
        else
        {
            r = -4;
        }
    }
    else
    {
        r = TRUE;
    }
    return r;
}

void PsmExplainer::sendFileComplete()
{
    if (_isProcessQueue)
    {
//        cxPrompt() << "PsmExplainer::resultFileWrite -begin : " << _sendPathInfo.fileName;
        if(1>0)
        {
            CxMutexScope lock(_processLock);
            CxFileSystem::PathInfo filePathInfo = _sendPathInfo;
            filePathInfo.path = CxString::newString(_sendPathInfo.path);
            filePathInfo.fileName = CxString::newString(_sendPathInfo.fileName);
            _processResultFiles.push(PsmFile(this, filePathInfo, _sendPathInfoAttach));
        }
        PsmExplainer::threadEventNotify(this, ci_psm_process_result_file_write);
//        cxPrompt() << "PsmExplainer::receivedFileWrite -end";
    }
    else if (_psmExplainNotify)
    {
        _psmExplainNotify->result_file_write(this, _sendPathInfo, & _sendPathInfoAttach);
    }
}

int PsmExplainer::sendFileInfo(const std::string & sFilePath, const PsmAttach * oAttach)
{
    CxFileSystem::PathInfo pathInfo;
    if ( CxFileSystem::getPathInfo(sFilePath, &pathInfo) )
    {
        return sendFileInfo(pathInfo, oAttach);
    }
    return -1;
}

int PsmExplainer::sendFileInfo(const CxFileSystem::PathInfo &mPathInfo, const PsmAttach *oAttach)
{
    _sendPathInfo = mPathInfo;
    if (oAttach) _sendPathInfoAttach = * oAttach;
    std::string infoData = pathInfoToBuffer(mPathInfo);

    return postData(gct_file_information_write, 0, infoData.data(), infoData.size(), 0, oAttach);
}

std::string PsmExplainer::pathInfoToBuffer(const CxFileSystem::PathInfo & pathInfo)
{
    std::string sFileName = pathInfo.fileName;
    int64 iFileSize = pathInfo.fileSize;
    if (iFileSize <= 0 || iFileSize > 1024 * 1024 * 128)
    {
        return std::string();
    }
    std::string infoData = CxString::newString(sFileName);
    infoData.push_back('\0');
    infoData.resize(infoData.size() + sizeof(int));

    *(int*)( (infoData.data() + infoData.size()) - sizeof(int)) = (int)iFileSize;
    return infoData;
}

CxFileSystem::PathInfo PsmExplainer::pathInfoFromBuffer(const char * pData, size_t iLength)
{
    CxFileSystem::PathInfo pathInfo;
    if (! (pData && iLength > 0 && iLength < 4096))
        return pathInfo;
    std::string sFileName = pData;
    int64 iFileSize = 0;
    if (iLength > sFileName.size() + sizeof(int))
    {
        iFileSize = *(int*)(pData + iLength - sizeof(int));
    }
    if (iFileSize <= 0 || iFileSize > 1024 * 1024 * 128)
    {
        pathInfo.fileSize = 0;
    }
    else
    {
        pathInfo.fileSize = iFileSize;
    }
    pathInfo.fileName = sFileName;
    return pathInfo;
}


int PsmExplainer::postData(int iCommand, int iParamType, const char * pData, int iDataLength, int iParamCount, const PsmAttach * oAttach)
{
    int iReason = 0;
    int iContainerId = 0;
    int iSourceId = 0;
    int iTargetId = 0;
    int iTag = 0;
    if (oAttach!=NULL)
    {
        iReason = oAttach->reason;
        iContainerId = oAttach->containerId;
        iSourceId = oAttach->sourceId;
        iTargetId = oAttach->targetId;
        iTag = oAttach->tag;
    }
    psm_packet_body_t body;
    body.frameType      = 0;
    body.sourceOriginal     = iSourceId;
    if (_sentSourceId > 0)
        body.sourceAddress  = _sentSourceId;
    else
        body.sourceAddress  = iSourceId;
    body.resFrame     = 0;
    if (_sentTargetId > 0)
        body.targetAddress  = _sentTargetId;
    else
        body.targetAddress  = iTargetId;
    body.controlWord    = CIPsmControlCode_Initiactive;
    body.command        = iCommand;
    body.reason         = iReason;
    body.resCommand            = iTag;
    body.container      = iContainerId;
    body.paramType      = iParamType;
    body.paramCount     = iParamCount;
    return postPacketData(body, pData, iDataLength);
}

int PsmExplainer::responseAck(int iCommand, const PsmAttach * oAttach)
{
    int iReason = 0;
    int iContainerId = 0;
    int iSourceId = 0;
    int iTargetId = 0;
    int iTag = 0;
    if (oAttach)
    {
        iContainerId = oAttach->containerId;
        iSourceId = oAttach->sourceId;
        iTargetId = oAttach->targetId;
        iTag = oAttach->tag;
    }
    psm_packet_body_t body;
    body.frameType      = 0;
    body.sourceOriginal     = iSourceId;
//    if (_sentSourceId != -1)
    if (_sentSourceId >0)
        body.sourceAddress  = _sentSourceId;
    else
        body.sourceAddress  = iSourceId;
    body.resFrame     = 0;
    body.targetAddress  = iTargetId;
    body.controlWord    = CIPsmControlCode_NACK;
    body.command        = iCommand;
    body.reason         = iReason;
    body.resCommand            = iTag;
    body.container      = iContainerId;
    body.paramType      = 0;
    body.paramCount     = 0;
    return postPacketData(body, NULL, 0);
}

int PsmExplainer::responseNack(int iCommand, int iErrorid, const PsmAttach * oAttach)
{
    int iReason = iErrorid;
    int iContainerId = 0;
    int iSourceId = 0;
    int iTargetId = 0;
    int iTag = 0;
    if (oAttach)
    {
        iContainerId = oAttach->containerId;
        iSourceId = oAttach->sourceId;
        iTargetId = oAttach->targetId;
        iTag = oAttach->tag;
    }
    psm_packet_body_t body;
    body.frameType      = 0;
    body.sourceOriginal     = iSourceId;
//    if (_sentSourceId != -1)
    if (_sentSourceId >0)
        body.sourceAddress  = _sentSourceId;
    else
        body.sourceAddress  = iSourceId;
    body.resFrame     = 0;
    body.targetAddress  = iTargetId;
    body.command        = iCommand;
    body.reason         = iReason;
    body.resCommand            = iTag;
    body.container      = iContainerId;
    body.paramType      = 0;
    body.paramCount     = 0;
    return postPacketData(body, NULL, 0);
}

int PsmExplainer::postPacketData(const psm_packet_body_t & body, const char * pData, int iDataLength)
{
    int iResult = _sendPacket.setPack(body, pData, iDataLength);
    if (iResult > 0)
    {
        return sendData(_sendPacket.data(), _sendPacket.size(), body.targetAddress);
    }
    else
    {
        cxWarning() << "postData : fail, data length too big!!!";
        return iResult;
    }
}

void PsmExplainer::dealPacketData(const PsmPacketData &packet)
{
    const psm_packet_body_t & body = packet.body;
    if (body.controlWord == CIPsmControlCode_NACK)
    {
        dealNack(packet);
    }
    else
    {
        int iDeal = 0;
//        cxPrompt()<<"PsmExplainer::dealPacketData:" <<body.command;
        switch (body.command)
        {
        //*heart jump
        case gct_channel_hand:
        {
            iDeal = DealReplyType_Ack;
        }
            break;
            //*message
        case gct_message_command_param:
        {
            iDeal = dealMessageCommand(packet);
        }
            break;
            //*realtime
        case gct_realtime_data_request:
        {
            iDeal = dealRealtimeRequest(packet);
        }
            break;
        case gct_realtime_data_post:
        {
            iDeal = dealRealtimePost(packet);
        }
            break;
            //*file
        case gct_file_information_write:
        {
            iDeal = dealFileInformationWrite(packet);
        }
            break;
        case gct_file_content_write:
        {
            iDeal = dealFileDataWrite(packet);
        }
            break;
        case gct_file_shell:
        {
//            iDeal = dealFileShell(packet);
        }
            break;
        default:
        {
            //预留外面注册处理的
            //*                typedef int (*gci_fn_command_t)( int iCommand, int iReason, int iParamType, const char * pData, int iDataLength, int iParamCount, int iContainerId, int iSourceID, int iTargetId, int iTag );
            //*                inline void registerCommand(int iCommand, gci_fn_command_t fn) { _fnCommands[iCommand] = fn; }
            //*                std::map<int, gci_fn_command_t> _fnCommands;
            //                gci_fn_command_t fn = CxContainer::value(_fnCommands, body.command, (gci_fn_command_t)0);
            //                if (fn != 0)
            //                {
            //                    int iCommand = body.command;
            //                    int iReason = body.reason;
            //                    int iParamType = body.paramType;
            //                    const char * pData = (const char *)packet.data;
            //                    int iDataLength = packet.length;
            //                    int iParamCount = body.paramCount;
            //                    int iContainerId = body.container;
            //                    int iSourceID = body.sourceAddress;
            //                    int iTargetId = body.targetAddress;
            //                    int iTag = body.res;
            //                    iDeal = fn(iCommand, iReason, iParamType, pData, iDataLength, iParamCount, iContainerId, iSourceID, iTargetId, iTag);
            //                }
        }
            break;
        }
        if (iDeal == DealReplyType_Ack)
        {
            int iReason = body.reason;
            int iContainerId = body.container;
            int iSourceID = body.targetAddress;
            int iTargetId = body.sourceAddress;
            int iTag = body.resCommand;
            PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceID, iTargetId, iTag);
            responseAck(body.command, &attach);
        }
        else if (iDeal < 0)
        {
            int iReason = body.reason;
            int iContainerId = body.container;
            int iSourceID = body.targetAddress;
            int iTargetId = body.sourceAddress;
            int iTag = body.resCommand;
            PsmAttach attach = PsmAttach(iReason, iContainerId, iSourceID, iTargetId, iTag);
            responseNack(body.command, iDeal, &attach);
        }
    }
}

void PsmExplainer::start()
{
    _checkProtocolTm->start();
    _fileSavePath = CxFileSystem::mergeFilePath(CxApplication::applicationDeployPath(), "temp");
}

void PsmExplainer::stop()
{
    _checkProtocolTm->stop();
}

string PsmExplainer::reportSelf()
{
    string sLastTime = CxTime::toString(_lastReceivedDataTime);
    std::string sResult = CxString::format(" sentSourceId: %d, sentTargetId: %d, lastReceivedDataTime: %s ",
                                           _sentSourceId, _sentTargetId, sLastTime.c_str());
    return sResult;
}

void PsmExplainer::timer_timeOut(const CxTimer *oTimer)
{
    if (oTimer == _checkProtocolTm)
    {
        checkProtocol();
    }
}

void PsmExplainer::checkProtocol()
{
    //发送超时
    if (_sendingFileTime != 0)
    {
        if (CxTime::milliSecondDifferToNow(_sendingFileTime) > 1000)
        {
            _sendFileTexts.clear();
            _sendFileCurrentIndex = -1;
            _sendFileTime = 0;
            _sendingFileTime = 0;
        }
    }
    else if (_sendFileTime != 0)
    {
        if (CxTime::milliSecondDifferToNow(_sendFileTime) > 1000)
        {
            _sendFileTexts.clear();
            _sendFileCurrentIndex = -1;
            _sendFileTime = 0;
            _sendingFileTime = 0;
        }
    }
}

int PsmExplainer::sendData(const char *pData, int iLength, int iTargetId)
{
    GM_INVALID_RETURE_(_psmExplainWrite, -1);
    return _psmExplainWrite->send_data(pData, iLength, iTargetId);
}

void PsmExplainer::receivedMessageCommand(const char *sCommand, const char *sParam, int iParamLength, const PsmAttach *oAttach)
{
    if (_isProcessQueue)
    {
//        cxPrompt() << "PsmExplainer::receivedMessageCommand -begin : " << sCommand;
        if(1>0)
        {
            CxMutexScope lock(_processLock);
            _processMessages.push(PsmMessage(this, string(sCommand), string(sParam, iParamLength), * oAttach));
        }
        PsmExplainer::threadEventNotify(this, ci_psm_process_received_message_command);
//        cxPrompt() << "PsmExplainer::receivedMessageCommand -end : ";
    }
    else if (_psmExplainNotify)
    {
        _psmExplainNotify->received_message_command(this, sCommand, sParam, iParamLength, oAttach);
    }
}

void PsmExplainer::receivedFileWrite(const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    if (_isProcessQueue)
    {
//        cxPrompt() << "PsmExplainer::receivedFileWrite -begin : " << pathInfo.fileName;
        std::string sFilePath = CxFileSystem::mergeFilePath( _fileSavePath , pathInfo.fileName );
        CxFile::save(sFilePath, fileData, std::string());
        if(1>0)
        {
            CxMutexScope lock(_processLock);
            CxFileSystem::PathInfo filePathInfo = pathInfo;
            filePathInfo.path = CxString::newString(_fileSavePath);
            filePathInfo.fileName = CxString::newString(pathInfo.fileName);
            _processFiles.push(PsmFile(this, filePathInfo, * oAttach));
        }
        PsmExplainer::threadEventNotify(this, ci_psm_process_received_file_write);
//        cxPrompt() << "PsmExplainer::receivedFileWrite -end";
    }
    else if (_psmExplainNotify)
    {
        _psmExplainNotify->received_file_write(this, pathInfo, fileData, oAttach);
    }
}

void PsmExplainer::receivedRealtimeDataRequest(const PsmAttach *oAttach)
{
    if (_isProcessQueue)
    {
//        cxPrompt() << "PsmExplainer::receivedRealtimeDataRequest -begin : ";
        if(1>0)
        {
            CxMutexScope lock(_processLock);
            _processRealtimeDataRequests.push(PsmData(this, 0, * oAttach));
        }
        PsmExplainer::threadEventNotify(this, ci_psm_process_received_realtime_data_request);
//        cxPrompt() << "PsmExplainer::receivedRealtimeDataRequest -end : ";
    }
    else if (_psmExplainNotify)
    {
        _psmExplainNotify->received_realtime_data_request(this, oAttach);
    }
}

void PsmExplainer::receivedRealtimeDataPost(int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    if (_isProcessQueue)
    {
//        cxPrompt() << "PsmExplainer::receivedRealtimeDataPost -begin : " << iParamType << " " << iParamCount;
        if(1>0)
        {
            CxMutexScope lock(_processLock);
            _processRealtimeDatas.push(PsmRealtimeData(this, iParamType, string(pData, iDataLength), iParamCount, * oAttach));
        }
        PsmExplainer::threadEventNotify(this, ci_psm_process_received_realtime_data_post);
//        cxPrompt() << "PsmExplainer::receivedRealtimeDataPost -end";
    }
    else if (_psmExplainNotify)
    {
        _psmExplainNotify->received_realtime_data_post(this, iParamType, pData, iDataLength, iParamCount, oAttach);
    }
}

void PsmExplainer::threadEventNotify(PsmExplainer *oPsm, int iMsg, int iTag, const void *pData, int iLength, void *oSource)
{
    CxApplication::pushProcessCallBack(PsmExplainer::processThreadNofity, iMsg, iTag, pData, iLength, oSource, oPsm);
    CxApplication::signalMainThread();
}

void PsmExplainer::processThreadNofity(int iMsg, int iTag, const void * pData, int iLength, void * oSource, void * oTarget)
{
    GM_INVALID_RETURE(oTarget != 0);
    PsmExplainer * oPsm = (PsmExplainer *)oTarget;
    oPsm->processThreadNofityImpl(iMsg, iTag, pData, iLength, oSource);
}

void PsmExplainer::processThreadNofityImpl(int iMsg, int iTag, const void * pData, int iLength, void * oSource)
{
    switch (iMsg)
    {
        //*message
    case ci_psm_process_received_message_command:
    {
        while (1)
        {
            PsmMessage psmMessage;
            if(1>0)
            {
                CxMutexScope lock(_processLock);
                if (! _processMessages.empty())
                {
                    psmMessage = _processMessages.front();
                    _processMessages.pop();
                }
            }
            if (psmMessage.psm)
            {
                _psmExplainNotify->received_message_command(this, psmMessage.command.data(), psmMessage.param.data(), psmMessage.param.size(), & psmMessage.attach);
            }
            else
            {
                break;
            }
        }
    }
        break;
        //*realtime
    case ci_psm_process_received_realtime_data_request:
    {
        while (1)
        {
            PsmData psmData;
            if(1>0)
            {
                CxMutexScope lock(_processLock);
                if (! _processRealtimeDataRequests.empty())
                {
                    psmData = _processRealtimeDataRequests.front();
                    _processRealtimeDataRequests.pop();
                }
            }
            if (psmData.psm)
            {
                _psmExplainNotify->received_realtime_data_request(this, & psmData.attach);
            }
            else
            {
                break;
            }
        }
    }
        break;
    case ci_psm_process_received_realtime_data_post:
    {
        while (1)
        {
            PsmRealtimeData psmRealtimeData;
            if(1>0)
            {
                CxMutexScope lock(_processLock);
                if (! _processRealtimeDatas.empty())
                {
                    psmRealtimeData = _processRealtimeDatas.front();
                    _processRealtimeDatas.pop();
                }
            }
            if (psmRealtimeData.psm)
            {
                _psmExplainNotify->received_realtime_data_post(this, psmRealtimeData.paramType, psmRealtimeData.data.data(), psmRealtimeData.data.size(), psmRealtimeData.paramCount, & psmRealtimeData.attach);
            }
            else
            {
                break;
            }
        }
    }
        break;
    case ci_psm_process_received_file_write:
    {
        while (1)
        {
            PsmFile psmFile;
            if(1>0)
            {
                CxMutexScope lock(_processLock);
                if (! _processFiles.empty())
                {
                    psmFile = _processFiles.front();
                    _processFiles.pop();
                }
            }
            if (psmFile.psm)
            {
                std::vector<string> fileData;
                _psmExplainNotify->received_file_write(this, psmFile.filePathInfo, fileData, & psmFile.attach);
            }
            else
            {
                break;
            }
        }
    }
        break;
    default:
    {
    }
        break;
    }
}
