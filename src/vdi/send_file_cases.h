#ifndef SEND_FILE_CASES_H
#define SEND_FILE_CASES_H

#include "cases_base.h"


struct SendFileAttach
{
    int strategy;
    int level;
    int result;
    int tag;
    SendFileAttach(int iStrategy = 0, int iLevel = 0, int iResult = 0, int iTag = 0) :
        strategy(iStrategy),level(iLevel),result(iResult),tag(iTag) {}
    inline SendFileAttach(const SendFileAttach &o) : strategy(o.strategy), level(o.level), result(o.result), tag(o.tag){ }
    inline SendFileAttach &operator=(const SendFileAttach &other)
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


class IProtocolSendFile
{
protected:
    virtual int post_file(const std::string& sFilePath) = 0;

    virtual int post_file(const std::string& sFilePath , SendFileAttach & attach) { attach.result = 0; return 0; }

    friend class SendFileCases;

};


class GM_GCL_VDI_API SendFileCases : public CasesBase
{

    GM_TYPEFULLNAME(CasesBase, CS_CasesTypeSimpleName_SendFile)

public:
    inline const IProtocolSendFile * protocolSendFile() { return _protocolSendFile; }

    inline int postFile(const std::string& sFilePath)
    {
        return _protocolSendFile->post_file(sFilePath);
    }

    inline int postFile(const std::string& sFilePath, SendFileAttach & attach)
    {
        return _protocolSendFile->post_file(sFilePath, attach);
    }

protected:
    inline void setProtocol(IProtocolSendFile * oProtocol){ _protocolSendFile = oProtocol; }

    friend class ProtocolBase;

private:
    IProtocolSendFile * _protocolSendFile;

};


#endif // SEND_FILE_CASES_H
