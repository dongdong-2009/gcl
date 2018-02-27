#-------------------------------------------------
#
# Project created by QtCreator 2014-07-08T10:49:26
#
#-------------------------------------------------
#CCXX define

DEFINES += PROJECT_TEMPLATE_LIB_DLL

include($$PWD/../gcl_def.pri)

DEFINES += GC_DB_EXPORTS

TARGET = gcl_db_oracle11g

## oracle11g
include($$GCL_SRC_PATH/gcdb/gcl_db_oracle11g.pri)
## 库头文件
include($$GCL_INCLUDE_PATH/db_inc.pri)
