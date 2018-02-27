
DEFINES += PROJECT_TEMPLATE_LIB_DLL
CONFIG(debug, debug|release) : DEFINES += LPEG_DEBUG

#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/script/script_vxd.pri)
#lua
include($$GCL_3RD_PATH/lua-5.3.4/src/lua.pri)

LIBS += -lgcl_vdi1
LIBS += -lgcl_ccxx1
LIBS += -lgcl_database_sqlite1
LIBS += -lgcl_database_odbc1
#LIBS += -lgcl_omc1
#LIBS += -gcl_net_monitor1
#LIBS += -lgcl_lfs1
#LIBS += -lgcl_lpeg1
