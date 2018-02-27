#-------------------------------------------------
#
# Project created by QtCreator 2014-07-08T10:56:19
#
#-------------------------------------------------

#CCXX define

DEFINES += PROJECT_TEMPLATE_LIB_DLL

include($$PWD/../gcl_def.pri)

DEFINES += GC_DB_EXPORTS

TARGET = gcl_db_mssql

## oracle11g
include($$GCL_SRC_PATH/gcdb/gcl_db_mssql.pri)
## 库头文件
include($$GCL_INCLUDE_PATH/db_inc.pri)
