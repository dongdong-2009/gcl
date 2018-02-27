
include($$PWD/../../gcl_environment.pri)

#
include($$GCL_SRC_PATH/psm/psm.pri)

include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)
#
include($$PWD/../../../../3rd/fcgi/fcgi.pri)
#
include($$PWD/../../../../3rd/cgicc/cgicc.pri)
#
include($$GCL_SRC_PATH/main/fastcgi/fastcgi.pri)
#
include($$GCL_SRC_PATH/vdi/vdi_common.pri)

LIBS += -lgcl_ccxx1
LIBS += -lgcl_database_odbc1
LIBS += -lgcl_database_sqlite1

TARGET = gcl_svr_fastcgi
