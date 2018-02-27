#include "gcl_measure_server.h"

#include <vdi/protocol_config.h>
#include <vdi/explain_xml.h>
#include <vdi/monsb_measure.h>
#include <vdi/ycadd_measure.h>
#include <vdi/straw_measure.h>
#include <script/gcl_script.h>
#include <vdi/common_service.h>

using namespace std;

std::map<std::string,int > f_mMapUrlToID;

static GCLMeasureServer * fn_oGCLMeasureSvr()
{
    CxApplication::registStartFunction(GCLMeasureServer::start);
    CxApplication::registStopFunction(GCLMeasureServer::stop);
    static GCLMeasureServer m;
    return &m;
}

static GCLMeasureServer * f_oGCLMeasureSvr = fn_oGCLMeasureSvr();

GCLMeasureServer::GCLMeasureServer()
{
}
//初始化
void GCLMeasureServer::start()
{
    loadUrlToID();
}

//停止
void GCLMeasureServer::stop()
{

}
//加载UrlToID
void GCLMeasureServer::loadUrlToID()
{
    MonsbManager::getMapUrl(f_mMapUrlToID);
    YcaddManager::getMapUrl(f_mMapUrlToID);
    StrawManager::getMapUrl(f_mMapUrlToID);
}

//根据url 获取MeasureID
int GCLMeasureServer::getIDbyUrl(string &sUrl)
{
    if(sUrl.empty()) return -1;
    return CxContainer::value(f_mMapUrlToID,sUrl,ci_int_minus_one);
}

//根据ID获取值
bool GCLMeasureServer::getValueByID(int id, gcl_stu_lua_value &value)
{
    bool bRet = false;

    value.clear();


    if((id>=ci_minMeasureIdMonsb)&&(id<=ci_maxMeasureIdMonsb)) //YX
    {
        MonsbMeasure *oMeasure =  MonsbManager::measure(id);
        if(oMeasure!=NULL)
        {
            value.q      = oMeasure->quality();
            value.id     = id;
            value.type   = GCL_VT_YX;
            value.len    = sizeof(int);
            value.iValue = oMeasure->value();
//            value.t      = oMeasure->lastRefreshTime();
            value.t      = oMeasure->lastChangeTime();
            value.src      = oMeasure->lastChangeSourceId();
            bRet = true;
        }
    }
    else if((id>=ci_minMeasureIdYcadd)&&(id<=ci_maxMeasureIdYcadd))//YC
    {
        YcaddMeasure *oMeasure =  YcaddManager::measure(id);
        if(oMeasure!=NULL)
        {
            value.q      = oMeasure->quality();
            value.id     = id;
            value.type   = GCL_VT_YC;
            value.len    = sizeof(double);
            value.dValue = oMeasure->value();
//            value.t      = oMeasure->lastRefreshTime();
            value.t      = oMeasure->lastChangeTime();
            value.src      = oMeasure->lastChangeSourceId();
            bRet = true;
        }
    }
    else if((id>=ci_minMeasureIdStraw)&&(id<=ci_maxMeasureIdStraw))//straw
    {
        StrawMeasure *oMeasure = StrawManager::measure(id);
        if(oMeasure!=NULL)
        {
            GString sValue = oMeasure->value();
            sValue = CxString::trim(sValue,(char)0);

            value.q      = oMeasure->quality();
            value.id     = id;
            value.type   = GCL_VT_STR;
            value.len    = sValue.length();
            value.sValue = sValue;
//            value.t      = oMeasure->lastRefreshTime();
            value.t      = oMeasure->lastChangeTime();
            value.src      = oMeasure->lastChangeSourceId();
            bRet = true;
        }

    }
    else //无效数据
    {
        bRet = false;
    }
    return bRet;
}

//根据Url获取值
bool GCLMeasureServer::getValueByUrl(string &url, gcl_stu_lua_value &value)
{
    if(url.length()<1) return false;

    bool bRet = false;

    int measureID = getIDbyUrl(url);

    //测点有效
    if(measureID >0 )
    {
        bRet = getValueByID(measureID,value);
    }

    if(!bRet)
    {
        //总表中查,字符串变量
        GCCommon::GCDataElement *e = CommonService::getGlobalValue(url);
        if(e!=NULL)
        {
            value.id   = 0;
            value.type = GCL_VT_STR;
            value.len  = e->dataLenth();
            value.sValue = string(e->buffer(),e->dataLenth());
            bRet = true;
        }
    }
    return bRet;
}



//根据ID 设定值
void GCLMeasureServer::setValueByID(gcl_stu_lua_value &value)
{
    switch(value.type)
    {
    case GCL_VT_YX:
    {
        MonsbMeasure *oMeasure =  MonsbManager::measure(value.id);
        if(oMeasure!=NULL)
        {
            MonsbMeasure::beginSetValues(0, 0, 0);
            oMeasure->setValue(value.iValue,value.src);
            oMeasure->setQuality(value.q);            
            MonsbMeasure::endSetValues();
        }
    }
        break;
    case GCL_VT_YC:
    {
        YcaddMeasure *oMeasure =  YcaddManager::measure(value.id);
        if(oMeasure!=NULL)
        {
            YcaddMeasure::beginSetValues(0, 0, 0);
            oMeasure->setValue(value.dValue,value.src);
            oMeasure->setQuality(value.q);
            YcaddMeasure::endSetValues();
        }
    }
        break;
    case GCL_VT_STR:
    {
        StrawMeasure *oMeasure = StrawManager::measure(value.id);
        if(oMeasure!=NULL)
        {
            StrawMeasure::beginSetValues(0, 0, 0);
            oMeasure->setValue(value.sValue,value.src);
            oMeasure->setQuality(value.q);
            StrawMeasure::endSetValues();
        }
    }
        break;
    default:
        break;
    }
}

//根据url 设定值
void GCLMeasureServer::setValueByUrl(string &sUrl, gcl_stu_lua_value &value)
{
    value.id = getIDbyUrl(sUrl);
    if(value.id>0)
    {
        setValueByID(value);
    }else{
      string sVal = value.toString();
      CommonService::addGlobalValue(sUrl,sVal);
    }
}
