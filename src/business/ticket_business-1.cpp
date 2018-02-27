#include "ticket_business.h"
#include <script/gcl_script.h>

using namespace std;

const string CS_EFile_Command = "send.efile.1";
const int CI_EFILE_LINK_RESP = 0x90000;
const int CI_EFILE_DATA_RESP = 0xC0000;

const string CS_EFile_Head_Source = "F_Source";
const string CS_EFile_Head_Target = "F_Target";
const string CS_EFile_Head_FunctionId = "F_FunctionId";
const string CS_EFile_Head_FunctionCode = "F_FunctionCode";
const string CS_EFile_Head_FunctionRef = "F_FunctionRef";
const string CS_EFile_Head_OperateType = "F_OperateType";
const string CS_EFile_Head_Transdate = "F_Transdate";
const string CS_EFile_Head_Error = "F_Error";
const string CS_EFile_Head_Ext = "F_Ext";

TicketBusinessFactory* fn_regTicketBusinessFactory()
{
    BusinessFactoryManager::createAndRegister<TicketBusinessFactory>();
    return NULL;
}

TicketBusinessFactory* TicketBusinessFactory::s_instance = fn_regTicketBusinessFactory();


TicketBusiness::TicketBusiness()
{

}

TicketBusiness::~TicketBusiness()
{
}

void TicketBusiness::toContext(CxISetSkv &context) const
{
    PsmBusiness::toContext(context);

}

void TicketBusiness::fromContext(const CxIGetSkv &context)
{
    PsmBusiness::fromContext(context);
    //初始化
    init();
}

void TicketBusiness::toReport(vector<string> &sReports) const
{
    PsmBusiness::toReport(sReports);

}

void TicketBusiness::businessAfterStart()
{
    PsmBusiness::businessAfterStart();

}

void TicketBusiness::businessAfterStop()
{
    PsmBusiness::businessAfterStop();

}


//******************************************************************************************************

void TicketBusiness::received_message_command(const PsmProtocol *oPsm, const char *pCommand, const char *pParam, int iParamLength, const PsmAttach * oAttach)
{
    string sCommand = string(pCommand);

    map<string,string> vCommand = CxString::splitToMap(sCommand,'=',';');

    string sKey = "send.efile.1";
    //E文件
    if(CxContainer::contain(vCommand,sKey))
    {




    }

    //E文件处理
    if(sCommand.find("send.efile.1")!= string::npos)
    {
        outBusinessPrompt()<<pCommand<<iParamLength<<pParam;

        map<string, string> declare;
        vector<EFileElement> vObj;
        //E文件解析
        if(ExplainEfile::explain(pParam,iParamLength,declare,vObj)>0)
        {
            _currDeclare = declare;
            _currFunctionCode.clear();
            //数据处理
            dealData(oPsm,declare,vObj,oAttach);

            //存文件
            GString sToday = CxTime::toStringDay(CxTime::currentSepoch(),'-');
            GString sPath = CxFileSystem::mergeFilePath(GclConfig::tempPath(),sToday);
            CxFileSystem::createDirMultiLevel(sPath);
            GString fileName = CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0)+"_Recv_"+ _currFunctionCode+".efl";
            GString sFilePath = CxFileSystem::mergeFilePath(sPath ,fileName);
            bool bSave = CxFile::save(sFilePath, string(pParam,iParamLength));
            if(bSave)
            {
                GclLuaEFile::dealFile(_currFunctionCode.data(),sFilePath.data());
            }
        }
        else
        {
            outBusinessPrompt()<<"Efile Parse is Failed";
        }
    }
    else
    {
        PsmBusiness::received_message_command(oPsm, pCommand, pParam, iParamLength, oAttach);
    }
}

