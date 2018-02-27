
#include "gcl_rttrigger.h"

using namespace std;

static int f_iTimeOutTimes = 0;

static size_t f_monsbChangedIndex = GM_UINT_MAX;
static size_t f_ycaddChangedIndex = GM_UINT_MAX;
static size_t f_strawChangedIndex = GM_UINT_MAX;

static string CS_CHANGED_NAME = "changed_";
static string CS_TIME_NAME = "timer_";
static string CS_COMMAND_NAME = "command_";
static string CS_CONFIG_NAME = "getConfig_";
static string CS_MeasureChanged = "fnMeasureChanged";
static string CS_MeasureTimeOut = "fnMeasureTimeOut";
static string CS_MeasureTimeOutInterval = "fnMeasureTimeOutInterval";
static string CS_MeasureCommand = "fnMeasureCommand";

typedef struct TimeMeasureInfo
{
    //config

    //history
    double nowTimeout;
    int dealSuccedTimes;
    int runTimes;
    int lastTimeout;
    char cfilename[256];
} MeasureInfo_t, * MeasureInfo_p;

typedef struct TimeMeasureRunInfo
{
    double interval; //定时
    //config info
    int overTimes;
    int runTimes;

    //run info
    msepoch_t nextRunTime;
    msepoch_t lastRunTime;
    vector<string> dealFunctionName;
}TimeMeasureInfo_t, * TimeMeasureInfo_p;

static map<int , TimeMeasureInfo> timeMeasureInfos;
static map<int , string> changeMeasureInfos;
static map<double, TimeMeasureRunInfo> timeMeasureRunInfos;
static map<int , string> msgMeasureInfos;


static map<string, CxFileSystem::PathInfo> dealFileInfos;

static map<int, map<string, string> > measureIDConfigs;


static RTTrigger *fn_RTTrigger()
{
    CxApplication::registStartFunction(RTTrigger::start);
    CxApplication::registStopFunction(RTTrigger::stop);
    GclLua::registLuaFileLoaded(RTTrigger::LuaFileLoaded);
    GeneralAccess::registerMessageCommandDeal(RTTrigger::dealMessage);
    CxInterinfoIn::addObserver(RTTrigger::interinfoIn_cmd);

    static RTTrigger m;
    return &m;
}


static RTTrigger *f_oRTTrigger = fn_RTTrigger();


void RTTrigger::start()
{
    CxTimerManager::startTimer(RTTrigger::timerTimeout, 100);
}

void RTTrigger::stop()
{
    CxTimerManager::stopTimer(RTTrigger::timerTimeout);
}


template<typename T>
static void fn_measureChangeds_impl(const typename MeasureShareMemory<T>::ChangedData * const oChangedBegin, const typename MeasureShareMemory<T>::ChangedData * const oChangedEnd)
{
    const typename MeasureShareMemory<T>::ChangedData * oChangedData = oChangedBegin;
    while (oChangedData < oChangedEnd)
    {
        const string sChangedMeasureID = CxString::toHexstring(oChangedData->measureId);
        map<int, string>::iterator it = changeMeasureInfos.find(oChangedData->measureId);
        if (it != changeMeasureInfos.end())
        {
            bool bResult = RTTrigger::dealMeasureID(sChangedMeasureID, it->second.c_str());
            if (bResult)
            {
                cxDebug() << "Deal MeasureID: " << sChangedMeasureID << "  Deal LuaFunction Success: " << it->second.c_str();
            }
            else
            {
                cxDebug() << "Deal MeasureID: " << sChangedMeasureID << "  Deal LuaFunction Failed: " << it->second.c_str();
            }
        }
        oChangedData++;
    }
}


static void fn_measureChangeds_monsb(const MonsbShareMemory::ChangedData * const oChangedBegin, const typename MonsbShareMemory::ChangedData * const oChangedEnd)
{
    fn_measureChangeds_impl<int>(oChangedBegin, oChangedEnd);
}

static void fn_measureChangeds_ycadd(const YcaddShareMemory::ChangedData * const oChangedBegin, const typename YcaddShareMemory::ChangedData * const oChangedEnd)
{
    fn_measureChangeds_impl<double>(oChangedBegin, oChangedEnd);
}

