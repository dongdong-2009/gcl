
#include "gcl_data_server.h"
#include <vdi/protocol_config.h>
#include <vdi/explain_xml.h>
#include <vdi/monsb_measure.h>
#include <vdi/ycadd_measure.h>
#include <vdi/straw_measure.h>
#include <script/gcl_script_vxd.h>
#include <vdi/common_service.h>


using namespace std;

std::map<std::string,GCCommon::GCComObjVector* > f_mapField; //字段映
std::map<std::string,GCCommon::GCComObjVector* > f_mapDataSouce;//数据源

std::map<std::string,GCMap_String_ObjVec*> f_mapRTDB;//RTDB映射表

std::map<uint32,uint32> f_mapChan;//链路映射

GCCommon::GCDataElement f_oDataEelementDt;//数据单元

GCCommon::GCDataEmQueue  f_qMsg;//消息队列

msepoch     f_tmStart;

char*  f_pBuffer;
int    f_bufLen;

static GCLDataServer * fn_oGCLDataServer()
{
    CxApplication::registStartFunction(GCLDataServer::start);
    CxApplication::registStopFunction(GCLDataServer::stop);
    GeneralAccess::registerMessageCommandDeal(GCLDataServer::dealMessage);
    GeneralAccess::registerFileWriteDeal(GCLDataServer::dealFileWrite);

    static GCLDataServer m;
    return &m;
}

static GCLDataServer * f_oGCLDataServer = fn_oGCLDataServer();

//初始化
void GCLDataServer::start()
{
    //加载CICS设置
    loadCICSConfig();
    //加载RTDB设置
    loadRTDBConfig();
    //加载CICS模板
    int n = XmlMsObjectTemplateManager::loadMessageTempltes(GclConfig::dataPath());

    cxDebug()<<"loadMessageTempltes="<<n<<cxEndLine;

    f_tmStart = CxTime::currentSystemTime();

    f_bufLen = f_oDataEelementDt.maxSize();
    f_pBuffer = new char[f_bufLen];
    memset(f_pBuffer,0,f_bufLen);

    f_qMsg.clear();

    //启动工作线程
    CxTimerManager::startTimer(GCLDataServer::timer, GCL_PERIOD_RTDB);
    //注册脚本代理
    ScriptProxy::registerNotify(f_oGCLDataServer);
}
//停止
void GCLDataServer::stop()
{
    f_qMsg.clear();

    CxTimerManager::stopTimer(GCLDataServer::timer);

    ScriptProxy::unregisterNotify(f_oGCLDataServer);
}

//
void GCLDataServer::timer(int interval)
{
    work();
}
//周期任务
void GCLDataServer::doMQ()
{
    if(f_qMsg.size()>0)
    {
        GCCommon::GCDataElement dt;
        int num =  f_qMsg.size();
        for(int i=0;i<num;i++)
        {
            if(f_qMsg.pop(dt))
            {
                dealMsg(dt);
            }
        }
    }
}

//工作任务
void GCLDataServer::work()
{ 
    doMQ();
}
//脚本消息处理
void GCLDataServer::sendMessage(string sCommand, string sParamter)
{
    if(sCommand == "send.command.1")
    {
        deal_command_1(sParamter);
    }
    else if(sCommand == "send.command.2")
    {
        deal_command_2(sParamter);
    }
    else if(sCommand == "send.sql.1")
    {
        execSql(sParamter);
    }
    else{
        cxDebug()<<CxTime::currentSepochString()<<"error:no support command"<<sCommand<<sParamter<<cxEndLine;
    }
}