void TicketBusiness::received_file_write(const PsmProtocol *oPsm, const CxFileSystem::PathInfo &pathInfo, const vector<GString> &fileData, const PsmAttach * oAttach)
{
    PsmBusiness::received_file_write(oPsm, pathInfo, fileData, oAttach);

    GString sToday = CxTime::toStringDay(CxTime::currentSepoch(),'-');

    GString sPath = CxFileSystem::mergeFilePath(GclConfig::tempPath(),sToday);
    CxFileSystem::createDirMultiLevel(sPath);

    GString sNow = CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0);
    GString fileName = pathInfo.fileName;
    //判断文件名是否含有yyyymmdd
    if(pathInfo.fileName.find(sNow.substr(0,8))== string::npos)
    {
        fileName = CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0)+"_Recv_"+ pathInfo.fileName;
    }

    GString sFilePath = CxFileSystem::mergeFilePath(sPath ,fileName );

    bool bSave = CxFile::save(sFilePath, fileData, std::string());

    cxDebug()<<"TicketBusiness - received_file_write - save file " << sFilePath;

    if(!bSave) return;
//    return;

    string suffixName = CxFileSystem::extractFileSuffixName(sFilePath);


    if(suffixName ==".xml")//xml文件
    {
        GString sBuff = CxString::format("fc=00000\nfile=%s",sFilePath.data());
        PsmProtocol * oPsmProtocol = (PsmProtocol *)oPsm;
        if(oPsmProtocol)oPsmProtocol->sendMsg(GCL_CMD_SYS_XML_CICS,0,sBuff.length(),(char*)sBuff.data());
    }
    else if(suffixName ==".efl")//e文件
    {
        map<string, string> declare;
        vector<EFileElement> vObj;
        //E文件解析
        if(ExplainEfile::loadFromFile(sFilePath,declare,vObj)>0)
        {
            _currDeclare = declare;
             _currFunctionCode.clear();
             //数据处理
            dealData(oPsm,declare,vObj);
            //调用脚本
            GclLuaEFile::dealFile(_currFunctionCode.data(),sFilePath.data());
        }
        else
        {
            outBusinessPrompt()<<"Efile Parse is Failed";
        }
    }
}

void TicketBusiness::msObjects_changed(const string &sTypeName, const vector<string> &sNames)
{
    PsmBusiness::msObjects_changed(sTypeName, sNames);

}


void TicketBusiness::received_realtime_data_request(const PsmProtocol *oPsm, const PsmAttach *oAttach)
{
    PsmBusiness::received_realtime_data_request(oPsm,oAttach);

}

void TicketBusiness::received_realtime_data_post(const PsmProtocol *oPsm, int iParamType, const char *pData, int iDataLength, int iParamCount, const PsmAttach *oAttach)
{
    PsmBusiness::received_realtime_data_post(oPsm,iParamType,pData,iDataLength,iParamCount,oAttach);
}

///////////////////////////////////////////////////////////////////////
int TicketBusiness::dealData(const PsmProtocol * oPsm,map<string, string> &declare, vector<EFileElement> &vObj,const PsmAttach * oAttach)
{
    //E文件为空
    if(vObj.size()<1) return  GCL_ERR_FAIL;

    EFileElement obj;
    GString className,instanceName;
    vector<string> vField;

    //头部处理
    obj = vObj.at(0);
    obj.getName(className,instanceName);
    obj.getField(vField);
    if(className!="Head")
    {
        return GCL_ERR_FORMAT; //格式错误
    }
    EFileElement head = obj;
    //头部处理
    uint32 fc = 0;
    dealHead(oPsm,declare,obj,fc,oAttach);

    _currFunctionCode = CxString::format("%05X",fc);
    //数据处理
    for(size_t i = 1;i<vObj.size();++i)
    {
        obj = vObj.at(i);
        obj.getName(className,instanceName);
        if((className=="TicketMain")|| (className=="TicketDetail"))//票主子表
        {
            //保存到实时库
            vector<int> columnTypes = getColumnTypesByClassName(className,vField);
            obj.saveTable(columnTypes);
        }
        else if(className == "Measure")//测量点
        {
            outBusinessPrompt()<<"Measure:" << className ;
        }
        else if(className == "SystemInfo")//系统信息
        {
            outBusinessPrompt()<<"SystemInfo:" << className ;
        }
        else
        {
            outBusinessPrompt()<<"other:" << className ;
        }
    }

    switch(fc&0xffff)
    {
    case  0x0200://票查询
//        respTickerReq(oPsm,declare,head,oAttach);
        break;
    case  0x0201://票传递
        break;
    case  0x0202://票执行
        break;
    case 0x0203://票回传
        break;
    case 0x0204://步回传
        break;
    default:
        break;
    }

    return GCL_SUCCED;
}

