#include "terminal_exe.h"

#include "terminal_loader.h"
#include "measure_sharememory_all.h"
#include "fileinfo_sharememory.h"
#include <psm/general_access.h>
#include "monsb_control_task.h"


using namespace std;


TerminalExe * fn_oTerminalExe()
{
    GeneralAccess::registerMessageCommandDeal(TerminalExe::dealMessage);
    GeneralAccess::registerFileWriteDeal(TerminalExe::dealFileWrite);
    GeneralAccess::registerResultFileWriteDeal(TerminalExe::dealResultFileWrite);
    static TerminalExe m;
    return &m;
}

static TerminalExe * f_oTerminalExe = fn_oTerminalExe();

//*static data
static std::vector<std::string> f_sLogs;


std::string TerminalExe::log_directory() const
{
    return GclConfig::logPath();
}

std::string TerminalExe::log_fileName(int type, int reason, int source, int target, int tag) const
{
    std::string r;
    //channel Type_IO_Channel
    if (type > 0x04000000 && type < 0x05000000)
    {
        TerminalBase * oTerminal = NULL;
        if (type < 0x04000201)
        {
            oTerminal = TerminalManager::terminalByChannel(source);
        }
        else if (type == CxInterinfo::Type_Terminal)
        {
            oTerminal = TerminalManager::terminal(source);
        }
        else if (type == CxInterinfo::Type_Protocol)
        {
            oTerminal = TerminalManager::terminalByProtocol(source);
        }
        else if (type == CxInterinfo::Type_Business)
        {
            oTerminal = TerminalManager::terminalByBusiness(source);
        }
        if (oTerminal)
        {
            r = oTerminal->terminalName();
        }
    }
    else if (type == CxInterinfo::Type_Script)
    {
        return "lua.log";
    }
    if (r.empty())
    {
//      return CxFileSystem::extractFilePrefixName(CxApplication::applicationFileName()) + ".log";
        return "system.log";
    }
    else
    {
        return r + ".log";
    }
}

bool TerminalExe::log_hasFileName(int type, int reason, int source, int target, int tag) const
{
    if (type >= 0x04000000 && type < 0x05000000)
    {
        return true;
    }
    else if (type == 0x03000008)
    {
        return true;
    }
    return false;
}