void GCLDataServer::dealMsg(GCCommon::GCDataElement &dt)
{
    switch(dt.type())
    {
    case GCL_CMD_SYS_SC_W://字符串写
    {
        std::map<GString,GString> map1 = CxString::splitToMap(GString(dt.buffer(), dt.dataLenth()), '=', '\n');

        for(std::map<GString,GString>::const_iterator it = map1.begin(); it != map1.end(); ++it)
        {
            //更新本地值
            string sKey = it->first;
            int id =  getIDbyUrl(sKey);
            if((id>=ci_minMeasureIdStraw)&&(id<=ci_maxMeasureIdStraw))//straw
            {
                StrawMeasure *oMeasure = StrawManager::measure(id);
                if(oMeasure!=NULL)
                {
                    StrawMeasure::beginSetValues(0,0,0);
                    oMeasure->setValue(it->second);
                    StrawMeasure::endSetValues();
                }
            }
            //总
            CommonService::addGlobalValue(it->first,it->second);
            cxDebug()<<it->first<<it->second<<cxEndLine;
        }
    }
        break;
    case GCL_CMD_SYS_MEAS_W_S2://变量写
    {
        int id = dt.pram();
        uint16 iValue = 0;
        if(CxBuffer::fromBuf((uchar*)dt.buffer(),dt.dataLenth(),iValue))
        {
            gcl_stu_lua_value mValue;
            //             memset(&mValue,0,sizeof(gcl_stu_lua_value));
            //获取值
            getValueByID(id,mValue);

            bool bRet =true;
            mValue.q = GCL_Q_GOOD;
            switch(mValue.type)
            {
            case GCL_VT_YX:
                mValue.iValue = iValue;
                break;
            case GCL_VT_YC:
                mValue.dValue = iValue;
                break;
            case GCL_VT_STR:
                mValue.sValue = CxString::format("%d",iValue);
                break;
            default:
                bRet = false;
                break;
            }
            if(bRet)
            {
                setValueByID(mValue);//设定值
            }
        }
    }
        break;
    case GCL_CMD_SYS_INFO: //系统信息
    {
        //        if(f_pDBThread!=NULL) f_pDBThread->saveInfoToDB(dt);
    }
        break;
    case GCL_CMD_SYS_SQL://SQL 语句
    {
        execSql(string(dt.buffer(),dt.dataLenth()));
    }
        break;
    case GCL_CMD_SYS_XML_CICS://CICS XML File
    {
        std::map<string,string> map1 = CxString::splitToMap(string(dt.buffer(), dt.dataLenth()), '=', '\n');

        string sKey = "fc";
        string fc = CxContainer::value(map1,sKey);
        fc = CxString::trim(fc);
        if(fc.empty()) break;

        sKey = "file";
        string fileName = CxContainer::value(map1,sKey);
        fileName = CxString::trim(fileName);
        if(fileName.empty()) break;
        //执行脚本
        GclLuaXml::dealFile(fc.data(),fileName.data());
    }
        break;
    case GCL_CMD_SYS_EFILE://E File
    {
        std::map<string,string> map1 = CxString::splitToMap(string(dt.buffer(), dt.dataLenth()), '=', '\n');

        string sKey = "fc";
        string fc = CxContainer::value(map1,sKey);
        fc = CxString::trim(fc);
        if(fc.empty()) break;

        sKey = "file";
        string fileName = CxContainer::value(map1,sKey);
        fileName = CxString::trim(fileName);
        if(fileName.empty()) break;
        //执行脚本
        GclLuaEFile::dealFile(fc.data(),fileName.data());
    }
        break;
    case GCL_CMD_SYS_MESSAGE://消息处理
    {
        std::map<string,string> map1 = CxString::splitToMap(string(dt.buffer(), dt.dataLenth()), '=', '\n');

        string sKey = "command";
        string cmd = CxContainer::value(map1,sKey);
        cmd = CxString::trim(cmd);
        if(cmd.empty()) break;

        sKey = "parameter";
        string pram = CxContainer::value(map1,sKey);
        pram = CxString::trim(pram);
        if(pram.empty()) break;
        //执行脚本
        string sCommand = CxString::format("command=%s",cmd.data());
        string sParam=CxString::format("parameter=%s",pram.c_str());
        ScriptProxy::postMessage(sCommand,sParam,CxApplication::applicationFileName());

    }
        break;

    case GCL_CMD_SYS_CMD: //命令
    {
        std::map<string,string> map1 = CxString::splitToMap(string(dt.buffer(), dt.dataLenth()), '=', '\n');
        string sKey = "@cics";
        string sValue = CxContainer::value(map1,sKey);
        sValue = CxString::trim(sValue);
        if(!sValue.empty())
        {
            dealCICS(sValue);
        }
    }
        break;
    default: //转发
    {
        //命令字
        string sCmd = CxString::format("0x%04X",dt.type());

        if(!CxContainer::contain(f_mapRTDB,sCmd))
        {
            cxDebug()<<"warning:command is nor exist." <<sCmd<<cxEndLine;
            break;
        }

        GCMap_String_ObjVec* obj = CxContainer::value(f_mapRTDB,sCmd,(GCMap_String_ObjVec*)NULL);

        if(obj==NULL)break;
        //点号
        string sMeasue = CxString::format("0x%08X",dt.pram());
        if(!CxContainer::contain(obj,sMeasue))
        {
            cxDebug()<<"warning:measure is nor exist." <<sCmd<<sMeasue<<cxEndLine;
            break;
        }

        GCCommon::GCComObjVector* o =CxContainer::value(obj,sMeasue,(GCCommon::GCComObjVector*)NULL);

        if(o==NULL) break;
        //数据处理
        for(size_t i=0;i<o->size();++i)
        {
            deal_data(dt,o->at(i));
        }
    }
        break;
    }
}