int TicketBusiness::dealHead(const PsmProtocol * oPsm,map<string, string> &declare, EFileElement &obj,uint32 &functionCode,const PsmAttach * oAttach)
{
    EFileElement Head = obj;
    //生成头部
    createHead(CI_EFILE_LINK_RESP,Head,functionCode,oAttach);

    _vEFileEle.clear();
    _vEFileEle.push_back(Head);

    //生成E文件
    vector<string> sLines = ExplainEfile::toEfileBuffer(declare,_vEFileEle);
    string sPram = CxString::join(sLines,'\n');

    //返回功能码
    string sRetfc= CxString::format("%05X",(functionCode&0x0000ffff)|CI_EFILE_LINK_RESP);
    //发送E文件
    string sCommand = "send.efile.1="+sRetfc;

    int  iRemoteTerminalId = CxString::toInt32("0x"+_recvSouceID);

    return postMessageCommand(oPsm,sCommand,sPram,oAttach);

}

int TicketBusiness::dealTicketMain(const PsmProtocol * oPsm,map<string, string> &declare, EFileElement &Obj,const PsmAttach * oAttach)
{
    return  -1;
}

int TicketBusiness::dealTicketDetail(const PsmProtocol * oPsm,map<string, string> &declare, EFileElement &Obj,const PsmAttach * oAttach)
{
    return  -1;
}
//
int TicketBusiness::createHead(int type, EFileElement &Head,uint32 &functionCode,const PsmAttach * oAttach)
{
    vector<map<string,string> > vObj = Head.getRec();
    if(vObj.size()!=1)return  GCL_ERR_FAIL;

    map<string,string> rec = vObj.at(0);

    //获取功能码
    string sFunctonCode=CxContainer::value(rec,CS_EFile_Head_FunctionCode);
    uint32 fc = CxString::toUint32("0x"+sFunctonCode);

    functionCode = fc;

    //返回功能码
    string sRetfc= CxString::format("%05X",(fc&0x0000ffff)|type);
    rec[CS_EFile_Head_FunctionCode] = sRetfc;

    //源 目的定义
    _recvSouceID =  CxContainer::value(rec,CS_EFile_Head_Source);

    rec[CS_EFile_Head_Target] = CxContainer::value(rec,CS_EFile_Head_Source);
    rec[CS_EFile_Head_Source] = "01801001";


    //重新生成记录
    vObj.clear();
    vObj.push_back(rec);

    Head.setRec(vObj);

    return GCL_SUCCED;

}

/////////////////////////////////////////////////////

int TicketBusiness::createEFile()
{
    _vEFileEle.clear();
    EFileElement obj;
    //   obj.setName();
}