int TerminalExe::load(int argc, const char *argv[])
{
    CxApplication::init(argc, argv);

//    cxDebug()<<argc<<argv<<cxEndLine;

    f_sLogs.push_back( "Application load begin" );

    GclConfig::load();

    CxLogManager::setLogFileTactics(f_oTerminalExe);

    MonsbOperateManager::initializeProcess();

    f_sLogs.push_back( "Application load MonsbManager YcaddManager StrawManager" );

    int iMeasureIdBegin = 0;
    int iMeasureCount = 0;
    if ( CxFileSystem::isExist( GclConfig::monsbMeasurePath()) )
    {
        MonsbManager::loadFromFile( GclConfig::monsbMeasurePath() );
    }
    else
    {
        iMeasureIdBegin = CxApplication::findConfig( CS_SectionMeasure , CS_EntryMonsbBegin , ci_int_zero );
        iMeasureCount = CxApplication::findConfig( CS_SectionMeasure , CS_EntryMonsbCount , ci_int_zero );
        if (iMeasureIdBegin>=ci_minMeasureIdMonsb && iMeasureIdBegin<ci_maxMeasureIdMonsb && iMeasureCount>0 && iMeasureCount<1000*100)
        {
            MonsbManager::loadFromAuto(iMeasureIdBegin, iMeasureCount);
        }
    }

    if ( CxFileSystem::isExist( GclConfig::ycaddMeasurePath()) )
    {
        YcaddManager::loadFromFile( GclConfig::ycaddMeasurePath() );
    }
    else
    {
        iMeasureIdBegin = CxApplication::findConfig( CS_SectionMeasure , CS_EntryYcaddBegin , ci_int_zero );
        iMeasureCount = CxApplication::findConfig( CS_SectionMeasure , CS_EntryYcaddCount , ci_int_zero );
        if (iMeasureIdBegin>=ci_minMeasureIdYcadd && iMeasureIdBegin<ci_maxMeasureIdYcadd && iMeasureCount>0 && iMeasureCount<1000*100)
        {
            YcaddManager::loadFromAuto(iMeasureIdBegin, iMeasureCount);
        }
    }

    if ( CxFileSystem::isExist( GclConfig::strawMeasurePath()) )
    {
        StrawManager::loadFromFile( GclConfig::strawMeasurePath() );
    }
    else
    {
        iMeasureIdBegin = CxApplication::findConfig( CS_SectionMeasure , CS_EntryStrawBegin , ci_int_zero );
        iMeasureCount = CxApplication::findConfig( CS_SectionMeasure , CS_EntryStrawCount , ci_int_zero );
        if (iMeasureIdBegin>=ci_minMeasureIdStraw && iMeasureIdBegin<ci_maxMeasureIdStraw && iMeasureCount>0 && iMeasureCount<1000*100)
        {
            StrawManager::loadFromAuto(iMeasureIdBegin, iMeasureCount);
        }
    }

    if ( GclConfig::isLoadMonsbOperate() )
    {
        f_sLogs.push_back( "Application load MonsbOperateManager" );
        MonsbOperateManager::loadFromFile( GclConfig::monsbOperatesPath() );
    }
    //加载全部链路
    string sConfigFilePath = CxApplication::configFilePath();
    if (!CxFileSystem::isExist(sConfigFilePath))
    {
        sConfigFilePath = CxApplication::configDefaultFilePath();
    }
    if(CxApplication::findArgument("terminal").empty())
    {
        f_sLogs.push_back( "Application load All Terminal" );
        TerminalLoader::load( sConfigFilePath );
    }
    else//加载指定链路
    {
        string sTermName = CxApplication::findArgument("terminal");

        f_sLogs.push_back( "Application load Terminal:"+sTermName);
        TerminalLoader::load( sConfigFilePath, sTermName );
    }

    CxSkverManager::closeDeleteAll();
    f_sLogs.push_back( "Application load end" );

    //every init function : do
    vector<fn_void_t> * oFnRegists = getFnAfterLoads();
    for (size_t i = 0; i < oFnRegists->size(); ++i)
    {
        fn_void_t fn = oFnRegists->at(i);
        fn();
    }

    return TRUE;
}

int TerminalExe::start()
{
    MonsbControlTaskManager::start();

    f_sLogs.push_back( "Application start MonsbControlTaskManager" );
    TerminalManager::start();
    f_sLogs.push_back( "Application start TerminalManager" );
    f_sLogs.push_back( "Application start end" );
    cxPrompt().lf().end() << f_sLogs;
    f_sLogs.clear();

    return TRUE;
}

void TerminalExe::stop()
{
    GeneralAccess::stop();

    TerminalManager::stop();

    MonsbControlTaskManager::stop();
#ifdef GCL_CONFIG
    TerminalLoader::save(CxApplication::applicationConfigFilePath());
#endif

    MeasureShareMemoryAll::close();
    FileInfoShareMemory::close();

    CxSkverManager::closeDeleteAll();
}

int TerminalExe::exe(int argc, const char *argv[])
{
    load(argc, argv);

    start();

    for(size_t i=0;i<f_sLogs.size();i++)
    {
        cxPrompt()<<f_sLogs.at(i)<<cxEndLine;
    }

    f_sLogs.clear();

    int iResult = CxApplication::exec();

    stop();

//    CxApplication::saveConfig();

    return iResult;
}

void TerminalExe::registAfterLoad(fn_void_t fn)
{
    vector<fn_void_t> * oFnAfterLoads = getFnAfterLoads();
    if (std::find(oFnAfterLoads->begin(), oFnAfterLoads->end(), fn) == oFnAfterLoads->end())
        oFnAfterLoads->push_back(fn);
}

int TerminalExe::dealMessage(const PsmExplainer *oPsm, const string &sCommand, const string &sParam, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    if (sCommand.find("send.yk") != string::npos || sCommand.find("send.yt") != string::npos || sCommand.find("send.ys") != string::npos )
    {
        TerminalExe::dealPsmMeasureControl(sCommand, sParam, oAttach);
        cxDebug() << "bus recv message [ " << sCommand << " ] from " << CxString::format("0x%08x",oAttach->sourceId) << " to " << CxString::format("0x%08x",oAttach->targetId);
        //return TRUE;
    }
    else
    {
    }

    return FALSE;
}

