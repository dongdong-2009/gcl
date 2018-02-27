
#ifndef GC_DATA_SVR_SINGTON_H
#define GC_DATA_SVR_SINGTON_H

#include <ccxx/ccxx.h>
#include <common/gccommon.h>
#include "gcl_measure_server.h"
#include <psm/general_access.h>
#include <script/scriptproxy.h>

typedef std::map<std::string,GCCommon::GCComObjVector* >  GCMap_String_ObjVec;


class GCLDataServer:public GCLMeasureServer,public IScriptProxyNotify
{
public:
   //启动
   static void start();

   //停止
   static void stop();

   //任务
   static void work();

   //消息
   static void sendMessage(std::string sCommand,std::string sParamter);

   //收到消息的处理
   static int dealMessage( const PsmExplainer * oPsm, const std::string & sCommand, const std::string & sParam, const PsmAttach * oAttach );
   //收到文件的处理
   static int dealFileWrite( const PsmExplainer * oPsm, const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const PsmAttach * oAttach );

   static bool execSql(string sSql);

   static bool loadSql(string sSql,std::vector<std::vector<std::string> > &rows,std::vector<std::string> *fields);

protected:
    void received_message_command(const std::string & sCommand, const std::string & sParam, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL);

    void received_file_data(const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL);

    void received_file(const std::string & sFilePath, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL);

private:
   static void timer(int interval);

   //消息处理
   static void  doMQ();

   //处理消息
   static void  dealMsg(GCCommon::GCDataElement &dt);

   static void deal_data(GCCommon::GCDataElement &dt,GCCommon::GCCommonObj &obj);

   //处理CICS业务
   static void  dealCICS(std::string fc);

   static bool dealCICS_01(std::string fc, GCCommon::GCComObjVector* pField, GCCommon::GCCommonObj &obj, std::vector<std::map<std::string,std::string> > &vDataSet);

   static bool dealCICS_02(std::string fc, GCCommon::GCComObjVector *pField, GCCommon::GCCommonObj &obj, std::vector<std::map<std::string,std::string> > &vDataSet);

   static bool dealCICS_03(std::string fc, GCCommon::GCComObjVector *pField, GCCommon::GCCommonObj &obj, std::vector<std::map<std::string,std::string> > &vDataSet);

   //任务处理
   static void doTask();

   //加载CICS配置
   static void loadCICSConfig();
   //加载RTDB配置
   static void loadRTDBConfig();

   static void loadChanConfig( TiXmlNode *node);
   static void loadMeasuerConfig( TiXmlNode *node);

   static bool loadMeasureMap(GCMap_String_ObjVec *obj, TiXmlNode *node);

   //加载通用对象队列
   static bool loadCommonObjVec(GCCommon::GCComObjVector *obj, TiXmlNode *node);

   //加载通用对象
   static bool loadCommonObj(GCCommon::GCCommonObj &obj, TiXmlElement *ele);


   static void createHead(std::map<std::string, std::string> &sHeads);

   static void deal_command_1(std::string sParam);

   static void deal_command_2(std::string sParam);

};

#endif // GC_DATA_SVR_SINGTON_H