int TicketBusiness::respTickerReq(const PsmProtocol *oPsm, map<string, string> &declare, EFileElement &Head,const PsmAttach * oAttach)
{
    uint32 functionCode = 0;
    EFileElement obj;
    _vEFileEle.clear();
    //生成头部
    createHead(CI_EFILE_DATA_RESP,Head,functionCode,oAttach);

    _vEFileEle.push_back(Head);

    string sKey;
    string sql;
    //主表
    obj.setName("TicketMain","TicketMain");
    sKey = "TicketMain";

    if(!CxContainer::contain(_mapTableToFiledName,sKey)) return GCL_ERR_FAIL;


    obj.setType(EFileElement::ee_type_row);
    obj.seField(CxContainer::value(_mapTableToFiledName,sKey));
    sql = "SELECT F_ID,F_Name,F_Desc,F_Type,F_Status,F_ModeOp,F_MaxTimeOut,F_UserCreate,F_UserMon,F_UserAud,F_UserOp,F_DtCreate,F_DtStartPlan,F_DtEndPlan,F_DtStartOp,F_DtEndOp,F_Ext FROM TicketMain";
    obj.loadTable2(sql);

    _vEFileEle.push_back(obj);

    //子表
    obj.clear();
    obj.setType(EFileElement::ee_type_row);
    obj.setName("TicketDetail","TicketDetail");
    sKey = "TicketDetail";
    if(!CxContainer::contain(_mapTableToFiledName,sKey)) return GCL_ERR_FAIL;

    obj.seField(CxContainer::value(_mapTableToFiledName,sKey));
    sql = "SELECT F_ID,F_PID,F_Order,F_TypeOp,F_Pram0,F_Pram1,F_DTStartOp,F_DTEndOp,F_ResOp,F_Status,F_ErrInfo,F_Ext FROM TicketDetail ORDER BY F_PID,F_ORDER";
    obj.loadTable2(sql);
    _vEFileEle.push_back(obj);

    //生成E文件
    GString sToday = CxTime::toStringDay(CxTime::currentSepoch(),'-');
    GString sPath = CxFileSystem::mergeFilePath(GclConfig::tempPath(),sToday);
    CxFileSystem::createDirMultiLevel(sPath);
    GString fileName = CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0)+"_Send_"+ _currFunctionCode+".efl";
    GString sFilePath = CxFileSystem::mergeFilePath(sPath ,fileName);

    ExplainEfile::saveToFile(sFilePath,declare,_vEFileEle);

//    vector<string> sLines = ExplainEfile::toEfileBuffer(declare,_vEFileEle);
//    string sPram = CxString::join(sLines,'\n');
    //返回功能码
    string sRetfc= CxString::format("%05X",(functionCode&0x0000ffff)|CI_EFILE_DATA_RESP);
    //发送E文件
    string sCommand1 = "send.efile.1="+sRetfc;
    int  iRemoteTerminalId = CxString::toInt32("0x"+_recvSouceID);

    vector<string> _sendFileTexts;

    CxFile::load(sFilePath,_sendFileTexts,3500);

    if(_sendFileTexts.size()>0)
    {
        for(size_t i=0;i<_sendFileTexts.size();++i)
        {
             string sCommand = sCommand1 + CxString::format(";count=%d;index=%d",_sendFileTexts.size(),i+1);
             postMessageCommand(oPsm,sCommand,_sendFileTexts.at(i),oAttach);
        }
    }

    return GCL_SUCCED;
}

int TicketBusiness::postMessageCommand(const PsmProtocol *oPsm, string &sCommand, string &sPram, const PsmAttach *oAttach)
{
    PsmProtocol *psm = (PsmProtocol *)oPsm;
    if(psm!=NULL)
    {
        PsmAttach *oDefault = (PsmAttach *)(psm->getDefaultAttach());
        if(oAttach!=NULL)
        {
            PsmAttach attach(0, 0, oDefault->sourceId, oAttach->sourceId, 0);
            psm->postMessageCommand(sCommand.data(), sPram.data(), sPram.length(),&attach);
            outBusinessPrompt()<<"send:"<<sCommand<<sPram;
            return  GCL_SUCCED;
        }
    }
    return  GCL_ERR_FAIL;
}