int TerminalExe::dealFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    int iResult = FALSE;

    return iResult;
}

int TerminalExe::dealResultFileWrite(const PsmExplainer *oPsm, const CxFileSystem::PathInfo &pathInfo, const PsmAttach *oAttach)
{
    GM_ASSER_OBJECT(oAttach);

    int iResult = FALSE;

    return iResult;
}

std::vector<fn_void_t> * TerminalExe::getFnAfterLoads()
{
    static vector<fn_void_t> m;
    return & m;
}

void TerminalExe::dealPsmMeasureControl(const std::string &sCommand, const string & sParam, const PsmAttach *oAttach)
{
    //    struct ControlData
    //    {
    //        int controlId; // 变化ID : 1,2,3...0x7fffffff
    //        int measureId; // 量ID
    //        T value; // 量值
    //        msepoch_t controlTime; // 值的修改时间
    //        int controlSourceId; // 控制的源ID
    //        int controlReasonId; // 控制的原因
    //        int controlStrategyId; // 控制的策略
    //        int controlMethodId; // 控制的方法
    //        char params[512];
    //        int res; // 保留
    //    };

    //send.yk measure=0x01000011;value=1;reason=2;strategy=3;method=4\nk11=v11;k12=v12\nk21=v21;k22=v22\n
    //sCommand : send.yk
    //sParam measure=0x01000011;value=1;reason=2;strategy=3;method=4\nk11=v11;k12=v12\nk21=v21;k22=v22\n

    //    std::string sMainParam = CxString::token(sParam, '\n');
    std::string sMainParam;
    std::string sSubParam;
    size_t iIndex = sParam.find('\n');
    if (iIndex != string::npos)
    {
        sMainParam = sParam.substr(0, iIndex);
        sSubParam = sParam.substr(iIndex+1);
    }
    else
    {
        sMainParam = sParam;
        sSubParam = "";
    }

    std::map<string, string> sMainParams = CxString::splitToMap( sMainParam , '=', ';' );

    int controlMeasureId = CxString::toInt32(sMainParams[CS_EntryControlMeasure]); // 量ID
    int iMeasureType = g_fn_getMeasureType(controlMeasureId);

    if (iMeasureType == MeasureType_None) {
        cxDebug() << "dealPsmMeasureControl error : measure is invalid !!!"<<controlMeasureId;
        return;
    }

    msepoch_t controlTime = CxTime::currentSystemTime(); // 值的控制时间
    int controlSourceId = oAttach ? oAttach->sourceId : 0; // 控制的源ID
    int controlReasonId = 0; // 控制的原因
    int controlStrategyId = 0; // 控制的策略
    int controlMethodId = 0; // 控制的方法
    int transferCount = 0;

    controlReasonId = CxContainer::valueTo(sMainParams, CS_EntryControlReasonId, controlReasonId);
    controlStrategyId = CxContainer::valueTo(sMainParams, CS_EntryControlStrategyId, controlStrategyId);
    controlMethodId = CxContainer::valueTo(sMainParams, CS_EntryControlMethodId, controlMethodId);

    if (iMeasureType == MeasureType_Monsb)
    {
        int controlValue = CxContainer::valueTo(sMainParams, CS_EntryControlValue, int(-1));
        int iResult = SendMonsbControlManager::sendControl(controlMeasureId, controlValue, controlSourceId);
        cxPrompt() << CxString::format("dealPsmMeasureControl sendControl(MeasureId, Value, SourceId):{(%d, %d, %d)result: %d}", controlMeasureId, controlValue, controlSourceId, iResult);
    }
    else if (iMeasureType == MeasureType_Ycadd)
    {
        cxPrompt() << "dealPsmMeasureControl do not suport Ycadd.";
    }
    else if (iMeasureType == MeasureType_Straw)
    {
        cxPrompt() << "dealPsmMeasureControl do not suport Straw.";
    }
}


