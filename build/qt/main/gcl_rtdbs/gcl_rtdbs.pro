
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE
#DEFINES += GMAC_SHARE_MEMORY
#DEFINES += GM_TEST
#DEFINES += GM_TRACK
#PROJECT_TARGET_DEPLOY = 1

#DEFINES += OTL_ODBC
#LIBS += -L$$PWD/../../../deploy/win32/gcl_debug
LIBS += -lgcl_ccxx1
#LIBS += -lgcl_vdi1
#LIBS += -lgcl_script_vxd1

#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/psm/psm.pri)
#
include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)
#
include($$GCL_SRC_PATH/main/gcl_rtdbs/gcl_rtdbs.pri)
#通用服务
include($$GCL_SRC_PATH/vdi/vdi_common.pri)

TARGET = gcl_svr_rtdbs
