
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE
#DEFINES += GMAC_SHARE_MEMORY
#DEFINES += GM_TEST
DEFINES += GM_TRACK_VALUE=0x01010001
#PROJECT_TARGET_DEPLOY = 1


#LIBS += -L$$PWD/../../../deploy/win32/gcl_debug
LIBS += -lgcl_ccxx1
#LIBS += -lgcl_database_odbc1
#LIBS += -lgcl_database_sqlite1
#LIBS += -lgcl_script1

#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)
#
include($$GCL_SRC_PATH/main/gcl_bus/gcl_bus.pri)

include($$GCL_SRC_PATH/vdi/vdi_common.pri)

TARGET = gcl_svr_bus
