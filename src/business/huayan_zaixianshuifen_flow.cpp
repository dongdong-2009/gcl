#include "huayan_zaixianshuifen_flow.h"


#include <vdi/terminal_exe.h>
#include <business/asm570_business.h>
#include <business/ruixiang_business.h>
#include <business/cics_business.h>


using namespace std;


HuayanZaixianshuifenFlow * fn_huayanZaixianshuifenFlow_init()
{
    TerminalExe::registAfterLoad(HuayanZaixianshuifenFlow::init);
    static HuayanZaixianshuifenFlow s_huayanZaixianshuifenFlow;
    return & s_huayanZaixianshuifenFlow;
}


static HuayanZaixianshuifenFlow * f_oHuayanZaixianshuifenFlow = fn_huayanZaixianshuifenFlow_init();

static RuixiangBusiness * f_oRuixiangBusiness = NULL;
static CicsBusiness * f_oCicsBusiness = NULL;
static Asm570Business * f_oAsm570Business = NULL;

std::string f_sRuixiangDeviceCode;
string f_sYPBH;
string f_sYPMM;
string f_sZDYM;


volatile int f_iProcessState = 0;
msepoch_t f_iProcessTime = 0;
int f_iAsm570State = 0;


void fn_timer_timeout_500(int iInterval)
{
    if (f_iProcessState == 11)
    {
        //*在线水分取样过程 004
        //5秒内变状态
        if (CxTime::milliSecondDifferToNow(f_iProcessTime) < 5000)
        {
            MonsbMeasure * oMonsb = MonsbManager::measure(f_oAsm570Business->workStateMeasureId());
            if (oMonsb)
            {
                int iState = oMonsb->value();
                if (iState == Asm570Business::WorkState_DiscAuto_Pre || iState == Asm570Business::WorkState_DiscAuto_Ing)
                {
                    //返回试样编码 格式：[command] [param:设备号\n样品编码]
                    f_oRuixiangBusiness->sendMessage("reply_sample", f_sRuixiangDeviceCode+"\n"+f_sYPMM);
                    f_iProcessState = 21;
                }
            }
        }
        else
        {
            f_iProcessState = 0;
        }
    }

    if (f_iProcessState == 21)
    {
        //*在线水分取样过程 006
        if (CxTime::milliSecondDifferToNow(f_iProcessTime) < 12000)
        {
            MonsbMeasure * oMonsb = MonsbManager::measure(f_oAsm570Business->workStateMeasureId());
            if (oMonsb)
            {
                int iState = oMonsb->value();
                if (iState == Asm570Business::WorkState_DiscAuto_Ed || iState == Asm570Business::WorkState_DiscAuto_Error || iState == Asm570Business::WorkState_Free)
                {
                    //返回试样编码 格式：[command] [param:设备号\n样品编码]
                    f_oRuixiangBusiness->sendMessage("reply_desample", f_sRuixiangDeviceCode+"\n"+f_sYPMM);
                    f_iProcessState = 0;
                }
            }
        }
        else
        {
            f_iProcessState = 0;
        }
    }
}

bool fn_isValidHuayanZaixianshuifenFlow()
{
    return f_oRuixiangBusiness && f_oCicsBusiness && f_oAsm570Business;
}


void HuayanZaixianshuifenFlow::ruixianReceivedCommandMessage(const std::string &sCommand, const char *pParam, int iParamLength)
{
    GM_INVALID_RETURE(fn_isValidHuayanZaixianshuifenFlow());

    //*在线水分取样过程 001
    if (sCommand == "request_sample" )
    {
        string sParam(sParam, iParamLength);
        f_sRuixiangDeviceCode = CxString::token(sParam, '\n');
        map<string, string> sHeads;
        f_oCicsBusiness->createHead(sHeads);
        f_oCicsBusiness->sendSingleTemplateMessage("00210", sHeads);
    }

    //*在线水分取样过程 005
    else if (sCommand == "request_desample" )
    {
        //确认弃样
        f_oAsm570Business->sendMessage("post.asm570.confirm.1");
    }
}

void HuayanZaixianshuifenFlow::cicsReceivedXmlContent(const string &sMsgNo, const std::map<string, string> &major, const std::map<string, std::map<string, string> > &details)
{
    GM_INVALID_RETURE(fn_isValidHuayanZaixianshuifenFlow());

    //*在线水分取样过程 002
    if (sMsgNo == "00211")
    {
        f_sYPBH = CxContainer::value(major, string("YPBH"));  //<!—样瓶RFID码-->
        f_sYPMM = CxContainer::value(major, string("YPMM"));  //<!—样瓶明码-->
        f_sZDYM = CxContainer::value(major, string("ZDYM"));  //<!—自定义码-->

        //注意要配置 WorkStateMeasureId
        MonsbMeasure * oMonsb = MonsbManager::measure(f_oAsm570Business->workStateMeasureId());
        if (oMonsb && oMonsb->value() == Asm570Business::WorkState_Free)
        {
            //*在线水分取样过程 003
            //请求弃样
            f_oAsm570Business->reqDiscard(f_sYPBH, f_sYPMM, f_sZDYM);
            f_iProcessTime = CxTime::currentSystemTime();
            f_iProcessState = 11;
        }
    }
}

void HuayanZaixianshuifenFlow::asm570ReceivedCommandMessage(const string &sCommand, const string & sParam)
{
    if(sCommand == "send.yx.1")
    {
        std::map<string, string> yxes =  CxString::splitToMap(sParam,'=','\n');
        for (map<string, string>::const_iterator it = yxes.begin(); it != yxes.end(); ++it)
        {
            string sMeasureId = it->first;
            string sState = it->second;
            f_iAsm570State = CxString::toInt32(sState);
            break;
        }
    }
}

void HuayanZaixianshuifenFlow::init()
{
    const std::vector<TerminalBase*>* oTerminals = TerminalManager::terminals();
    for (size_t i = 0; i < oTerminals->size(); ++i)
    {
        TerminalBase * oTerminal = oTerminals->at(i);
        TBIBase * oBusiness = oTerminal->business();
        if (oBusiness && oBusiness->typeSimpleName() == CS_BusinessTypeSimpleName_Ruixiang)
        {
            f_oRuixiangBusiness = (RuixiangBusiness*)oBusiness;
        }
        else if (oBusiness && oBusiness->typeSimpleName() == CS_BusinessTypeSimpleName_Cics)
        {
            f_oCicsBusiness = (CicsBusiness*)oBusiness;
        }
        else if (oBusiness && oBusiness->typeSimpleName() == CS_BusinessTypeSimpleName_Asm570)
        {
            f_oAsm570Business = (Asm570Business*)oBusiness;
        }
    }

    CxTimerManager::startTimer(fn_timer_timeout_500, 500);
}

