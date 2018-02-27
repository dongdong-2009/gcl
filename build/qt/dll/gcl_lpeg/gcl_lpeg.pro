#-------------------------------------------------
#
# Project created by QtCreator 2015-06-01T21:57:46
#
#-------------------------------------------------

DEFINES += PROJECT_TEMPLATE_LIB_DLL


CONFIG(debug, debug|release) : DEFINES += LPEG_DEBUG



include($$PWD/../../gcl_environment.pri)

#lua
include($$GCL_3RD_PATH/lua-5.3.4/src/lua.pri)
#
include($$GCL_3RD_PATH/luatoolbox/lpeg-0.12.2/lpeg.pri)