/////////////////////////////////////////////////////
//初始化
bool TicketBusiness::init()
{
    if(loadTemplateTable())
    {
        ExplainEfile::setFieldTemplate(_mapTableToFiledName);
        return true;
    }

    return false;
}
//加载表定义模板
bool TicketBusiness::loadTemplateTable()
{
    CxDatabase::defaultDb()->openDatabase(CxFileSystem::mergeFilePath(GclConfig::databasePath(), "rtdb.db"));

    string  sql = CxString::format("select f_name from t_cmm_table");
    vector<map<string, string> > vRecs = CxDatabase::defaultDb()->queryToMapVector(sql);

    outBusinessPrompt()<<"TicketBusiness::loadTemplateTable:"<<vRecs.size();

    for(size_t i=0;i<vRecs.size();++i)
    {
        map<string,string> rec = vRecs.at(i);
        string key = "F_Name";
        if(CxContainer::contain(rec,key))
        {
            string sTable = CxContainer::value(rec,key);
            //字段信息
//            vector<GCCommon::GCCommonObj> vFieldObj;
//            if(loadTemplateField(vFieldObj,sTable))
//            {
//                _mapTableToFiled[sTable] = vFieldObj;
//            }
            //字段名
            vector<string> vFieldName;
            if(loadTemplateField(vFieldName,sTable))
            {
                _mapTableToFiledName[sTable] = vFieldName;
            }
        }
    }
    return true;
}
//加载字段定义模板
bool TicketBusiness::loadTemplateField(vector<GCCommon::GCCommonObj> &vField,string &sName)
{
    string  sql = CxString::format("select f_name,f_datatype0 from t_cmm_field where f_tableparent = '%s' order by f_order",sName.data());
    vector<map<string, string> > vRecs = CxDatabase::defaultDb()->queryToMapVector(sql);

    outBusinessPrompt()<<"TicketBusiness::loadTemplateFieldObj:"<<vRecs.size();


    vField.clear();
    for(size_t i=0;i<vRecs.size();++i)
    {
        map<string, string> rec = vRecs.at(i);
        GCCommon::GCCommonObj obj;


        string key = "F_Name";
        obj.setName(CxContainer::value(rec,key));

        key = "F_DataType0";
        uint32 code = CxString::toUint32(CxContainer::value(rec,key));
        obj.setCode(code);

        vField.push_back(obj);
    }
    return true;
}

bool TicketBusiness::loadTemplateField(vector<string> &vField, string &sName)
{
    string  sql = CxString::format("select f_name from t_cmm_field where f_tableparent = '%s' order by f_order",sName.data());
    vector<map<string, string> > vRecs = CxDatabase::defaultDb()->queryToMapVector(sql);

    outBusinessPrompt()<<"TicketBusiness::loadTemplateFieldName:"<<vRecs.size();

    vField.clear();
    for(size_t i=0;i<vRecs.size();++i)
    {
        map<string, string> rec = vRecs.at(i);

        string key = "F_Name";
        vField.push_back(CxContainer::value(rec,key));
    }
    return true;
}

vector<int> TicketBusiness::getColumnTypesByClassName(GString &sName,vector<string> &vObj)
{
    string  sql = CxString::format("select f_name,f_datatype0 from t_cmm_field where f_tableparent = '%s' order by f_order",sName.data());
    vector<map<string, string> > vRec = CxDatabase::defaultDb()->queryToMapVector(sql);

    vector<int> v;
    v.clear();
    //大小是否相等
    if(vRec.size()!= vObj.size()) return v;

    string field;
    for(size_t i=0;i<vObj.size();++i)
    {
        field = vObj.at(i);
        bool bFind = false;
        for(size_t j=0;j<vRec.size();++j)
        {
            map<string, string> rec = vRec.at(j);
            string key = "F_Name";
            string name = CxContainer::value(rec,key);
            key = "F_DataType0";
            string datatype = CxContainer::value(rec,key);
            if(name == field)
            {
                v.push_back(CxString::toInt32(datatype));
                bFind = true;
                break;
            }
        }
        if(!bFind)v.push_back(0);
    }
    return v;
}

