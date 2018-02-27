
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE
#DEFINES += GMAC_SHARE_MEMORY
#DEFINES += GM_TEST
#DEFINES += GM_TRACK
#PROJECT_TARGET_DEPLOY = 1


#LIBS += -L$$PWD/../../../deploy/win32/gcl_debug
LIBS += -lgcl_ccxx1
#LIBS += -lgcl_database_odbc1
LIBS += -lgcl_database_sqlite1
#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/psm/psm.pri)
#
include($$GCL_SRC_PATH/main/gcl_dbs/gcl_dbs.pri)
#通用服务
include($$GCL_SRC_PATH/vdi/vdi_common.pri)

TARGET = gcl_svr_dbs
