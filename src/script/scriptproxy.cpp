#include "scriptproxy.h"
#include "gcl_lua_business.h"


using namespace std;


vector<IScriptProxyNotify*> f_scriptProxyNotifies;


void ScriptProxy::registerNotify(IScriptProxyNotify *oNotify)
{
    if (! CxContainer::contain(f_scriptProxyNotifies, oNotify))
        f_scriptProxyNotifies.push_back(oNotify);
}

void ScriptProxy::unregisterNotify(IScriptProxyNotify *oNotify)
{
    CxContainer::remove(f_scriptProxyNotifies, oNotify);
}

int ScriptProxy::postMessage(const string &sCommand, const string &sParam, const string &sSender, const ScriptProxyAttach *oAttach)
{
    if (oAttach != NULL)
    {
        return GclLuaBusiness::sendMessage(sSender, sCommand, sParam, ((ScriptProxyAttach *) oAttach)->toMap());
    }
    else
    {
        return GclLuaBusiness::sendMessage(sSender, sCommand, sParam);
    }
}


int ScriptProxy::postFileData(const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const string &sSender, const ScriptProxyAttach *oAttach)
{
    return -1;
}

int ScriptProxy::postFile(const string &sFilePath, const string &sSender, const ScriptProxyAttach *oAttach)
{
    return -1;
}

void ScriptProxy::raiseReceivedMessageCommand(const string &sCommand, const string &sParam, const string &sSender, const ScriptProxyAttach *oAttach)
{
    for (size_t i = 0; i < f_scriptProxyNotifies.size(); ++i)
    {
        IScriptProxyNotify * o = f_scriptProxyNotifies.at(i);
        o->received_message_command(sCommand, sParam, sSender, oAttach);
    }
}

void ScriptProxy::raiseReceivedFileData(const CxFileSystem::PathInfo &pathInfo, const std::vector<string> &fileData, const string &sSender, const ScriptProxyAttach *oAttach)
{
    for (size_t i = 0; i < f_scriptProxyNotifies.size(); ++i)
    {
        IScriptProxyNotify * o = f_scriptProxyNotifies.at(i);
        o->received_file_data(pathInfo, fileData, sSender, oAttach);
    }
}

void ScriptProxy::raiseReceivedFile(const string &sFilePath, const string &sSender, const ScriptProxyAttach *oAttach)
{
    for (size_t i = 0; i < f_scriptProxyNotifies.size(); ++i)
    {
        IScriptProxyNotify * o = f_scriptProxyNotifies.at(i);
        o->received_file(sFilePath, sSender, oAttach);
    }
}
