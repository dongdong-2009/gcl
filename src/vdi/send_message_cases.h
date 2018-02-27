#ifndef SEND_MESSAGE_CASES_H
#define SEND_MESSAGE_CASES_H

#include "cases_base.h"


struct SendMessageAttach
{
    int strategy;
    int level;
    int result;
    int tag;
    SendMessageAttach(int iStrategy = 0, int iLevel = 0, int iResult = 0, int iTag = 0) :
        strategy(iStrategy),level(iLevel),result(iResult),tag(iTag) {}
    inline SendMessageAttach(const SendMessageAttach &o) : strategy(o.strategy), level(o.level), result(o.result), tag(o.tag){ }
    inline SendMessageAttach &operator=(const SendMessageAttach &other)
    {
        if (this != &other)
        {
            strategy = other.strategy;
            level = other.level;
            result = other.result;
            tag = other.tag;
        }
        return *this;
    }
};


class IProtocolSendMessage
{
protected:
    virtual int post_message(const std::string& sCommand , const std::string & sParam) = 0;

    virtual int post_message(const std::string& sCommand , const std::string & sParam , SendMessageAttach & attach) { attach.result = 0; return 0; }

    friend class SendMessageCases;

};


class GM_GCL_VDI_API SendMessageCases : public CasesBase
{

    GM_TYPEFULLNAME(CasesBase, CS_CasesTypeSimpleName_SendMessage)

public:
    inline const IProtocolSendMessage * protocolSendMessage() { return _protocolSendMessage; }

    inline int postMessage(const std::string& sCommand , const std::string & sParam)
    {
        return _protocolSendMessage->post_message(sCommand, sParam);
    }

    inline int postMessage(const std::string& sCommand , const std::string & sParam , SendMessageAttach & attach)
    {
        return _protocolSendMessage->post_message(sCommand, sParam, attach);
    }

protected:
    inline void setProtocol(IProtocolSendMessage * oProtocol){ _protocolSendMessage = oProtocol; }

    friend class ProtocolBase;

private:
    IProtocolSendMessage * _protocolSendMessage;

};


#endif // SEND_MESSAGE_CASES_H
