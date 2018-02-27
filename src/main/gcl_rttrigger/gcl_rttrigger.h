#ifndef GCL_RTTRIGGER
#define GCL_RTTRIGGER

#include <vdi/measure_sharememory_all.h>
#include <psm/general_access.h>
#include <common/gcl_config.h>
#include <script/gcl_lua_global.h>
#include <script/gcl_lua_base.h>

class RTTrigger
{
public:
    static void start();

    static void stop();

    static void timerTimeout(int iInterval);

    //处理
    static bool dealMeasureID(const std::string sMeasureID, const char *sFunctionName);

    static void LuaFileLoaded(const char* pFilePath, int iResultLoaded);

    static std::map<std::string, std::string> getMeasureConfig(lua_State *L, const char *pFunctionName);

    static int dealMessage( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );

    static int interinfoIn_cmd(const std::string &sCommand, const std::map<std::string, std::string> &sParams, const msepoch_t &dtIn, int iSource, int eInType, int iTag);

    std::vector<std::string> reportAll();

private:

};


#endif // GCL_RTTRIGGER