static void fn_measureChangeds_straw(const StrawShareMemory::ChangedData * const oChangedBegin, const typename StrawShareMemory::ChangedData * const oChangedEnd)
{
    fn_measureChangeds_impl<StrawValue>(oChangedBegin, oChangedEnd);
}


void RTTrigger::timerTimeout(int iInterval)
{
    //处理变化
    if (MonsbShareMemory::memoryManager()->isValid())
    {
        f_monsbChangedIndex = MonsbShareMemory::memoryManager()->checkChangeds(f_monsbChangedIndex, fn_measureChangeds_monsb);
    }

    if (YcaddShareMemory::memoryManager()->isValid())
    {
        f_ycaddChangedIndex = YcaddShareMemory::memoryManager()->checkChangeds(f_ycaddChangedIndex, fn_measureChangeds_ycadd);
    }

    if (StrawShareMemory::memoryManager()->isValid())
    {
        f_strawChangedIndex = StrawShareMemory::memoryManager()->checkChangeds(f_strawChangedIndex, fn_measureChangeds_straw);
    }

    //处理定时
    for (map<double, TimeMeasureRunInfo>::iterator it = timeMeasureRunInfos.begin(); it != timeMeasureRunInfos.end(); ++it)
    {
        if (it->second.runTimes <= 0)
        {
            it->second.lastRunTime = CxTime::currentSystemTime();
            it->second.nextRunTime = CxTime::currentSystemTime() + (it->first * 1000);
            (it->second.runTimes)++;

        }
        else
        {
            if (CxTime::currentSystemTime() > it->second.nextRunTime)
            {
                for (size_t i = 0; i < it->second.dealFunctionName.size(); ++i)
                {
                    const string & sFunctionName = it->second.dealFunctionName.at(i);
                    if (sFunctionName != "")
                    {
                        bool bResult = dealMeasureID("", sFunctionName.c_str());
                        if (bResult)
                        {
                            cxDebug() << "TimeOut: " << it->first << "s" << "  Deal LuaFunction Success: " << sFunctionName;
                        }
                        else
                        {
                            cxDebug() << "TimeOut: " << it->first << "s" << "  Deal LuaFunction Faile: " << sFunctionName;
                        }
                    }
                }
                it->second.lastRunTime = it->second.nextRunTime;
                it->second.nextRunTime = it->second.nextRunTime + (it->first * 1000);
                (it->second.runTimes)++;
//                cxDebug() << "Deal Time: " << it->first << f_iTimeOutTimes/10;
            }
        }
    }

    f_iTimeOutTimes++;
}


bool RTTrigger::dealMeasureID(const string sMeasureID, const char *sFunctionName)
{
    int iResult = 0;
    bool bResult = FALSE;
    lua_State *L = GclLua::luaState();

    int iLuaType = lua_getglobal(L, sFunctionName);
    if (LUA_TFUNCTION == iLuaType)
    {
        if (CxString::beginWith(sFunctionName, CS_CHANGED_NAME))
        {
            lua_pushstring(L, sMeasureID.c_str());
            bResult = LuaCommon::callLuaFunction(L, 1, 0, iResult);
        }
        else
        {
            bResult = LuaCommon::callLuaFunction(L, 0, 0, iResult);
        }

    }
    else if (LUA_TNIL == iLuaType)
    {
        cxDebug() << "Get LuaFunction Error,Please check lua configs. Configs Function Name: " << sFunctionName << "    MeasureID: " << sMeasureID;
    }

    return bResult;
}


