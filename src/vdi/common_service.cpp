#include "common_service.h"

using namespace std;

bool    f_isStart; //启动标志
msepoch f_tmStart;
int     f_debugLevel;

int     f_livePeriod = 3000; //心跳周期
bool    f_isSendLive;//心跳发送标志

char    f_sendBuf[GCL_LEN_DT];
int     f_sendLen;

std::string f_remoteIp;
int     f_remotePort;

std::string f_localIp;
int     f_localPort;

std::map<std::string,GCCommon::GCDataElement*>  f_mapGlobalValue;

static CxMutex  m_oMutexCS;

static CxChannelBase * f_oHeartChannel = NULL;

CommonService::CommonService()
{
}

void CommonService::init()
{
    f_isStart = false;
    f_tmStart = CxTime::currentSepoch();
    f_debugLevel = 0xFF;
    f_isSendLive = false;

    f_livePeriod=3000;

    memset(f_sendBuf,0,GCL_LEN_DT);
    uint32 lead = GCL_MSG_FLAG_LEAD;
    memcpy(f_sendBuf,&lead,sizeof(uint32));
    f_sendLen = sizeof(uint32);
    //设置心跳信息
    setLiveInfo();

    CxApplication::registStartFunction(CommonService::start);
    CxApplication::registStopFunction(CommonService::stop);
}

void CommonService::start()
{
    if(!f_isStart)
    {
        CxTimerManager::startTimer(CommonService::task60s, 60000);
        CxTimerManager::startTimer(CommonService::taskLive, f_livePeriod);
        f_isStart = true;
    }
}

void CommonService::stop()
{
    if(f_isStart)
    {
        CxTimerManager::stopTimer(CommonService::task60s);
        CxTimerManager::stopTimer(CommonService::taskLive);
        f_isStart = false;
    }
}
//分钟任务
void CommonService::runLog()
{
    int runMinute = (CxTime::currentSepoch()-f_tmStart)/60;
    if(runMinute>0)
    {
        std::string szTmp = CxString::format("_[%04d-%02d:%02d]",runMinute/(24*60),(runMinute%(24*60))/60,(runMinute%(24*60))%60);
        //#ifdef GMAC_CONSOLE
        std::printf("%s:run time%s\n",CxTime::currentSepochString().c_str(),szTmp.c_str());
        //#endif
        if((runMinute%60)<1)
        {
            std::string szInfo = CxTime::toString(CxTime::currentSepoch())+":run time"+szTmp;
//            LogManager::WriteLogS(GCL_FILE_DEFAULT,szInfo);
        }
    }
}
//发送心跳包到看守进程
int CommonService::sendLiveToDamon()
{
    if(f_isSendLive && f_sendLen>0 && f_remotePort>0)
    {
        if (f_oHeartChannel)
        {
            return f_oHeartChannel->sendData(f_sendBuf, f_sendLen);
        }
    }
    return 0;
}


void CommonService::setLiveInfo(std::string sLocalIp,int nLocolPort,std::string sRemoteIp,int nRemotePort)
{
    memset(f_sendBuf,0,GCL_LEN_DT);
    uint32 lead = GCL_MSG_FLAG_LEAD;
    memcpy(f_sendBuf,&lead,sizeof(uint32));
    f_sendLen = sizeof(uint32);

    f_oHeartChannel  = CxChannelManager::createChannel("Udp");
    if (f_oHeartChannel)
    {
        std::map<std::string,std::string> map1;
        map1[CS_EntryLocalIpAddress]=sLocalIp;
        map1[CS_EntryLocalPort]=CxString::format("%d",nLocolPort);
        map1[CS_EntryRemoteIpAddress]=sRemoteIp;
        map1[CS_EntryRemotePort]=CxString::format("%d",nRemotePort);

        CxGetSkvMap skv(map1);
        f_oHeartChannel->fromContext(skv);
        f_oHeartChannel->setAutoOpenInterval(60000);
        f_oHeartChannel->open();
     }

    f_isSendLive = true;
}

void CommonService::setLiveInfo()
{
    std::string sInfo = CxApplication::findArgument("liveConfig");
    sInfo = CxString::trim(sInfo,CxGlobal::spaceChar);
    cxPrompt()<<"liveInfo:"<<sInfo<<cxEndLine;
    if(sInfo.length()>0)
    {
        std::vector<std::string> v0 = CxString::split(sInfo,',');
        if(v0.size()>=4)
        {
            int pos = 0;

            f_localIp     = v0.at(pos++);
            f_localPort   = CxString::toUint32(v0.at(pos++));
            f_remoteIp    = v0.at(pos++);
            f_remotePort  = CxString::toUint32(v0.at(pos++));
        }

        if(!CxString::isValidIp(f_localIp))return;
        if(!CxString::isValidIp(f_remoteIp))return;

        if(v0.size()>=5)
        {
            std::string sBuf = v0.at(4);
            std::vector<uchar> v1 = CxString::fromHexstring(sBuf);
            memset(f_sendBuf,0,GCL_LEN_DT);
            memcpy(f_sendBuf,&v1.front()/* v1.data()*/,v1.size());
            f_sendLen = v1.size();
        }

        if(v0.size()>=6)
        {
            f_livePeriod     = CxString::toUint32(v0.at(5));
            if(f_livePeriod<3000)f_livePeriod=3000;
        }

        f_oHeartChannel  = CxChannelManager::createChannel("Udp");
        if (f_oHeartChannel)
        {
            std::map<std::string,std::string> map1;
            map1[CS_EntryLocalIpAddress]=f_localIp;
            map1[CS_EntryLocalPort]=CxString::format("%d",f_localPort);
            map1[CS_EntryRemoteIpAddress]=f_remoteIp;
            map1[CS_EntryRemotePort]=CxString::format("%d",f_remotePort);

            CxGetSkvMap skv(map1);
            f_oHeartChannel->fromContext(skv);
            f_oHeartChannel->setAutoOpenInterval(f_livePeriod*2);
            f_oHeartChannel->open();

         }
         f_isSendLive = true;
    }
}

int CommonService::getDebugLevel()
{
    return f_debugLevel;
}

void CommonService::setDebugLevel(int value)
{
   f_debugLevel = value;
}

void CommonService::addGlobalValue(std::string key, std::string value)
{
    if(!CxContainer::contain(f_mapGlobalValue,key))
    {
        GCCommon::GCDataElement *obj = new GCCommon::GCDataElement();
        obj->setType(GCL_VT_STRING);
        obj->setBuffer(value.length(),(char*)value.c_str());
        obj->setId(1);
        f_mapGlobalValue[key] = obj;
//        cxDebug()<<"new:"<<key<<value<<cxEndLine;
    }else{
        GCCommon::GCDataElement *obj = f_mapGlobalValue[key];
        obj->setType(GCL_VT_STRING);
        obj->setBuffer(value.length(),(char*)value.c_str());
        obj->setId(1);
//        cxDebug()<<"update:"<<key<<value<<cxEndLine;
    }
}

void CommonService::addGlobalValue(std::string key, GCCommon::GCDataElement *value)
{
    f_mapGlobalValue[key] = value;
}

GCCommon::GCDataElement *CommonService::getGlobalValue(std::string key)
{
    if(CxContainer::contain(f_mapGlobalValue,key))
    {
        return f_mapGlobalValue[key];
    }else
    {
        return NULL;
    }
}

void CommonService::task60s(int interval)
{
    runLog();
}

void CommonService::taskLive(int interval)
{
    sendLiveToDamon();
}
/////////////////////////////////////////////////////////////////////////