void GCLDataServer::deal_data(GCCommon::GCDataElement &dt,GCCommon::GCCommonObj &obj)
{
    if(obj.id()==0) return;

    if(!CxContainer::contain(f_mapChan,obj.id())) return;

    uint32 iTargetId = CxContainer::value(f_mapChan,obj.id());

    bool bRet = false;
    //转发
    if(GeneralAccess::defaultAttach()->sourceId != iTargetId)
    {
        string sCommand;
        string sParam;

        if(obj.type()==0)//转发原命令
        {
            sCommand = CxString::format("command=0x%08x",dt.type());

            if(dt.toBuffer(f_pBuffer))
            {
                sParam = string(f_pBuffer,dt.size());
                bRet = true;
            }
        }
        else //组织新命令
        {
            byte type = (obj.type()>>24)&0xff;
            uint32 fc = obj.type()&0x00ffffff;
            switch(type)
            {
            case 0:
            {
                sCommand = CxString::format("command=0x%04x",fc);

                string s = CxString::toHexstring(dt.buffer(),dt.dataLenth(),false);

                sParam  = CxString::format("id=%d;code=%d;type=%d;ext=",obj.id(),obj.code(),fc)+s;
                bRet = true;

            }
                break;
            case 1: //16进制转换
            {
                sCommand = CxString::format("command=0x%04x",fc);
                //            vector<char> v = CxString::hexToData(obj.ext());
                sParam  = CxString::format("id=%d;code=%d;type=%d;ext=",obj.id(),obj.code(),fc)+obj.ext();
                bRet = true;
            }
                break;
            default:
                break;
            }
        }
        if(bRet)
        {
            cxDebug()<<sCommand<<sParam<<cxEndLine;
            PsmAttach o(0,0,GeneralAccess::defaultAttach()->sourceId,iTargetId,0);
            int iSend = GeneralAccess::postMessage(sCommand.c_str(),sParam.c_str(),sParam.length(),&o);
        }
        else
        {
            cxDebug()<<"warning:"<<sCommand<<sParam<<cxEndLine;
        }
    }
    else{ //本地应用
        if(obj.type()!=0)//转发原命令
        {
            byte type = (obj.type()>>24)&0xff;
            uint32 fc = obj.type()&0x00ffffff;
            switch(type)
            {
            case 0:
            {
                f_oDataEelementDt.setValue(obj.id(),fc,obj.code(),dt.dataLenth(),dt.buffer());
                f_qMsg.push(f_oDataEelementDt);
                bRet = true;
            }
                break;
            case 1: //16进制转换
            {
                if(obj.ext().size()>0)
                {
                    vector<char> v = CxString::hexToData(obj.ext());
                    f_oDataEelementDt.setValue(obj.id(),fc,obj.code(),v.size(),v.data());
                    f_qMsg.push(f_oDataEelementDt);
                    bRet = true;
                }
            }
                break;
            default:
                break;
            }
        }
    }
}

//命令处理
void GCLDataServer::deal_command_1(string sParam)
{
    if(sParam.empty()) return;
    std::map<GString,GString> mapValue = CxString::splitToMap(sParam,'=',CS_GCL_MARK_SPIT);

    uint32 id  = 0;
    uint32 cmd = 0;
    uint32 measureID  = 0;
    string sBuff="";

    string sKey;

    sKey = string("channel");
    if(CxContainer::contain(mapValue,sKey))
    {
        id = CxString::toUint32(CxContainer::value(mapValue,sKey));
    }

    sKey= string("command");
    if(CxContainer::contain(mapValue,sKey))
    {
        cmd = CxString::toUint32(CxContainer::value(mapValue,sKey));
    }

    sKey = string("measure");
    if(CxContainer::contain(mapValue,sKey))
    {
        measureID = CxString::toUint32(CxContainer::value(mapValue,sKey));
    }

    sKey = string("parameter");
    if(CxContainer::contain(mapValue,sKey))
    {
        sBuff = CxContainer::value(mapValue,sKey);
    }

    GString sValue="";
    std::map<string,string> map1 = CxString::splitToMap(sBuff,'|',',');
    sValue = CxString::join(map1,"=","\n");

    f_oDataEelementDt.setValue(id,cmd,measureID,sValue.length(),(char*)sValue.data());
    f_qMsg.push(f_oDataEelementDt);
    //    cxDebug()<<"push command:"<<id<<cmd<<measureID<<sValue<<cxEndLine;
}

