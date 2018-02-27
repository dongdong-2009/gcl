
DEFINES += PROJECT_TEMPLATE_LIB_DLL

#
include($$PWD/../../gcl_environment.pri)

TARGET = gcl_database_odbc

DEFINES += GM_BUILD_CCXX_ODBC_LIB

#
include($$CCXX_PATH/ccxx/ccxx_odbc.pri)

LIBS += -lgcl_ccxx1
