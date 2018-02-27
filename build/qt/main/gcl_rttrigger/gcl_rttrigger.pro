
#应用程序类型
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE

#PROJECT_TARGET_DEPLOY = 1

#DEFINES += GMAC_SHARE_MEMORY

#OTL_ODBC

DEFINES += OTL_ODBC

LIBS += -lgcl_ccxx1
LIBS += -lgcl_script1

include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/psm/psm.pri)
#
include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)
#服务入口
include($$GCL_SRC_PATH/main/gcl_rttrigger/gcl_rttrigger.pri)
#
include($$GCL_SRC_PATH/vdi/vdi_common.pri)

TARGET = gcl_rttrigger
