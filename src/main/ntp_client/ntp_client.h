#ifndef DB_SERVICE_H
#define DB_SERVICE_H

#include <ccxx/ccxx.h>
#include <psm/general_access.h>

class NtpClient
{
public:
   static void start();

   static void stop();

   //定时 30 秒
   static void timerNtp(int iInterval);

   //收到消息的处理
   static int dealMessage( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );

   //收到文件的处理
   static int dealFileWrite( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach );

   static inline CxInterinfoOutStream outGdsPrompt() {
       CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::promptString << " NtpClient : "; return r;
   }

#if defined(GM_DEBUG)
   static inline CxInterinfoOutStream outGdsDebug() {
       CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::debugString << " NtpClient : "; return r;
   }
#else
#undef outScriptDebug
   static inline CxInterinfoNoOutStream outScriptDebug() { return CxInterinfoNoOutStream(); }
#define CX_NO_outScriptDebug_MACRO while (false) outScriptDebug
#define outScriptDebug CX_NO_outScriptDebug_MACRO
#endif

};

#endif // DB_SERVICE_H