//命令处理
void GCLDataServer::deal_command_2(string sParam)
{
    if(sParam.empty()) return;
    std::map<GString,GString> mapValue = CxString::splitToMap(sParam,'=',CS_GCL_MARK_SPIT);

    uint32 id  = 0;
    uint32 cmd = 0;
    uint32 measureID  = 0;
    string sBuff="";

    string sKey;

    sKey = string("channel");
    if(CxContainer::contain(mapValue,sKey))
    {
        id = CxString::toUint32(CxContainer::value(mapValue,sKey));
    }

    sKey= string("command");
    if(CxContainer::contain(mapValue,sKey))
    {
        cmd = CxString::toUint32(CxContainer::value(mapValue,sKey));
    }

    sKey = string("measure");
    if(CxContainer::contain(mapValue,sKey))
    {
        measureID = CxString::toUint32(CxContainer::value(mapValue,sKey));
    }

    sKey = string("parameter");
    if(CxContainer::contain(mapValue,sKey))
    {
        sBuff = CxContainer::value(mapValue,sKey);
        sBuff = CxString::trim(sBuff);
    }
    std::map<string,string> map1 = CxString::splitToMap(sBuff,'|',',');

    char cTmp[GCL_LEN_DT] = {0};
    string sValue;
    int pos = 0;
    for(std::map<string,string>::const_iterator it = map1.begin(); it != map1.end(); ++it)
    {
        sKey = it->first;
        sValue = it->second;
        if(sKey=="s2")
        {
            int s = CxString::fromString(sValue,ci_int_zero);
            short s1 = (short)s;
            memcpy(&cTmp[pos],&s1,sizeof(short));
            pos += sizeof(short);
        }
        else if(sKey == "f4")
        {
            float s = CxString::fromString(sValue, cf_float_zero);
            memcpy(&cTmp[pos],&s,sizeof(float));
            pos += sizeof(float);
        }
        else if(sKey == "i4")
        {
            int s = CxString::fromString(sValue, ci_int_zero);
            memcpy(&cTmp[pos],&s,sizeof(int));
            pos += sizeof(int);
        }
        else if(sKey == "str")
        {
            memcpy(&cTmp[pos],sValue.data(),sValue.length());
            pos += sValue.length();
        }
    }
    if(pos>0)
    {
        f_oDataEelementDt.setValue(id,cmd,measureID,pos,cTmp);
        f_qMsg.push(f_oDataEelementDt);
        //        cxDebug()<<"push command:"<<id<<cmd<<measureID<<sBuff<<cxEndLine;
    }
}

//处理CICS业务
void GCLDataServer::dealCICS(string fc)
{
    if(fc.length()<1) return;

    if(CxContainer::contain(f_mapDataSouce,fc))
    {
        if(CxContainer::contain(f_mapField,fc))
        {
            GCCommon::GCComObjVector*  pDS   = (GCCommon::GCComObjVector*)f_mapDataSouce[fc];
            GCCommon::GCComObjVector* pField = (GCCommon::GCComObjVector*)f_mapField[fc];

            if((pDS!=NULL)&&(pDS->size()>0) && (pField !=NULL))
            {
                std::vector<std::map<std::string,std::string> > vDataSet;
                vDataSet.clear();
                //数据获取
                if(pDS->size()==1)//单数据源
                {
                    GCCommon::GCCommonObj obj = pDS->at(0);
                    switch(obj.type())
                    {
                    case 1:
                        dealCICS_01(fc,pField,obj,vDataSet);
                        break;
                    case 2:
                        dealCICS_02(fc,pField,obj,vDataSet);
                        break;
                    case 3:
                        dealCICS_03(fc,pField,obj,vDataSet);
                        break;
                    default:
                        break;
                    }
                }
                else
                { //多数据源

                    for(int i=0;i<pDS->size();i++)
                    {
                        GCCommon::GCCommonObj obj = pDS->at(i);
                        int pos = obj.code()>>16;
                        int len = obj.code()&0xffff;

                        GCCommon::GCComObjVector oField;

                        if((pos>=0) && (len>0) && ((pos+len)<=pField->size()))
                        {
                            for(int j=pos;j<=(pos+len);j++)
                            {
                                oField.push_back(pField->at(j));
                            }
                            switch(obj.type())
                            {
                            case 1:
                                dealCICS_01(fc,&oField,obj,vDataSet);
                                break;
                            case 2:
                                dealCICS_02(fc,&oField,obj,vDataSet);
                                break;
                            case 3:
                                dealCICS_03(fc,&oField,obj,vDataSet);
                                break;
                            default:
                                break;
                            }
                        }
                    }
                }

                //文件生成
                string filename;
                string sPath = CxFileSystem::mergeFilePath(GclConfig::tempPath(),CxTime::toStringDay(CxTime::currentSepoch(),'-'));
                std::map<string, string> sHeads;
                createHead(sHeads);

                if(XmlMsObjectTemplateManager::createDetailTemplate(fc,sHeads,vDataSet,sPath,filename))
                {
                    string sCommand = CxString::format("command=send.file.1;fc=%s",fc.c_str());
                    string sParam=CxString::format("filepath=%s",filename.c_str());
                    ScriptProxy::postMessage(sCommand,sParam,CxApplication::applicationFileName());
                }
            }
        }
    }
}

