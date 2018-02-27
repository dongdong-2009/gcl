#ifndef GCL_SCRIPT_GLOBAL_H
#define GCL_SCRIPT_GLOBAL_H


#include <ccxx/ccxx.h>


class GclScript
{
public:
    static inline CxInterinfoOutStream outScriptPrompt() {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::promptString << " Script : "; return r;
    }

#if defined(GM_DEBUG)
    static inline CxInterinfoOutStream outScriptDebug() {
        CxInterinfoOutStream r; r.noend(); r.type(CxInterinfo::Type_Script); r << CxGlobal::debugString << " Script : "; return r;
    }
#else
#undef outScriptDebug
    static inline CxInterinfoNoOutStream outScriptDebug() { return CxInterinfoNoOutStream(); }
#define CX_NO_outScriptDebug_MACRO while (false) outScriptDebug
#define outScriptDebug CX_NO_outScriptDebug_MACRO
#endif

};

#endif // GCL_SCRIPT_GLOBAL_H
