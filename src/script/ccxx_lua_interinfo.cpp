#include "ccxx_lua_interinfo.h"


using namespace std;

CxLuaInterInfo * fn_oCxLuaInterInfo()
{
    GclLua::registInitFunction(CxLuaInterInfo::registerLua);
    static CxLuaInterInfo m;
    return &m;
}

static CxLuaInterInfo * f_oCxLuaInterInfo = fn_oCxLuaInterInfo();




void CxLuaInterInfo::registerLua()
{

}

CxLuaInterInfo::CxLuaInterInfo()
{
}