map<string, string> RTTrigger::getMeasureConfig(lua_State *L, const char *pFunctionName)
{
    int bResult;
    map<string, string> sConfigs;

    int iLuaType = lua_getglobal(L, pFunctionName);

    if (LUA_TFUNCTION == iLuaType)
    {
        cxDebug() << "Get config: " << pFunctionName;
        //获取 config table
        bResult = lua_pcall(L,0,1,0);
        if (0 == bResult)
        {
            int iType = lua_type(L, -1);
            if (iType == LUA_TTABLE)
            {
                int t = 0;
                //获取变化函数
                lua_pushstring(L, CS_MeasureChanged.c_str());
                t = lua_gettable(L, -2);
                if (t != LUA_TNIL)
                {
                    size_t iLen = 0;
                    string sfnMeasureChanged = lua_tolstring(L, -1, &iLen);
                    if (sfnMeasureChanged != "")
                    {
                        sConfigs[CS_MeasureChanged] = sfnMeasureChanged;
                        cxDebug() << "Get fnMeasureChanged: " << sfnMeasureChanged;
                    }

                }
                lua_pop(L, 1);

                //获取定时函数
                lua_pushstring(L, CS_MeasureTimeOut.c_str());
                t = lua_gettable(L, -2);
                if (t != LUA_TNIL)
                {
                    size_t iLen = 0;
                    string sfnMeasureTimeOut = lua_tolstring(L, -1, &iLen);
                    sConfigs[CS_MeasureTimeOut] = sfnMeasureTimeOut;
                    cxDebug() << "Get fnMeasureTimeOut: " << sfnMeasureTimeOut;

                }
                lua_pop(L, 1);

                //获取定时时间
                lua_pushstring(L, CS_MeasureTimeOutInterval.c_str());
                t = lua_gettable(L, -2);
                if (t != LUA_TNIL)
                {
                    string sfnMeasureTimeOutInterval = lua_tostring(L, -1);
                    if (sfnMeasureTimeOutInterval != "")
                    {
                        sConfigs[CS_MeasureTimeOutInterval] = sfnMeasureTimeOutInterval;
                        cxDebug() << "Get fnMeasureTimeOutInterval: " << sfnMeasureTimeOutInterval;
                    }
                }
                lua_pop(L, 1);

                //获取命令函数
                lua_pushstring(L, CS_MeasureCommand.c_str());
                t = lua_gettable(L, -2);
                if (t != LUA_TNIL)
                {
                    string sfnMeasureCommand = lua_tostring(L, -1);
                    sConfigs[CS_MeasureCommand] = sfnMeasureCommand;
                    cxDebug() << "Get fnMeasureCommand: " << sfnMeasureCommand;
                }
                lua_pop(L, 1);

            }
        }
    }
    lua_settop(L, 0);
    return sConfigs;
}

