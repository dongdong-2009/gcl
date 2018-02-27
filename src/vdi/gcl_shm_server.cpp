
#include "gcl_shm_server.h"
#include "gcl_measure_server.h"
#include <vdi/monsb_sharememory.h>
#include <vdi/ycadd_sharememory.h>
#include <vdi/straw_sharememory.h>
#include <vdi/measure_sharememory.h>

using namespace std;

GCLShareMemServer::GCLShareMemServer()
{
}
//根据url 获取MeasureID
int GCLShareMemServer::getIDbyUrl(string &sUrl)
{
    return GCLMeasureServer::getIDbyUrl(sUrl);
}

//根据ID获取值
bool GCLShareMemServer::getValueByID(int id, gcl_stu_lua_value &value)
{
    bool bRet = false;

    value.clear();

    if((id>=ci_minMeasureIdMonsb)&&(id<=ci_maxMeasureIdMonsb)) //YX
    {
        MonsbShareMemory::MeasureData *oMeasure =  MonsbShareMemory::memoryManager()->measureData(id);
        if(oMeasure!=NULL)
        {
            value.q      = oMeasure->quality;
            value.id     = id;
            value.type   = GCL_VT_YX;
            value.len    = sizeof(int);
            value.iValue = oMeasure->value;
//            value.t      = oMeasure->refreshTime;
            value.t      = oMeasure->changedTime;
            value.src    = oMeasure->changedSourceId;
            bRet = true;
        }
    }
    else if((id>=ci_minMeasureIdYcadd)&&(id<=ci_maxMeasureIdYcadd))//YC
    {
        YcaddShareMemory::MeasureData *oMeasure = YcaddShareMemory::memoryManager()->measureData(id);
        if(oMeasure!=NULL)
        {
            value.q      = oMeasure->quality;
            value.id     = id;
            value.type   = GCL_VT_YC;
            value.len    = sizeof(double);
            value.dValue = oMeasure->value;
//            value.t      = oMeasure->refreshTime;
            value.t      = oMeasure->changedTime;
            value.src    = oMeasure->changedSourceId;
            bRet = true;
        }
    }
    else if((id>=ci_minMeasureIdStraw)&&(id<=ci_maxMeasureIdStraw))//straw
    {
        StrawShareMemory::MeasureData *oMeasure =  StrawShareMemory::memoryManager()->measureData(id);
        if(oMeasure!=NULL)
        {
            StrawValue sValue = oMeasure->value;
            value.q      = oMeasure->quality;
            value.id     = id;
            value.type   = GCL_VT_STR;
            value.sValue = string(sValue.value);
            value.len    = value.sValue.length();
//            value.t      = oMeasure->refreshTime;
            value.t      = oMeasure->changedTime;
            value.src    = oMeasure->changedSourceId;
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
bool GCLShareMemServer::getValueByUrl(string &url, gcl_stu_lua_value &value)
{
    if(url.length()<1) return false;

    bool bRet = false;

    int measureID = getIDbyUrl(url);

    //测点有效
    if(measureID >0 )
    {
        bRet = getValueByID(measureID,value);
    }
    return bRet;
}

//根据ID 设定值
//跟新到本地表，通过GA发送到BUS更新共享内存
void GCLShareMemServer::setValueByID(gcl_stu_lua_value &value)
{
    GCLMeasureServer::setValueByID(value);
}

//根据url 设定值
void GCLShareMemServer::setValueByUrl(string &sUrl, gcl_stu_lua_value &value)
{
    value.id = getIDbyUrl(sUrl);
    setValueByID(value);
}
