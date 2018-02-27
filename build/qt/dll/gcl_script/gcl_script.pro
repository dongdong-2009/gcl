
DEFINES += PROJECT_TEMPLATE_LIB_DLL
#DEFINES += PROJECT_TEMPLATE_LIB_STATIC
CONFIG(debug, debug|release) : DEFINES += LPEG_DEBUG

#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/script/script.pri)
#
include($$GCL_3RD_PATH/lua-5.3.4/src/lua.pri)

LIBS += -lgcl_ccxx1