void RTTrigger::LuaFileLoaded(const char *pFilePath, int iResultLoaded)
{
    string sFilePath = string(pFilePath);

    if (CxString::endWith(sFilePath, ".deal.lua") && (iResultLoaded == 0))
    {
        string sFileName = CxFileSystem::extractFileName(sFilePath);
        vector<string> sHead = CxString::split(sFileName, ".");

        if (sHead.size()>0 && CxString::isvalidInteger(sHead.at(0)))
        {
            //前期预处理
            MeasureInfo_t measureInfo;
            TimeMeasureInfo_t timeMeasureInfo;
            //TimeMeasureInfo_t.dealFunctionName.clear();

            //材料准备
            lua_State *L = GclLua::luaState();
            const string sGetConfigName = CS_CONFIG_NAME + sHead.at(0);

            int iTearminalID = CxString::toInt32(sHead.at(0));

            map<string, string> sConfigs = RTTrigger::getMeasureConfig(L, sGetConfigName.c_str());
            map<string, string> oldMeasureIDConfigs;
            if (CxContainer::contain(measureIDConfigs, iTearminalID))
            {
                oldMeasureIDConfigs = measureIDConfigs[iTearminalID];
            }
            else
            {
                oldMeasureIDConfigs = sConfigs;
            }

            double iOldTime = 0;
            if(CxContainer::contain(oldMeasureIDConfigs, CS_MeasureTimeOutInterval))
            {
                iOldTime = CxString::toDouble(oldMeasureIDConfigs[CS_MeasureTimeOutInterval]);
            }

            //配置处理
            measureIDConfigs[iTearminalID] = sConfigs;

            if (oldMeasureIDConfigs.size() > sConfigs.size())
            {
                for (map<string, string>::iterator it = oldMeasureIDConfigs.begin(); it != oldMeasureIDConfigs.end(); ++it)
                {
                    string sKey = it->first;
                    map<string, string>::iterator itMeasureConfig = sConfigs.find(sKey);
                    //删除
                    if(itMeasureConfig == sConfigs.end())
                    {
                        if (sKey == CS_MeasureChanged)
                        {
                            CxContainer::remove(changeMeasureInfos, iTearminalID);
                        }
                        else if(sKey == CS_MeasureTimeOut)
                        {
                            CxContainer::remove(timeMeasureInfos, iTearminalID);
                            if (iOldTime > 0 && CxContainer::contain(timeMeasureRunInfos, iOldTime))
                            {
                                CxContainer::remove(timeMeasureRunInfos[iOldTime].dealFunctionName, it->second);
                            }
                        }
                        else if(sKey == CS_MeasureCommand)
                        {
                            CxContainer::remove(msgMeasureInfos, iTearminalID);
                        }
                    }
                }
            }

            for (map<string, string>::iterator itConfig = sConfigs.begin(); itConfig != sConfigs.end(); ++itConfig)
            {
                const string & sFunctionName = itConfig->second;;

                if  (itConfig->first == CS_MeasureChanged)
                {
                    changeMeasureInfos[iTearminalID] = itConfig->second;
                }
                else if (itConfig->first == CS_MeasureCommand)
                {
                    msgMeasureInfos[iTearminalID] = itConfig->second;
                }
                else if (itConfig->first == CS_MeasureTimeOut)
                {
                    if (CxContainer::contain(sConfigs, CS_MeasureTimeOutInterval))
                    {
                        double itimeout = CxString::toDouble(sConfigs[CS_MeasureTimeOutInterval]);
                        if (itimeout > 0)
                        {
                            strcpy(measureInfo.cfilename, sFunctionName.c_str());
                            measureInfo.nowTimeout = itimeout;
                            timeMeasureInfos[iTearminalID] = measureInfo;

                            if (CxContainer::contain(timeMeasureRunInfos, itimeout))
                            {
                                //删除旧的
                                if (iOldTime > 0 && CxContainer::contain(oldMeasureIDConfigs, CS_MeasureTimeOut))
                                {
                                    const string & oldFnMeasureTimeOut = oldMeasureIDConfigs[CS_MeasureTimeOut];
                                    if (CxContainer::contain(timeMeasureRunInfos, iOldTime))
                                    {
                                        CxContainer::remove(timeMeasureRunInfos[iOldTime].dealFunctionName, oldFnMeasureTimeOut);
                                    }
                                }
                                timeMeasureRunInfos[itimeout].dealFunctionName.push_back(sFunctionName);
                            }
                            else
                            {
                                //配置中时间改变
                                //删旧配置
                                if (iOldTime > 0 && CxContainer::contain(oldMeasureIDConfigs, CS_MeasureTimeOut))
                                {
                                    CxContainer::remove(timeMeasureRunInfos[iOldTime].dealFunctionName, sFunctionName);
                                    CxContainer::remove(timeMeasureInfo.dealFunctionName, sFunctionName);
                                }
                                //配置中程序名改变 时间改变
                                timeMeasureInfo.dealFunctionName.push_back(sFunctionName);
                                timeMeasureInfo.interval = itimeout;
                                timeMeasureInfo.runTimes = 0;
                                timeMeasureRunInfos[itimeout] = timeMeasureInfo;
                            }
                        }
                        else
                        {
                            cxDebug() << "Time error! Please check the value of fnMeasureTimeOutInterval is valid!  FileName : " << sFileName.c_str();
                        }
                    }
                    else
                    {
                        cxDebug() << "Lua Config error! Please check the fnMeasureTimeOutInterval is valid!     FileName : " << sFileName.c_str();
                        if (iOldTime > 0 && CxContainer::contain(oldMeasureIDConfigs, CS_MeasureTimeOut))
                        {
                            CxContainer::remove(timeMeasureRunInfos[iOldTime].dealFunctionName, sFunctionName);
                        }
                        //删除旧时间中的程序名
                        CxContainer::remove(timeMeasureInfo.dealFunctionName, sFunctionName);
                    }
                }
                else if (itConfig->first == "fnMeasureTimeOutInterval")
                {

                }
                else
                {
                    cxDebug() << "Lua Config error! Please check the Config. Error Function Name: " << sFunctionName.c_str();
                }
            }

            map<double, TimeMeasureRunInfo>::iterator itInfos = timeMeasureRunInfos.begin();
            while (itInfos != timeMeasureRunInfos.end())
            {
                if (itInfos->second.dealFunctionName.empty())
                {
                    timeMeasureRunInfos.erase(itInfos++);
                }
                else
                {
                    ++itInfos;
                }
            }
            double dTime = 0;
            CxContainer::remove(timeMeasureRunInfos, dTime);
        }
        else
        {
            cxDebug() << "FileName is no correct! FileName: " << sFileName.c_str();
        }
    }
}

