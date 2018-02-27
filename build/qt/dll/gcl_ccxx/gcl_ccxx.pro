
DEFINES += PROJECT_TEMPLATE_LIB_DLL
#DEFINES += PROJECT_TEMPLATE_LIB_STATIC
#DEFINES += GCL_NO_CCXX

#
include($$PWD/../../gcl_environment.pri)

TARGET = gcl_ccxx

DEFINES += GM_BUILD_CCXX_CORE_LIB

#
include($$CCXX_PATH/ccxx/ccxx.pri)
