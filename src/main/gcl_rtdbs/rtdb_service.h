#ifndef RTDB_SERVICE_H
#define RTDB_SERVICE_H


#include <ccxx/ccxx.h>
#include <psm_struct.h>
#include <psm/general_access.h>

class RtdbService
{
public:
    static void start();

    static void stop();

    static void measureShareMemoryAfterLoad(int iMeasureType);

    static void measureShareMemoryBeforeClose(int iMeasureType);

    //定时保存变化的实时数据，建议定时为 100 毫秒
    static void timerTimeout(int iInterval);

    //收到消息的处理
    static int dealMessage( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );

    //收到文件的处理
    static int dealFileWrite( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach );

    static inline CxInterinfoOutStream outGdsPrompt() {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::promptString << " Gds : "; return r;
    }

 #if defined(GM_DEBUG)
    static inline CxInterinfoOutStream outGdsDebug() {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::debugString << " Gds : "; return r;
    }
 #else
 #undef outScriptDebug
    static inline CxInterinfoNoOutStream outScriptDebug() { return CxInterinfoNoOutStream(); }
 #define CX_NO_outScriptDebug_MACRO while (false) outScriptDebug
 #define outScriptDebug CX_NO_outScriptDebug_MACRO
 #endif

private:
    static void doStart(int, int, const void *, int, void *, void *);

    static void initDb();

};

#endif // RTDB_SERVICE_H