int RTTrigger::dealMessage(const PsmExplainer *oPsm, const string &sCommand, const string &sParam, const PsmAttach *oAttach)
{
    int iResult = 0;
    bool bResult = FALSE;
    cxDebug() << "Get Message:\nCommand: " << sCommand << "\nParam: " << sParam;

    if ((sCommand.find("measure.trigger") != string::npos)  && (sParam.find("measure") != string::npos))
    {
        vector<string> sLines = CxString::split(sParam, ";");
        for (size_t i = 0; i < sLines.size(); ++i)
        {
            if (sLines.at(i).find("measure") != string::npos)
            {
                vector<string> measureLines = CxString::split(sLines.at(i), "=");
                if (measureLines.size() > 1 && CxString::isvalidInteger(measureLines.at(1)))
                {
                    int iMeasureID = CxString::toInt32(measureLines.at(1));
                    map<int, string>::iterator it = msgMeasureInfos.find(iMeasureID);
                    if (it != msgMeasureInfos.end())
                    {
                        lua_State * L = GclLua::luaState();
                        //获取全局函数
                        int iLuaType = lua_getglobal(L, it->second.c_str());
                        if (iLuaType == LUA_TFUNCTION)
                        {
                            //参数压栈
                            lua_pushstring(L, sCommand.c_str());
                            GclLuaBase::stringToTable(L, sParam);
                            bResult = LuaCommon::callLuaFunction(L, 2, 1, iResult);
                            if (bResult)
                            {
                                cxDebug() << "Deal Meassage Function Success! FunctionName: " << it->second.c_str();
                            }
                            else
                            {
                                cxDebug() << "Deal Meassage Function Failed! FunctionName: " << it->second.c_str();
                            }
                        }
                    }
                }
                else
                {
                    cxDebug() << "Get Message, MeasureID Error: " << measureLines.at(1);
                }
            }
        }
    }
    return bResult;
}