//CICS 业务01,从数据库中读写
bool GCLDataServer::dealCICS_01(string fc,GCCommon::GCComObjVector* pField,GCCommon::GCCommonObj &obj, std::vector<std::map<std::string,std::string> > &vDataSet)
{
    std::vector<GCCommon::GCStringInfo> vPram;

    string sql = obj.prop();
    int pos;
    int len;
    string value;
    while(CxString::find(sql,"{$","$}",pos,len,value))
    {
        sql = sql.substr(0,pos);
        GCCommon::GCStringInfo o(value,pos,len);
        vPram.push_back(o);
    }

    for(int i=0;i<vPram.size();i++)
    {
        GCCommon::GCStringInfo o = vPram.at(i);
        GCCommon::GCDataElement *e = CommonService::getGlobalValue(o.value());
        cxDebug()<<"o.value="<<o.value()<<cxEndLine;
        if(e!=NULL)
        {
            GString szVal = GString(e->buffer(),e->dataLenth());
            sql = obj.prop();
            sql = sql.replace(o.pos(),o.len(),szVal);
            obj.setProp(sql);
        }
    }
    //获取SQL语句
    sql = obj.prop();

    cxDebug()<<"sql="<<sql<<cxEndLine;

    std::vector<std::vector<std::string> > rows;
    std::vector<std::string> fields;


    bool bRet = false;
    if(loadSql(sql,rows,&fields))
    {
        GCCommon::GCCommonObj obj;
        for(size_t i=0;i<rows.size();++i)
        {
            std::vector<std::string>  rec = rows.at(i);

            string sKey;
            std::map<std::string,std::string> map1;
            for(size_t j=0;j<rec.size();j++)
            {
                //字段名称，通过数据库返回
                //                if(j<fields.size())
                //                {
                //                    sKey = fields.at(j);
                //                }else{
                //                    sKey = CxString::format("%d",j);
                //                }
                //字段名称，外部指定
                if(j<pField->size())
                {
                    obj = pField->at(j);
                    sKey = obj.name();
                }else{
                    sKey = CxString::format("%d",j);
                }

                map1[sKey] = rec.at(j);
            }
            vDataSet.push_back(map1);
        }
        bRet = true;
    }
    return bRet;
}
//CICS 业务01,从内存库中读写
bool GCLDataServer::dealCICS_02(string fc,GCCommon::GCComObjVector* pField,GCCommon::GCCommonObj &obj, std::vector<std::map<std::string,std::string> > &vDataSet)
{
    std::vector<GString> vPram = CxString::split(obj.prop(),";");

    if(vPram.size()>pField->size()) return false;

    gcl_stu_lua_value value;

    std::map<std::string,std::string> mapValue;

    for(int i=0;i<vPram.size();i++)
    {
        GString o = vPram.at(i);

        int id = CxString::toUint32(o); //支持10进制和16进制两种
        getValueByID(id,value);

        GCCommon::GCCommonObj obj2 = pField->at(i);

        if(value.type == GCL_VT_YX)
        {
            mapValue[obj2.name()] = CxString::trim(CxString::format("%d",value.iValue),CxGlobal::spaceChar);
        }
        else if(value.type == GCL_VT_YC)
        {
            switch(obj2.level())
            {
            case 0:
                mapValue[obj2.name()] =  CxString::trim(CxString::format("%12.0f",value.dValue),CxGlobal::spaceChar);
                break;
            case 1:
                mapValue[obj2.name()] =  CxString::trim(CxString::format("%12.1f",value.dValue),CxGlobal::spaceChar);
                break;
            default:
                mapValue[obj2.name()] =  CxString::trim(CxString::format("%12.2f",value.dValue),CxGlobal::spaceChar);
                break;
            }
        }
        else if(value.type == GCL_VT_STR)
        {
            mapValue[obj2.name()] =  CxString::trim(value.sValue,CxGlobal::spaceChar);
        }
    }
    vDataSet.push_back(mapValue);

    return true;
}

