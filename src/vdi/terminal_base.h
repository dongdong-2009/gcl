#ifndef TERMINAL_BASE_H
#define TERMINAL_BASE_H

#include "protocol_global.h"

#include "protocol_base.h"
#include "tbi_base.h"
#include "terminal_strings.h"


class TerminalBase;
class TerminalManager;


class TerminalFactoryBase : public CxFactoryTemplate<TerminalBase>
{

};


class GM_GCL_VDI_API TerminalBase
{
public:
    static std::vector<CxFactoryTemplate<TerminalBase> *> * factoriesContainer();

    static void factoriesCreateAndRegister();

    GM_TYPEFULLNAME_BASE( CS_Custom )

public:
    TerminalBase();

    virtual ~TerminalBase();

    inline int terminalId() const { return _terminalId; }
    inline void setTerminalId(int value);

    inline int remoteTerminalId() const { return _remoteTerminalId; }
    inline void setRemoteTerminalId(int value) { _remoteTerminalId = value; }

    virtual void fromContext(const CxIGetSkv& context);

    virtual void toContext(CxISetSkv& context) const;

    inline CxInterinfoOutStream outTerminalPrompt() {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Terminal); r.source(_terminalId); r << CxGlobal::promptString << typeSimpleName() << " Terminal : "; return r;
    }

#if defined(GM_DEBUG)
    inline CxInterinfoOutStream outTerminalDebug() {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Terminal); r.source(_terminalId); r << CxGlobal::debugString << typeSimpleName() << " Terminal : "; return r;
    }
#else
#undef outTerminalDebug
    inline CxInterinfoNoOutStream outTerminalDebug() { return CxInterinfoNoOutStream(); }
#define CX_NO_outTerminalDebug_MACRO while (false) outTerminalDebug
#define outTerminalDebug CX_NO_outTerminalDebug_MACRO
#endif

    inline ProtocolBase * protocol() { return _protocol; }
    inline void setProtocol(ProtocolBase* oProtocol) { _protocol = oProtocol; }

    inline TBIBase * business() { return _business; }
    inline void setBusiness(TBIBase* oBusiness) { _business = oBusiness; }

    inline const std::string& terminalName() const { return _terminalName; }
    inline void setTerminalName(const std::string& value){ _terminalName = value; }

    std::vector<std::string> reportSelf() const;

protected:
    virtual void toReport(std::vector<std::string>& sReports) const;

private:
    int _terminalId;
    int _remoteTerminalId;
    std::string _terminalName;
    ProtocolBase* _protocol;
    TBIBase* _business;

    friend class TerminalManager;
    friend class TerminalLoader;

};


class TerminalFactoryManager : public CxFactoryManagerTemplate<TerminalBase>
{
};


#endif // !defined(TERMINAL_BASE_H)
