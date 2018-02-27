#include "plc_manager.h"


using namespace std;


bool f_cancelWait = false;


bool PlcManager::setYxCommand(const string &sCommand, int iValue,bool vfy)
{
    GCCommon::GCCommonObj obj;
    obj.setName(sCommand);
    obj.setCode(iValue);
    return PlcManager::setYxCommand(obj,vfy);
}

bool PlcManager::setYxCommand(GCCommon::GCCommonObj &obj, bool vfy)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    bool bRet = false;
    int iValue = 0;
    string sName = obj.name();
    if(vfy)
    {
        for(int j=0;j<PLC_RETRY_TIME;j++)
        {
            PlcManager::getPlcProtocol()->readAllYX();
            CxApplication::waiting(PLC_WATING_PERIOD);


            if(PlcManager::getPlcProtocol()->getYxCommand(sName, iValue))
            {
                if(iValue == (int)obj.code())
                {
                    bRet = true;
                    break;
                }
                else
                {
                    PlcManager::getPlcProtocol()->setYxCommand(sName, (int)obj.code());
                }
            }
            if(bRet) break;

        }
    }
    else //无需校验
    {
        bRet = PlcManager::getPlcProtocol()->setYxCommand(sName, (int)obj.code());
    }

    return bRet;
}

bool PlcManager::setYxCommand(GCCommon::GCComObjVector &vObj,bool vfy)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    if(vObj.size()<1) return false;

    bool bRet = false;
    int iValue = 0;
    for(int j=0;j<PLC_RETRY_TIME;j++)
    {
        bRet = true;

        PlcManager::getPlcProtocol()->readAllYX();
        CxApplication::waiting(PLC_WATING_PERIOD*(vObj.size()/PLC_WATING_TIME+1));

        for(int i=0;i<vObj.size();i++)
        {
            GCCommon::GCCommonObj obj = vObj.at(i);
            string sName = obj.name();
            if(PlcManager::getPlcProtocol()->getYxCommand(sName,iValue))
            {
                if(iValue != (int)obj.code())
                {
                    PlcManager::getPlcProtocol()->setYxCommand(sName, (int)obj.code());
                    bRet = false;
                }
            }
        }
        if(bRet) break;
    }

    return bRet;
}

bool PlcManager::setYcCommand(const string &sCommand, int iValue,bool vfy)
{
    GCCommon::GCCommonObj obj;
    obj.setName(sCommand);
    obj.setCode(iValue);
    return setYcCommand(obj,vfy);
}

bool PlcManager::setYcCommand(GCCommon::GCCommonObj &obj,bool vfy)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);

    bool bRet = false;
    int iValue = 0;

    std::string sName = obj.name();
    if(vfy)
    {
        for(int j=0;j<PLC_RETRY_TIME;j++)
        {
            PlcManager::getPlcProtocol()->readAllYC();
            CxApplication::waiting(PLC_WATING_PERIOD);

            if(PlcManager::getPlcProtocol()->getYcCommand(sName, iValue))
            {
                if(iValue == (int)obj.code())
                {
                    bRet = true;
                    break;
                }
                else
                {
                    PlcManager::getPlcProtocol()->setYcCommand(sName, (int)obj.code());
                }
            }
            if(bRet) break;
        }
    }
    else //无需校验
    {
        bRet = PlcManager::getPlcProtocol()->setYcCommand(sName, (int)obj.code());
    }

    return bRet;
}

bool PlcManager::setYcCommand(GCCommon::GCComObjVector &vObj,bool vfy)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    if(vObj.size()<1) return false;

    bool bRet = false;
    int iValue = 0;
    for(int j=0;j<PLC_RETRY_TIME;j++)
    {
        bRet = true;

        PlcManager::getPlcProtocol()->readAllYC();
        CxApplication::waiting(PLC_WATING_PERIOD*(vObj.size()/PLC_WATING_TIME+1));

        for(int i=0;i<vObj.size();i++)
        {
            GCCommon::GCCommonObj obj = vObj.at(i);
            std::string sName = obj.name();
            if(PlcManager::getPlcProtocol()->getYcCommand(sName, iValue))
            {
                if(iValue != (int)obj.code())
                {
                    PlcManager::getPlcProtocol()->setYcCommand(sName,(int)obj.code());
                    bRet = false;
                }
            }
        }
        if(bRet) break;
    }

    return bRet;
}


bool PlcManager::getYxCommand(const string &sCommand, int iValue,bool bReq)
{
    GCCommon::GCCommonObj obj;
    obj.setName(sCommand);
    obj.setCode(iValue);
    return getYxCommand(obj,bReq);
}