bool GCLDataServer::dealCICS_03(string fc,GCCommon::GCComObjVector* pField,GCCommon::GCCommonObj &obj, std::vector<std::map<std::string,std::string> > &vDataSet)
{
    std::vector<GString> vPram = CxString::split(obj.prop(),";");

    if(vPram.size()>pField->size()) return false;


    gcl_stu_lua_value value;

    std::map<std::string,std::string> mapValue;

    for(int i=0;i<vPram.size();i++)
    {
        if(!getValueByUrl(vPram.at(i),value))continue;

        GCCommon::GCCommonObj obj2 = pField->at(i);

        if(value.type == GCL_VT_YX)
        {
            mapValue[obj2.name()] = CxString::trim(CxString::format("%d",value.iValue),CxGlobal::spaceChar);
        }
        else if(value.type == GCL_VT_YC)
        {
            switch(obj2.level())
            {
            case 0:
                mapValue[obj2.name()] =  CxString::trim(CxString::format("%12.0f",value.dValue),CxGlobal::spaceChar);
                break;
            case 1:
                mapValue[obj2.name()] =  CxString::trim(CxString::format("%12.1f",value.dValue),CxGlobal::spaceChar);
                break;
            default:
                mapValue[obj2.name()] =  CxString::trim(CxString::format("%12.2f",value.dValue),CxGlobal::spaceChar);
                break;
            }
        }
        else if(value.type == GCL_VT_STR)
        {
            mapValue[obj2.name()] =  CxString::trim(value.sValue,CxGlobal::spaceChar);
        }
    }

    vDataSet.push_back(mapValue);
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
bool GCLDataServer::execSql(string sSql)
{
    if (sSql.size()>0)
    {
        CxDatabase * oDb = CxDatabaseManager::getDbByIndex(1);
        if (! oDb)
            return FALSE;

        msepoch_t dtNow = CxTime::currentMsepoch();

        int iResult = oDb->execSql(sSql);

        return iResult > 0;
    }
}

bool GCLDataServer::loadSql(string sSql,std::vector<std::vector<std::string> > &rows,std::vector<std::string> *fields)
{
    if (sSql.size()>0)
    {
        CxDatabase * oDb = CxDatabaseManager::getDbByIndex(1);
        if (! oDb)
            return FALSE;

        int iResult = oDb->loadSql(sSql,rows,fields);

        return iResult >= 0 ? true : false;
    }
    return false;
}
void GCLDataServer::createHead(std::map<string, string> &sHeads)
{
    sHeads["MsgId"]  = CxString::format("%03d%s",4,CxTime::toString(CxTime::currentMsepoch(), (char)0, (char)0, (char)0).c_str());
    sHeads["MsgRef"] = "";
    sHeads["TransDate"] = CxTime::toString(CxTime::currentMsepoch(), '-', ' ', ':');
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
//加载CICS配置
void GCLDataServer::loadCICSConfig()
{
    TiXmlDocument doc;

    //获取配置文件名
    string sFile = CxFileSystem::mergeFilePath(GclConfig::configPath(),CxApplication::applicationTargetName())+".cics.xml";

    cxPrompt() << "load cics config file::" <<sFile<< cxEndLine;
    if(doc.LoadFile(sFile))
    {
        TiXmlHandle docHandle( &doc );

        TiXmlNode* node = docHandle.FirstChild("YGCT").FirstChild("CICS").FirstChild("FC").Node();

        if(node==NULL)
        {
            cxPrompt() << "FC node is NULL" << cxEndLine;
            return;
        }

        TiXmlElement *ele =  node->ToElement();
        for(ele;ele;ele = ele->NextSiblingElement())
        {
            GString szName = "";
            if(ele->QueryStringAttribute("name",&szName)==TIXML_SUCCESS)
            {

                GCCommon::GCComObjVector *pData1 = new GCCommon::GCComObjVector();
                if(loadCommonObjVec(pData1,ele->FirstChild("DataSource")))
                {
                    f_mapDataSouce[szName] = pData1;
                }

                GCCommon::GCComObjVector *pData2 = new GCCommon::GCComObjVector();
                if(loadCommonObjVec(pData2,ele->FirstChild("Field")))
                {
                    f_mapField[szName] = pData2;
                }
            }
        }
    }
}

//加载RTDB配置
void GCLDataServer::loadRTDBConfig()
{
    TiXmlDocument doc;

    string sFile = CxFileSystem::mergeFilePath(GclConfig::configPath(),CxApplication::applicationTargetName())+".rtdb.xml";

    cxDebug() << "load rtdb config file:" <<sFile<< cxEndLine;
    if(doc.LoadFile(sFile))
    {
        TiXmlHandle docHandle( &doc );

        //获取测点配置信息
        TiXmlNode* node = docHandle.FirstChild("YGCT").FirstChild("RTDB").FirstChild("FC").Node();
        loadMeasuerConfig(node);

        //获取链路配置信息
        node = docHandle.FirstChild("YGCT").FirstChild("CONFIG").FirstChild("FC").Node();
        loadChanConfig(node);
    }
}

void GCLDataServer::loadChanConfig(TiXmlNode *node)
{
    if(node==NULL) return;
    TiXmlElement *ele =  node->ToElement();
    for(ele;ele;ele = ele->NextSiblingElement())
    {
        GString szName = "";
        if(ele->QueryStringAttribute("name",&szName)==TIXML_SUCCESS)
        {
            uint32 nPrant = CxString::toUint32(szName);
            GCCommon::GCComObjVector v;
            if(loadCommonObjVec(&v,ele))
            {
                for(size_t i=0;i<v.size();++i)
                {
                    f_mapChan[v.at(i).id()] = nPrant;
                    cxDebug()<<"id="<<v.at(i).id()<<"name:"<<v.at(i).name()<<"pID=:"<<nPrant<<cxEndLine;
                }
            }
        }
    }
}

void GCLDataServer::loadMeasuerConfig(TiXmlNode *node)
{
    if(node==NULL) return;
    TiXmlElement *ele =  node->ToElement();
    for(ele;ele;ele = ele->NextSiblingElement())
    {
        GString szName = "";
        if(ele->QueryStringAttribute("name",&szName)==TIXML_SUCCESS)
        {
            GCMap_String_ObjVec *pMap = new GCMap_String_ObjVec();
            if(loadMeasureMap(pMap,ele))
            {
                f_mapRTDB[szName]=pMap;
            }
        }
    }
}
bool GCLDataServer::loadMeasureMap(GCMap_String_ObjVec *obj, TiXmlNode *node)
{
    if((obj==NULL) || (node==NULL)) return false;

    TiXmlNode* node1 = node->FirstChild("Measure");
    if(node1==NULL)return false;

    TiXmlElement *ele =  node1->ToElement();
    for(ele;ele;ele = ele->NextSiblingElement())
    {
        GString szName = "";
        if(ele->QueryStringAttribute("id",&szName)==TIXML_SUCCESS)
        {
            GCCommon::GCComObjVector *pData1 = new GCCommon::GCComObjVector();
            if(loadCommonObjVec(pData1,ele))
            {
                (*obj)[szName]=(pData1);
            }
        }
    }
    return true;

}

bool GCLDataServer::loadCommonObjVec(GCCommon::GCComObjVector *obj, TiXmlNode *node)
{
    if((obj==NULL) || (node==NULL)) return false;

    TiXmlNode *node1 = node->FirstChild("Prop");
    if(node1==NULL) return false;

    TiXmlElement *ele = node1->ToElement();
    for(ele;ele;ele = ele->NextSiblingElement())
    {
        GCCommon::GCCommonObj o;
        loadCommonObj(o,ele);
        obj->push_back(o);
    }
    return true;
}

bool GCLDataServer::loadCommonObj(GCCommon::GCCommonObj &obj, TiXmlElement *ele)
{
    if(ele==NULL) return false;

    obj.clear();

    GString szVal = std::string();

    int nVal = 0;
    if(ele->QueryIntAttribute("id",&nVal)==TIXML_SUCCESS)
    {
        obj.setId(nVal);
    }
    szVal = std::string();
    if(ele->QueryStringAttribute("name",&szVal)==TIXML_SUCCESS)
    {
        obj.setName(szVal);
    }
    szVal = std::string();
    if(ele->QueryStringAttribute("ext",&szVal)==TIXML_SUCCESS)
    {
        obj.setExt(szVal);
    }
    szVal = std::string();
    if(ele->QueryStringAttribute("desc",&szVal)==TIXML_SUCCESS)
    {
        obj.setDesc(szVal);
    }
    szVal = std::string();
    if(ele->QueryStringAttribute("code",&szVal)==TIXML_SUCCESS)
    {
        obj.setCode(CxString::toUint32(szVal));
    }
    szVal = std::string();
    if(ele->QueryStringAttribute("type",&szVal)==TIXML_SUCCESS)
    {
        obj.setType(CxString::toUint32(szVal));
    }
    szVal = std::string();
    if(ele->QueryStringAttribute("level",&szVal)==TIXML_SUCCESS)
    {
        obj.setLevel(CxString::toUint32(szVal));
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//GA消息处理
int GCLDataServer::dealMessage(const PsmExplainer *oPsm, const string &sCommand, const string &sParam, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    bool bRet = false;
    if (sCommand.find("send.yk") != string::npos || sCommand.find("send.yt") != string::npos || sCommand.find("send.ys") != string::npos )
    {
        std::map<string, string> vParams = CxString::splitToMap( sParam , '=', ';' );

        int measureID = CxString::toInt32(vParams[CS_EntryControlMeasure]); // 量ID
        int iMeasureType = g_fn_getMeasureType(measureID);

        if (iMeasureType == MeasureType_None) {
            cxDebug() << "dealPsmMeasureControl error : measure is invalid !!!";
            return -1;
        }

        int controlValue = CxContainer::valueTo(vParams, CS_EntryControlValue, int(-1));
        if (controlValue == -1) {
            cxDebug() << "dealPsmMeasureControl error : valud is invalid, shareMemory do not setControl. but push transfer list!!!";
            return -1;
        }
        cxDebug()<<sCommand<<CxString::format("0x%08X",measureID)<<controlValue<<cxEndLine;
        f_oDataEelementDt.setValue(0,GCL_CMD_SYS_YK,measureID,sizeof(int),(char*)&controlValue);
        f_qMsg.push(f_oDataEelementDt);
        bRet=true;
    }
    else if(sCommand.find("send.vtl") != string::npos)
    {
        f_oDataEelementDt.setValue(0,GCL_CMD_SYS_SC_W,0,sParam.length(),(char*)sParam.data());
        f_qMsg.push(f_oDataEelementDt);
        bRet=true;
    }
    else if(sCommand.find("send.command")!= string::npos)
    {
        f_oDataEelementDt.setValue(0,GCL_CMD_SYS_CMD,0,sParam.length(),(char*)sParam.data());
        f_qMsg.push(f_oDataEelementDt);
        bRet=true;
    }
    else{

        GString sKey = CxString::trim(sCommand);
        if(sKey.empty())sKey = "00000";

        //以下是转成文件发出去的
        int nRet = 0; //以数字字符串作为命令码 XML格式文件
        CxString::fromString(sKey,nRet);
        if(nRet>=0)
        {
            GString sFilePath = CxFileSystem::mergeFilePath( GclConfig::tempPath(),CxTime::toStringDay(CxTime::currentSepoch(),'-'));
            CxFileSystem::createDirMultiLevel(sFilePath);
            GString sFileName = CxFileSystem::mergeFilePath(sFilePath,CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0)+"_"+sKey+".xml");
            if(CxFile::save(sFileName, sParam))
            {
                GclLuaXml::dealFile(sKey.data(),sFileName.data());
                bRet=true;
            }
        }
    }
    return bRet;
}

int GCLDataServer::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    bool bRet = false;
    if(pathInfo.filePath().size()>0&& fileData.size()==0)
    {
        GString sFilePath = pathInfo.filePath();
        if(CxFileSystem::extractFileSuffixName(sFilePath)==".xml")
        {
            GclLuaXml::dealFile("0000",sFilePath.data());
            bRet = true;
        }

    }else{
        GString sPath = CxFileSystem::mergeFilePath( GclConfig::tempPath(),CxTime::toStringDay(CxTime::currentSepoch(),'-'));
        CxFileSystem::createDirMultiLevel(sPath);
        GString fileName = CxTime::toString(CxTime::currentSystemTime(),(char)0,(char)0,(char)0)+"_"+ pathInfo.fileName;
        GString sFilePath = CxFileSystem::mergeFilePath(sPath ,fileName );

        bool bSave = CxFile::save(sFilePath, fileData, "");

        if(bSave && CxFileSystem::extractFileSuffixName(sFilePath)==".xml")
        {
            GclLuaXml::dealFile("0000",sFilePath.data());
            bRet = true;
        }
    }
    return bRet;
}
////////////////////////////////////////////////////////////////////////////////////////////
//脚本消息处理
void GCLDataServer::received_message_command(const string &sCommand, const string &sParam, const string &sSender, const ScriptProxyAttach *oAttach)
{
    //    cxDebug()<<"received_message_command:"<<sCommand<<sParam<<cxEndLine;
    sendMessage(sCommand,sParam);
}
//收到文件
void GCLDataServer::received_file_data(const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const string &sSender, const ScriptProxyAttach *oAttach)
{
    cxDebug()<<"received_file_data:"<<pathInfo.fileName<<cxEndLine;
}
//收到文件
void GCLDataServer::received_file(const string &sFilePath, const string &sSender, const ScriptProxyAttach *oAttach)
{
    cxDebug()<<"received_file:"<<sFilePath<<cxEndLine;

}
