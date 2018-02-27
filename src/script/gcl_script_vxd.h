#ifndef GCL_SCRIPT_H
#define GCL_SCRIPT_H


#include "gcl_script_global.h"
#include "gcl_lua_global.h"
#include "gcl_lua_common.h"
#include "gcl_lua_db.h"
#include "gcl_lua_efile.h"
#include "gcl_lua_vxd.h"
#include "gcl_lua_ga.h"
#include "gcl_lua_xml.h"
#include "gcl_lua_measure.h"
#include "gcl_lua_psm.h"
#include "gcl_lua_shm.h"
#include "gcl_lua_business.h"
#include "scriptproxy.h"
#include "gcl_lua_fs.h"
#include "gcl_lua_peg.h"

class GclScriptVxd
{
public:
    static void init();

    static void uninit();

};


#endif // GCL_SCRIPT_H
