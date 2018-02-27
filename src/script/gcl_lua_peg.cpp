
#include "gcl_lua_peg.h"
#include "gcl_lua_global.h"
#include <luatoolbox/lpeg-0.12.2/lpeg.h>

GclLuaPeg * fn_oGclLuaPeg()
{
    GclLua::registInitFunction(GclLuaPeg::registerLua);
    static GclLuaPeg m;
    return &m;
}

static GclLuaPeg * f_oGclLuaPeg = fn_oGclLuaPeg();

void GclLuaPeg::registerLua()
{
    lua_State * L = GclLua::luaState();
    luaopen_lpeg(L);
}


GclLuaPeg::GclLuaPeg()
{
}
