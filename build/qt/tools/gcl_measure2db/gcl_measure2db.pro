
TARGET = gcl_tool_measure2db

#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)
#
include($$GCL_SRC_PATH/psm/psm.pri)
#
include($$GCL_SRC_PATH/tools/gcl_measure2db/gcl_measure2db.pri)

LIBS += -lgcl_ccxx1
LIBS += -lgcl_script1
LIBS += -lgcl_database_odbc1
LIBS += -lgcl_database_sqlite1
