#include "gcl_script_vxd.h"


#include <ccxx/cxdatabase_sqlite.h>
#include <ccxx/cxdatabase_odbc.h>



void GclScriptVxd::init()
{
    GclLua::start();
}

void GclScriptVxd::uninit()
{
    GclLua::stop();
}