bool PlcManager::getYxCommand(GCCommon::GCCommonObj &obj, bool bReq)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    bool bRet = false;
    int iValue = 0;

    if(bReq)
    {
        PlcManager::getPlcProtocol()->readAllYX();
        CxApplication::waiting(PLC_WATING_PERIOD*2);
    }

    std::string sName = obj.name();
    if(PlcManager::getPlcProtocol()->getYxCommand(sName, iValue))
    {
        if(iValue == obj.code())
        {
            bRet = true;
        }
    }

    return bRet;
}


bool PlcManager::getYxCommand(GCCommon::GCComObjVector &vObj, int &index, bool bReq)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    if(vObj.size()<1) return false;

    bool bRet = false;
    int iValue = 0;

    if(bReq)
    {
        PlcManager::getPlcProtocol()->readAllYX();
        CxApplication::waiting(PLC_WATING_PERIOD*2);
    }

    for(int i=0;i<vObj.size();i++)
    {
        GCCommon::GCCommonObj obj = vObj.at(i);
        std::string sName = obj.name();
        if(PlcManager::getPlcProtocol()->getYxCommand(sName, iValue))
        {
            if(iValue == (int)obj.code())
            {
                index = i;
                bRet = true;
                break;
            }
        }
    }

    return bRet;
}

bool PlcManager::getYcCommand(const string &sCommand, int iValue,bool bReq)
{
    GCCommon::GCCommonObj obj;
    obj.setName(sCommand);
    obj.setCode(iValue);
    return getYcCommand(obj,bReq);
}

bool PlcManager::getYcCommand(GCCommon::GCCommonObj &obj, bool bReq)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    bool bRet = false;
    int iValue = 0;

    if(bReq)
    {
        PlcManager::getPlcProtocol()->readAllYC();
        CxApplication::waiting(PLC_WATING_PERIOD*2);
    }

    std::string sName = (obj.name());
    if(PlcManager::getPlcProtocol()->getYcCommand(sName, iValue))
    {
        if(iValue == obj.code())
        {
            bRet = true;
        }
    }

    return bRet;
}

bool PlcManager::getYcCommand(GCCommon::GCComObjVector &vObj, int &index, bool bReq)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    if(vObj.size()<1) return false;

    bool bRet = false;
    int iValue = 0;

    if(bReq)
    {
        PlcManager::getPlcProtocol()->readAllYC();
        CxApplication::waiting(PLC_WATING_PERIOD*2);
    }

    for(int i=0;i<vObj.size();i++)
    {
        GCCommon::GCCommonObj obj = vObj.at(i);
        std::string sName = (obj.name());
        if(PlcManager::getPlcProtocol()->getYcCommand(sName, iValue))
        {
            if(iValue == (int)obj.code())
            {
                index = i;
                bRet = true;
                break;
            }
        }
    }

    return bRet;
}

bool PlcManager::waitYxEvent(const string &sCommand, int iValue,uint32 iTimeOut, bool bCanStop)
{
    GCCommon::GCCommonObj obj;
    obj.setName(sCommand);
    obj.setCode(iValue);
    return waitYxEvent(obj,iTimeOut,bCanStop);
}


bool PlcManager::waitYxEvent(GCCommon::GCCommonObj &obj, uint32 iTimeOut, bool bCanStop)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    bool bRet = false;

    int iValue = 0;
    msepoch_t dtNow = CxTime::currentSystemTime();
    if(dtNow > 0)
    {
        msepoch_t iG = CxTime::currentSystemTime() - dtNow;
        while(iG < iTimeOut)
        {
            if (bCanStop)
            {
                if (f_cancelWait)
                {
                    f_cancelWait = false;
                    break;
                }
            }
            iG = CxTime::currentSystemTime() - dtNow;
            CxApplication::processEvents();

            PlcManager::getPlcProtocol()->readAllYX();
            CxApplication::waiting(PLC_WATING_PERIOD);

            std::string sName = (obj.name());
            if(PlcManager::getPlcProtocol()->getYxCommand(sName, iValue))
            {
                if(iValue == obj.code())
                {
                    cxDebug() << CxTime::currentSystemTime() << sName << iValue << cxEndLine;
                    bRet = true;
                    break;
                }
            }
            if(bRet) break;
        }
    }

    return bRet;
}

