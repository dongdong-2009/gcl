
#应用程序类型
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE

#PROJECT_TARGET_DEPLOY = 1

#DEFINES += GMAC_SHARE_MEMORY

#OTL_ODBC
#LUA
DEFINES += OTL_ODBC


include($$PWD/../../gcl_environment.pri)
LIBS += -lgcl_vdi1
LIBS += -lgcl_ccxx1
LIBS += -lgcl_script_vxd1

#服务入口
include($$GCL_SRC_PATH/main/script/script_vxd_svr.pri)

TARGET = gcl_lua_host_vxd

