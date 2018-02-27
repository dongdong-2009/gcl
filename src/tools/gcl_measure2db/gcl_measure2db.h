#ifndef GCL_MEASURE2DB_H
#define GCL_MEASURE2DB_H

#include <ccxx/ccxx.h>
#include <common/gcl.h>
#include <psm/general_access.h>

class Measure2db
{
public:
   static void start();

   static void stop();

   //在主线程发布出去
   //定时 1000 毫秒
   static void timerDo(int iInterval);

   //收到消息的处理
   static int dealMessage( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );

   //收到文件的处理
   static int dealFileWrite( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach );

   static inline CxInterinfoOutStream outPrompt() {
       CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::promptString << " measure2db : "; return r;
   }

#if defined(GM_DEBUG)
   static inline CxInterinfoOutStream outDebug() {
       CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::debugString << " measure2db : "; return r;
   }
#else
#undef outScriptDebug
   static inline CxInterinfoNoOutStream outScriptDebug() { return CxInterinfoNoOutStream(); }
#define CX_NO_outScriptDebug_MACRO while (false) outScriptDebug
#define outScriptDebug CX_NO_outScriptDebug_MACRO
#endif

   //json : <terminal="0x01830101" measures="0x02000001~0x02000100">
   static void update(const std::map<std::string, std::string> & row);

   static std::vector<std::string> reportSelf();

   static void doXml2db(const std::string & sXml41117);

   //CxInterinfo在命令行输入、IO的输入
   static int interinfoIn_line( const std::string & sInfo, const msepoch_t & dtIn, int iSource, int eInType, int iTag);

   //CxInterinfo在命令行输入、IO的输入
   static int interinfoIn_cmd( const std::string & sCommand, const std::map<std::string, std::string> & sParams, const msepoch_t & dtIn, int iSource, int eInType, int iTag);

private:
   static void loadMeasureConfig(const std::string &sFilePath);

};

#endif // GCL_MEASURE2DB_H
