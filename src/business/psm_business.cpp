#include "psm_business.h"

using namespace std;


void PsmBusiness::toContext(CxISetSkv &context) const
{
    TBIBase::toContext(context);

}

void PsmBusiness::fromContext(const CxIGetSkv &context)
{
    TBIBase::fromContext(context);

    string sLocalStructTypies;
    sLocalStructTypies = context.getValue(CS_Entry_LocalStructTypies, sLocalStructTypies);
    std::map<std::string, std::string> localStructTypies = CxString::splitToMap(sLocalStructTypies, '=', ',');
    _localStructTypies = CxContainer::valuesTo(localStructTypies, (int)0);

    string sLocalOperateTypies;
    sLocalOperateTypies = context.getValue(CS_Entry_LocalOperateTypies, sLocalOperateTypies);
    std::map<std::string, std::string> localOperateTypies = CxString::splitToMap(sLocalOperateTypies, '=', ',');
    _localOperateTypies = CxContainer::valuesTo(localOperateTypies, (int)0);

    string sMapingTypeNames;
    sMapingTypeNames = context.getValue(CS_Entry_MapingTypeNames, sMapingTypeNames);
    //_mapingTypeNames = CxString::splitToMap_reverse(sMapingTypeNames, '=', ',');  ttt

    string sMapingFieldNames;
    sMapingFieldNames = context.getValue(CS_Entry_MapingFieldNames, sMapingFieldNames);
    _mapingFieldNames = CxString::splitToMap(sMapingFieldNames, '=', ',');

    string sTransparentCommands;
    sTransparentCommands = context.getValue(CS_Entry_TransparentCommands, sTransparentCommands);
    _transparentCommands = CxString::split(sTransparentCommands, ',');
}

void PsmBusiness::describeSelf(CxDescribeUnit *oDescribeUnit) const
{
    TBIBase::describeSelf(oDescribeUnit);

    CxDescribeString* oLocalStructTypiesDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_Entry_LocalStructTypies); oLocalStructTypiesDescribe->initValue( CxContainer::joinToString(_localStructTypies, std::string("="), std::string(",")) );
    CxDescribeString* oLocalOperateTypiesDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_Entry_LocalOperateTypies); oLocalOperateTypiesDescribe->initValue( CxContainer::joinToString(_localOperateTypies, std::string("="), std::string(",")) );
    CxDescribeString* oMapingTypeNamesDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_Entry_MapingTypeNames); oMapingTypeNamesDescribe->initValue( CxContainer::joinToString(_mapingTypeNames, std::string("="), std::string(",")) );
    CxDescribeString* oMapingFieldNamesDescribe = oDescribeUnit->createAndRegister<CxDescribeString>(CS_Entry_MapingFieldNames); oMapingFieldNamesDescribe->initValue( CxContainer::joinToString(_mapingFieldNames, std::string("="), std::string(",")) );
}

void PsmBusiness::toReport(std::vector<string> &sReports) const
{
    TBIBase::toReport(sReports);

    sReports.push_back(std::string("LocalStructTypies: ") + CxContainer::joinToString(_localStructTypies, std::string("="), std::string(",")));
    sReports.push_back(std::string("LocalOperateTypies: ") + CxContainer::joinToString(_localOperateTypies, "=", ","));
    sReports.push_back(std::string("MapingTypeNames: ") + CxContainer::joinToString(_mapingTypeNames, "=", ","));
    sReports.push_back(std::string("MapingFieldNames: ") + CxContainer::joinToString(_mapingFieldNames, "=", ","));
}

void PsmBusiness::businessAfterStart()
{
    TBIBase::businessAfterStart();

    CxMsObjectDataManager::addObserver(this);
}

void PsmBusiness::businessAfterStop()
{
    TBIBase::businessAfterStop();

    CxMsObjectDataManager::removeObserver(this);
}

