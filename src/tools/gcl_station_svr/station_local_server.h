#ifndef STATION_LOCAL_SERVER_H
#define STATION_LOCAL_SERVER_H

#include <ccxx/ccxx.h>
#include <common/gcl.h>
#include <psm/general_access.h>

class StationLocalServer
{
public:
   static void start();

   static void stop();

   //在主线程中把 文件 发送出去
   //定时 30 ~ 50 毫秒
   static void timerRunSls(int iInterval);

   //收到消息的处理
   static int dealMessage( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );

   //收到文件的处理
   static int dealFileWrite( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach );

   static inline CxInterinfoOutStream outSlsPrompt() {
       CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::promptString << " Sls : "; return r;
   }

#if defined(GM_DEBUG)
   static inline CxInterinfoOutStream outSlsDebug() {
       CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::debugString << " Sls : "; return r;
   }
#else
#undef outScriptDebug
   static inline CxInterinfoNoOutStream outScriptDebug() { return CxInterinfoNoOutStream(); }
#define CX_NO_outScriptDebug_MACRO while (false) outScriptDebug
#define outScriptDebug CX_NO_outScriptDebug_MACRO
#endif

};

#endif // STATION_LOCAL_SERVER_H
