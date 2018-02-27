#ifndef SCRIPTPROXY_H
#define SCRIPTPROXY_H


#include <ccxx/ccxx.h>


struct ScriptProxyAttach
{
    int reason;
    int containerId;
    int sourceId;
    int targetId;
    int tag;

    ScriptProxyAttach(int iReason = 0, int iContainerId = 0, int iSourceID = 0 , int iTargetID = 0, int iTag = 0) :
        reason(iReason),containerId(iContainerId),sourceId(iSourceID),targetId(iTargetID),tag(iTag) {}
    inline ScriptProxyAttach(const ScriptProxyAttach &o) : reason(o.reason), containerId(o.containerId), sourceId(o.sourceId), targetId(o.targetId), tag(o.tag){ }
    inline ScriptProxyAttach &operator=(const ScriptProxyAttach &other)
    {
        if (this != &other)
        {
            reason = other.reason;
            containerId = other.containerId;
            sourceId = other.sourceId;
            targetId = other.targetId;
            tag = other.tag;
        }
        return *this;
    }
    inline std::string toString()
    {
        return CxString::format("reason=%d;containerId=%d;sourceId=%d;targetId=%d;tag=%d",reason,containerId,sourceId,targetId,tag);
    }
    inline std::map<std::string,std::string> toMap()
    {
        return CxString::splitToMap(toString(),CxGlobal::middleChar,CxGlobal::splitChar);
    }

    void fromString(const std::string &data)
    {
        std::map<std::string,std::string> map1 = CxString::splitToMap(data,CxGlobal::middleChar,CxGlobal::splitChar);
        fromString(map1);
    }

    void fromString(std::map<std::string,std::string> &map1)
    {
        if(map1.size()<1) return;
        std::string sKey = "reason";
        std::string sDefVal="0";

        reason      = CxString::toInt32(CxContainer::value(map1,sKey,sDefVal));
        sKey = "containerId";
        containerId = CxString::toInt32(CxContainer::value(map1,sKey,sDefVal));
        sKey = "sourceId";
        sourceId    = CxString::toInt32(CxContainer::value(map1,sKey,sDefVal));
        sKey = "targetId";
        targetId    = CxString::toInt32(CxContainer::value(map1,sKey,sDefVal));
        sKey = "tag";
        tag         = CxString::toInt32(CxContainer::value(map1,sKey,sDefVal));
    }
};


class IScriptProxyNotify
{
protected:
    virtual void received_message_command(const std::string & sCommand, const std::string & sParam, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL) = 0;

    virtual void received_file_data(const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL) = 0;

    virtual void received_file(const std::string & sFilePath, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL) = 0;

    friend class ScriptProxy;

};


class ScriptProxy
{
public:
    static void start();

    static void stop();

    //注册通知
    static void registerNotify(IScriptProxyNotify * oNotify);

    static void unregisterNotify(IScriptProxyNotify * oNotify);
public:
    static int postMessage(const std::string & sCommand, const std::string & sParam, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL);

    static int postFileData(const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL);

    static int postFile(const std::string & sFilePath, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL);

public:
    static void raiseReceivedMessageCommand(const std::string & sCommand, const std::string & sParam, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL);

    static void raiseReceivedFileData(const CxFileSystem::PathInfo & pathInfo, const std::vector<std::string> & fileData, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL);

    static void raiseReceivedFile(const std::string & sFilePath, const std::string & sSender, const ScriptProxyAttach * oAttach = NULL);

};


#endif // SCRIPTPROXY_H
