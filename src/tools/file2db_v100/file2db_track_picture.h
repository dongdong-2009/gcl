#ifndef FILE2DB_TRACK_WEIGH_H
#define FILE2DB_TRACK_WEIGH_H

#include <ccxx/ccxx.h>
#include <common/gcl.h>
#include <psm/general_access.h>

class File2dbTrackPicture
{
public:
   static void start();

   static void stop();

   //在主线程中把 文件 发送出去
   //定时 30 ~ 50 毫秒
   static void timerDo(int iInterval);

   //收到消息的处理
   static int dealMessage( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );

   //收到文件的处理
   static int dealFileWrite( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach );

   static inline CxInterinfoOutStream outPrompt() {
       CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::promptString << " Sls : "; return r;
   }

#if defined(GM_DEBUG)
   static inline CxInterinfoOutStream outDebug() {
       CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::debugString << " Sls : "; return r;
   }
#else
#undef outScriptDebug
   static inline CxInterinfoNoOutStream outScriptDebug() { return CxInterinfoNoOutStream(); }
#define CX_NO_outScriptDebug_MACRO while (false) outScriptDebug
#define outScriptDebug CX_NO_outScriptDebug_MACRO
#endif

private:
   static void doFile2db(const CxFileSystem::PathInfo & pathInfo);

   static int doFileData2Db(const std::vector<std::string> & sFileRows, const CxFileSystem::PathInfo & pathInfo, const std::string & sPid, std::string &sOutMessage);

   static int doFileData2Mc(const std::string & sXmlBody, const CxFileSystem::PathInfo & pathInfo, const std::string & sPid, std::string &sOutMessage);

};

#endif // FILE2DB_TRACK_WEIGH_H
