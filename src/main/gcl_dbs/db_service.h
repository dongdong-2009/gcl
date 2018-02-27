#ifndef DB_SERVICE_H
#define DB_SERVICE_H

#include <ccxx/ccxx.h>
#include <common/gcl.h>
#include <psm/general_access.h>

class DbService
{
public:
   static void start();

   static void stop();

   //在主线程中把 文件 发送出去
   //定时 30 ~ 50 毫秒
   static void timerRunGds(int iInterval);

   //定时 心跳
   static void timerHeartJump(int iInterval);

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

};

#endif // DB_SERVICE_H
