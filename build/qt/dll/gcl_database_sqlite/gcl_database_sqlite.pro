
DEFINES += PROJECT_TEMPLATE_LIB_DLL

#
include($$PWD/../../gcl_environment.pri)

TARGET = gcl_database_sqlite

DEFINES += GM_BUILD_CCXX_SQLITE_LIB

#
include($$CCXX_PATH/ccxx/ccxx_sqlite3.pri)

LIBS += -lgcl_ccxx1
