
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE
#DEFINES += GMAC_SHARE_MEMORY
#DEFINES += GM_TEST
#DEFINES += GM_TRACK
#PROJECT_TARGET_DEPLOY = 1


#LIBS += -L$$PWD/../../../deploy/win32/gcl_debug
LIBS += -lgcl_ccxx1
LIBS += -lgcl_script1

include($$PWD/../../gcl_environment.pri)

#script
include($$GCL_3RD_PATH/lua-5.3.4/src/lua.pri)

#control
include($$GCL_SRC_PATH/main/gcl_dispatch/gcl_dispatch.pri)
#通用服务
include($$GCL_SRC_PATH/vdi/vdi_common.pri)

TARGET = gcl_svr_dispatch
