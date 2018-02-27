
TARGET = gcl_file2db_v100

#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/psm/psm.pri)
#
include($$GCL_SRC_PATH/tools/file2db_v100/file2db_v100.pri)

include($$GCL_SRC_PATH/vdi/vdi_common.pri)

LIBS += -lgcl_ccxx1
LIBS += -lgcl_script1
LIBS += -lgcl_database_odbc1
LIBS += -lgcl_database_sqlite1