vector<string> RTTrigger::reportAll()
{
    vector<string> sReports;
    string sReportFilePath = CxFileSystem::mergeFilePath( CxApplication::applicationDeployPath(), "report" );
    sReportFilePath = CxFileSystem::mergeFilePath( sReportFilePath, CxApplication::applicationTargetName() + ".report.txt" );

    //配置
    sReports.push_back("**** Measure ID Lua File Config Report ****");
    sReports.push_back(CxString::format("Measure ID Config Count: %d,", measureIDConfigs.size()));
    sReports.push_back("Config:[");
    map<int, map<string, string> > tempMeasureIDConfigs = measureIDConfigs;
    for (map<int, map<string, string> >::iterator it = tempMeasureIDConfigs.begin(); it != tempMeasureIDConfigs.end(); ++it)
    {
       sReports.push_back(CxString::format("Measure ID %08d : { %s : %18s, %s : %18s, %s : %18s, %s : %-04s }",
                                           it->first, CS_MeasureChanged.c_str(), it->second[CS_MeasureChanged].c_str(), CS_MeasureCommand.c_str(), it->second[CS_MeasureCommand].c_str(),
                                           CS_MeasureTimeOut.c_str(), it->second[CS_MeasureTimeOut].c_str(), CS_MeasureTimeOutInterval.c_str(), it->second[CS_MeasureTimeOutInterval].c_str()));
    }
    sReports.push_back("]");
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");

    //实时变化
    sReports.push_back("**** Change Measure Infos Report ****");
    sReports.push_back(CxString::format("Change Measure Infos Count: %d,", changeMeasureInfos.size()));
    sReports.push_back("ChangeInfos:[");
    for (map<int, string>::iterator it = changeMeasureInfos.begin(); it != changeMeasureInfos.end(); ++it)
    {
        //{Measure ID : 16777218,		ChangeFunctionName : changed_0x01000002}
        sReports.push_back(CxString::format("Measure ID : %d, ChangeFunctionName : %18s", it->first, it->second.c_str()));
    }
    sReports.push_back("]");
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");

    //消息
    sReports.push_back("**** Message Measure Infos Report ****");
    sReports.push_back(CxString::format("Message Measure Infos Count: %d,", msgMeasureInfos.size()));
    sReports.push_back("MessageInfos:[");
    for (map<int, string>::iterator it = msgMeasureInfos.begin(); it != msgMeasureInfos.end(); ++it)
    {
        sReports.push_back(CxString::format("Measure ID : %d, MessageFunctionName : %18s", it->first, it->second.c_str()));
    }
    sReports.push_back("]");
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");

    //定时
    sReports.push_back("**** TimeOut Measure Infos Report ****");
    sReports.push_back(CxString::format("TimeOut Measure Infos Count: %d,", timeMeasureInfos.size()));
    sReports.push_back("TimeOutInfos:[");
    for (map<int, TimeMeasureInfo>::iterator it = timeMeasureInfos.begin(); it != timeMeasureInfos.end(); ++it)
    {
        sReports.push_back(CxString::format("Measure ID : %d, TimeOutFunctionName : %18s, Time : %-02f", it->first, it->second.cfilename, it->second.nowTimeout));
    }
    sReports.push_back("]");
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");

    sReports.push_back("**** Time Measure Run Infos Report ****");
    sReports.push_back(CxString::format("Time Measure Run Infos Count: %d,", timeMeasureRunInfos.size()));
    sReports.push_back("RunTimeMeasureInfos:[");
    for (map<double, TimeMeasureRunInfo>::iterator it = timeMeasureRunInfos.begin(); it != timeMeasureRunInfos.end(); ++it)
    {
        string sFunctionList = "FunctionName : [";
        for (size_t i = 0; i < it->second.dealFunctionName.size(); ++i)
        {
            string dealFunctionName = it->second.dealFunctionName.at(i);
            if(i == (it->second.dealFunctionName.size() - 1))
            {
                sFunctionList = sFunctionList + "%18s]";
                sFunctionList = CxString::format(sFunctionList.c_str(), dealFunctionName.c_str());
                continue;
            }
            sFunctionList = sFunctionList + "%18s, ";
            sFunctionList = CxString::format(sFunctionList.c_str(), dealFunctionName.c_str());
        }
        string sLastRunTime = CxTime::toString(it->second.lastRunTime);
        string sNextRunTime = CxTime::toString(it->second.nextRunTime);
        sReports.push_back(CxString::format("Time : %8.2f : {%s, LastRunTime : %s, NextRunTime : %s}", it->first, sFunctionList.c_str(), sLastRunTime.c_str(), sNextRunTime.c_str()));
    }
    sReports.push_back("]");
    sReports.push_back("#");sReports.push_back("#");sReports.push_back("#");

    bool bResult = CxFile::save(sReportFilePath, sReports);
    if (bResult)
    {
        cxPrompt() << "trigger report success , and save to file " << sReportFilePath;
    }
    else
    {
        cxPrompt() << "trigger report fail!!!" << sReportFilePath;
    }

    return sReports;
}

int RTTrigger::interinfoIn_cmd(const std::string & sCommand, const std::map<std::string, std::string> & sParams, const msepoch_t & dtIn, int iSource, int eInType, int iTag)
{
    if (sCommand == "trigger")
    {
        if (CxContainer::contain(sParams, std::string("report")))
        {
            //report
            f_oRTTrigger->reportAll();
            return CxInterinfo::InDeal_Ed;
        }
        if (CxContainer::contain(sParams, std::string("exit")))
        {
            CxApplication::exit();
            return CxInterinfo::InDeal_Ed;
        }
        return CxInterinfo::InDeal_Ed;
    }
    return 0;
}
