#应用程序类型
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE

#PROJECT_TARGET_DEPLOY = 1

#DEFINES += GMAC_SHARE_MEMORY

#OTL_ODBC
#LUA
DEFINES += OTL_ODBC


include($$PWD/../../gcl_environment.pri)
#服务入口
include($$GCL_SRC_PATH/main/script/script_svr.pri)

LIBS += -lgcl_ccxx1
LIBS += -lgcl_script1


TARGET = gcl_lua_host

#通用服务
include($$GCL_SRC_PATH/vdi/vdi_common.pri)