void PsmBusiness::registerProtocol(ProtocolBase *oProtocol)
{
    TBIBase::registerProtocol(oProtocol);

    if (oProtocol && oProtocol->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
    {
        PsmProtocol* oPsmProtocol = (PsmProtocol*)oProtocol;
        oPsmProtocol->addObserver( this );
    }
}

void PsmBusiness::unregisterProtocol()
{
    TBIBase::unregisterProtocol();

    ProtocolBase * oProtocol = _protocol;
    if (oProtocol && oProtocol->typeSimpleName() == CS_ProtocolTypeSimpleName_Psm)
    {
        PsmProtocol* oPsmProtocol = (PsmProtocol*)oProtocol;
        oPsmProtocol->removeObserver( this );
    }
}

void PsmBusiness::received_message_command(const PsmProtocol *oPsm, const char *sCommand, const char *sParam, int iParamLength, const PsmAttach *oAttach)
{
    assert(oPsm != NULL);

    if (iParamLength < 500)
    {
        outPromptInfo() << "received message Command : " << sCommand << " , Param : " << sParam << cxEndLine;
    }
    else
    {
        outPromptInfo() << "received message Command : " << sCommand << " , ParamLength : " << iParamLength << cxEndLine;
    }

    PsmProtocol * oPsmProtocol = (PsmProtocol *)oPsm;
    GString sCommand1 = sCommand;
    sCommand1 = CxString::trim(sCommand1);

    GString sParam1(sParam, iParamLength);

    outBusinessDebug()<<sCommand1<<sParam1<<cxEndLine;

    if(_transparentCommands.size()>0 && CxContainer::contain(_transparentCommands, sCommand1))
    {
        uint16 cmdLen = sCommand1.size();
        int dataLen = cmdLen+iParamLength+sizeof(uint16);
        if(dataLen<=GCL_LEN_DATA_EM)
        {
            int pos = 0;
            char cData[GCL_LEN_DATA_EM]={0};

            memcpy(&cData[pos],&cmdLen,sizeof(uint16));
            pos+=sizeof(uint16);
            memcpy(&cData[pos],sCommand,cmdLen);
            pos+=cmdLen;
            memcpy(&cData[pos],sParam,iParamLength);
            pos+=iParamLength;
            //消息命令转发
            _protocol->sendMsg(GCL_CMD_SYS_MSG_TRAN,0,dataLen,cData);
        }
        else
        {
            outBusinessPrompt()<<"pramter too big:"<<GString(sCommand)<<cxEndLine;
        }
    }
    else if(sCommand1 == "req.allyx")
    {
        oPsmProtocol->senddAllYX();
    }
    else if(sCommand1 == "req.allyc")
    {
        oPsmProtocol->sendAllYC();
    }
    else if(sCommand1 == "send.yk.1")
    {
        oPsmProtocol->dealYK_01(sParam1);
    }
    else if(sCommand1 == "send.yt.1")
    {
        oPsmProtocol->dealYT_01(sParam1);
    }
    else if(sCommand1 == "send.ys.1")
    {
        oPsmProtocol->dealYS_01(sParam1);
    }
    else if(sCommand1 == "send.yx.1")
    {
        oPsmProtocol->dealYX_01(sParam1);
    }
    else if(sCommand1 == "send.yc.1")
    {
        oPsmProtocol->dealYC_01(sParam1);
    }
    else if(sCommand1 == "send.vtl.1")
    {
        //oPsmProtocol->dealVTL_01(vParam);
        oPsmProtocol->sendMsg(GCL_CMD_SYS_SC_W,0,iParamLength,(char*)sParam);
    }
    else if(sCommand1 == "send.event.1")
    {

    }
    else if(sCommand1 == "send.soe.1")
    {

    }
    else if(sCommand1 == "send.message.1")
    {
        oPsmProtocol->dealMessage_01(sParam1);
    }
    else if(sCommand1 == "send.key.value")//键值对
    {

    }
    else if(sCommand1 == "send.sql.data")//sql 语句
    {
        oPsmProtocol->sendMsg(GCL_CMD_SYS_SQL,0,iParamLength,(char*)sParam);
    }
    else if(sCommand1 == "send.xml.data")//xml 数据流
    {
        oPsmProtocol->sendMsg(GCL_CMD_SYS_XML,0,iParamLength,(char*)sParam);
    }
    else if(sCommand1 == "send.command")
    {
        oPsmProtocol->sendMsg(GCL_CMD_SYS_CMD,0,iParamLength,(char*)sParam);
    }
    else
    {
//        GString sKey = CxString::trim(sCommand1);
//        if(sKey.empty())sKey = "00000";

//        int nRet = 0; //以数字字符串作为命令码 XML格式文件
//        CxString::fromString(sKey,nRet);
//        if(nRet>=0)
//        {
//            GString sFilePath = CxFileSystem::mergeFilePath( GclConfig::tempPath(),"Recv");
//            CxFileSystem::createDirMultiLevel(sFilePath);
//            GString sFileName = CxFileSystem::mergeFilePath(sFilePath,CxTime::toString(CxTime::currentSystemTime(),0,0,0)+"_"+sKey+".xml");
//            if(CxFile::save(sFileName, sParam1))
//            {
//                GString sBuff = CxString::format("fc=%s\nfile=%s",sKey.data(),sFileName.data());
//                oPsmProtocol->sendMsg(GCL_CMD_SYS_XML_CICS,0,sBuff.length(),(char*)sBuff.data());
//                outProtocolDebug()<<"sendMsg:"<<sBuff<<cxEndLine;
//            }
//        }
    }
}

volatile int f_received_file_count = 0;

void PsmBusiness::received_file_write(const PsmProtocol *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<std::string> &fileData, const PsmAttach *oAttach)
{
//    assert(oPsm != NULL);

//    GString sPath = CxFileSystem::mergeFilePath(GclConfig::tempPath(),"Recv");
//    CxFileSystem::createDirMultiLevel(sPath);
//    GString fileName = CxTime::toString(CxTime::currentSystemTime(),0,0,0)+"_"+ pathInfo.fileName;
//    GString sFilePath = CxFileSystem::mergeFilePath(sPath ,fileName );

//    bool bSave = CxFile::save(sFilePath, fileData, std::string());

//    if(bSave && CxFileSystem::extractFileSuffixName(sFilePath)==".xml")
//    {
//        GString sBuff = CxString::format("fc=00000\nfile=%s",sFilePath.data());
//        PsmProtocol * oPsmProtocol = (PsmProtocol *)oPsm;
//        if(oPsmProtocol)oPsmProtocol->sendMsg(GCL_CMD_SYS_XML_CICS,0,sBuff.length(),(char*)sBuff.data());
//    }
    outPromptInfo() << "received file - fileName=" << pathInfo.fileName << " fileSize=" << pathInfo.fileSize << "fileCount=" << ++f_received_file_count << cxEndLine;
}

void PsmBusiness::received_realtime_data_request(const PsmProtocol *oPsm, const PsmAttach *oAttach)
{
    assert(oPsm != NULL);

    CxInterinfoOutStream outStream; outStream.noend();

    outStream << typeSimpleName() << "received realtime_data_request : " ;
    outStream << "-reason" << oAttach->reason;
    outStream << "-containerId" << oAttach->containerId;
    outStream << "-sourceId" << oAttach->sourceId;
    outStream << "-targetID" << oAttach->targetId;
    outStream << "-tag" << oAttach->tag;
    outStream << cxEndLine;
}

void PsmBusiness::received_realtime_data_post(const PsmProtocol *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    assert(oPsm != NULL);

//    outPromptInfo() << "received realtime_data_post -iParamType : " << iParamType << " -iDataLength " << iDataLength << cxEndLine;
}

int PsmBusiness::sendMessage(const string & sCommand, const string &sParam)
{
    int r = 0;
    PsmProtocol * oPsm = psmProtocol();
    if (oPsm && (sParam.size() < 4096 - 96 - sCommand.size()))
    {
        r = oPsm->postMessageCommand(sCommand.data(), sParam.c_str(), sParam.size());
    }

    outPromptInfo() << "post message Command : " << sCommand << " , paramLength : " << sParam.size() << ", return : " << r << cxEndLine;
//    if (sParam.size() < 500)
//    {
//         outPromptInfo() << "post message Command : " << sCommand << " , param : " << sParam << ", return : " << r << cxEndLine;
//    }
//    else
//    {
//        outPromptInfo() << "post message Command : " << sCommand << " , paramLength : " << sParam.size() << ", return : " << r << cxEndLine;
//    }

    return r;
}

int PsmBusiness::sendFile(const string &sFilePath)
{
    int r = 0;

    PsmProtocol * oPsm = psmProtocol();

    if (oPsm)
    {
        r = oPsm->postFile(sFilePath);
    }

    outPromptInfo() << "postFile  FilePath : " << sFilePath << ", return : " << r << cxEndLine;

    return r;
}