bool PlcManager::waitYxEvent(GCCommon::GCComObjVector &vObj, uint32 iTimeOut, int * pIndex, bool bCanStop)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    if(vObj.size()<1) return false;
    bool bRet = false;

    int iValue = 0;
    msepoch_t dtNow = CxTime::currentSystemTime();
    if(dtNow > 0)
    {
        msepoch_t iG = CxTime::currentSystemTime() - dtNow;
        while(iG < iTimeOut)
        {
            if (bCanStop)
            {
                if (f_cancelWait)
                {
                    f_cancelWait = false;
                    break;
                }
            }
            iG = CxTime::currentSystemTime() - dtNow;
            CxApplication::processEvents();

            PlcManager::getPlcProtocol()->readAllYX();
            CxApplication::waiting(PLC_WATING_PERIOD*(vObj.size()/PLC_WATING_TIME+1));

            for(int i=0;i<vObj.size();i++)
            {
                GCCommon::GCCommonObj obj = vObj.at(i);

                std::string sName = (obj.name());
                if(PlcManager::getPlcProtocol()->getYxCommand(sName, iValue))
                {
                    if(iValue == obj.code())
                    {
                        bRet = true;
                        if (pIndex)
                        {
                            * pIndex = i;
                        }
                        break;
                    }
                }
            }
            if(bRet) break;
        }
    }

    return bRet;
}

bool PlcManager::waitYcEvent(const string &sCommand, int iValue,uint32 iTimeOut)
{
    GCCommon::GCCommonObj obj;
    obj.setName(sCommand);
    obj.setCode(iValue);
    return waitYcEvent(obj,iTimeOut);
}

bool PlcManager::waitYcEvent(GCCommon::GCCommonObj &obj, uint32 iTimeOut)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    bool bRet = false;

    int iValue = 0;
    msepoch_t dtNow = CxTime::currentSystemTime();
    if(dtNow > 0)
    {
        msepoch_t iG = CxTime::currentSystemTime() - dtNow;
        while(iG < iTimeOut)
        {
            iG = CxTime::currentSystemTime() - dtNow;
            CxApplication::processEvents();

            PlcManager::getPlcProtocol()->readAllYC();
            CxApplication::waiting(PLC_WATING_PERIOD);

            std::string sName = (obj.name());
            if(PlcManager::getPlcProtocol()->getYcCommand(sName, iValue))
            {
                if(iValue == obj.code())
                {
                    bRet = true;
                    break;
                }
            }
            if(bRet)break;
        }
    }

    return bRet;
}

bool PlcManager::waitYcEvent(GCCommon::GCComObjVector &vObj, uint32 iTimeOut)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    if(vObj.size()<1) return false;
    bool bRet = false;

    int iValue = 0;
    msepoch_t dtNow = CxTime::currentSystemTime();
    if(dtNow > 0)
    {
        msepoch_t iG = CxTime::currentSystemTime() - dtNow;
        while(iG < iTimeOut)
        {
            iG = CxTime::currentSystemTime() - dtNow;
            CxApplication::processEvents();

            PlcManager::getPlcProtocol()->readAllYC();
            CxApplication::waiting(PLC_WATING_PERIOD*(vObj.size()/PLC_WATING_TIME+1));

            for(int i=0;i<vObj.size();i++)
            {
                GCCommon::GCCommonObj obj = vObj.at(i);

                std::string sName = (obj.name());
                if(PlcManager::getPlcProtocol()->getYcCommand(sName, iValue))
                {
                    if(iValue == obj.code())
                    {
                        bRet = true;
                        break;
                    }
                }
            }
            if(bRet)break;
        }
    }

    return bRet;
}

bool PlcManager::waitYcEvent(GCCommon::GCComObjVector &vObj, uint32 iTimeOut, int &index)
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    if(vObj.size()<1) return false;
    int iValue = 0;
    bool bRet = false;
    msepoch_t dtNow = CxTime::currentSystemTime();
    if(dtNow > 0)
    {
        msepoch_t iG = CxTime::currentSystemTime() - dtNow;
        while(iG < iTimeOut)
        {
            iG = CxTime::currentSystemTime() - dtNow;
            CxApplication::processEvents();

            PlcManager::getPlcProtocol()->readAllYC();
            CxApplication::waiting(PLC_WATING_PERIOD*(vObj.size()/PLC_WATING_TIME+1));

            for(int i=0;i<vObj.size();i++)
            {
                GCCommon::GCCommonObj obj = vObj.at(i);

                std::string sName = (obj.name());
                if(PlcManager::getPlcProtocol()->getYcCommand(sName, iValue))
                {
                    if(iValue == obj.code())
                    {
                        bRet = true;
                        index = i;
                        break;
                    }
                }
            }
            if(bRet)break;
        }
    }
    return bRet;
}

bool PlcManager::getCancelWait()
{
    return f_cancelWait;
}


void PlcManager::setCancelWait(bool value)
{
    f_cancelWait = value;
}

//总召
bool PlcManager::callUp()
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    PlcManager::getPlcProtocol()->CallUp(true);
    return true;
}

bool PlcManager::isOnline()
{
    cxPromptCheck(PlcManager::getPlcProtocol(), return false);
    return PlcManager::getPlcProtocol()->isOnline();
}

