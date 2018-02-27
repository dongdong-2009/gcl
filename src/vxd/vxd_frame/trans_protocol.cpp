#include "trans_Protocol.h"
#include <script/gcl_script_vxd.h>


GCL_Trans_ProtocolFactory* GCL_Trans_ProtocolFactory::s_instance =
        ProtocolFactoryManager::createAndRegister<GCL_Trans_ProtocolFactory>();

GCL_Trans_Protocol::GCL_Trans_Protocol()
{

}

GCL_Trans_Protocol::~GCL_Trans_Protocol()
{
}

void GCL_Trans_Protocol::toContext(CxISetSkv& context) const
{
    ProtocolCommon::toContext(context);
}

void GCL_Trans_Protocol::fromContext(const CxIGetSkv& context)
{
    ProtocolCommon::fromContext(context);
}

int GCL_Trans_Protocol::sendCustomData(int iSourceId, int iCommandId, std::map<string, string> sParams, int iDataLength, const char *pData)
{
    //needtodo
    string sData = sParams["data"];
    return writeData(sData.c_str(), sData.size());
}

void GCL_Trans_Protocol::protocolTimerTimeOut(const CxTimer *oTimer)
{
    ProtocolCommon::protocolTimerTimeOut(oTimer);

}

void GCL_Trans_Protocol::protocolReceivedData(const uchar *pData, int iLength)
{
    //收到数据
    receivedProtocolDataNow();

    //resp ok
    int iSourceId = terminalId();
    int iCommandId = 0;
    std::map<std::string, std::string> sParams;
    int iDataLength = iLength;
    //const char * pData = ,
    int iTargetId = 0;
    GclLuaVxd::dealCustomData(iSourceId, iCommandId, sParams, iDataLength, (const char *)pData, iTargetId);
}
//启动
void GCL_Trans_Protocol::protocolAfterStart()
{
    ProtocolCommon::protocolAfterStart();
}

void GCL_Trans_Protocol::protocolAfterStop()
{

    ProtocolCommon::protocolAfterStop();



}

void GCL_Trans_Protocol::doTask(GCCommon::GCDataElement dt)
{
    switch(dt.type())
    {
    case GCL_CMD_SYS_CLOCK://校时
        break;
    case GCL_CMD_SYS_INFO://信息
        break;
    default:
        break;
    }
}
void GCL_Trans_Protocol::clock()
{

}
//////////////////////////////////////////////////////////////////////////////////////////////////

