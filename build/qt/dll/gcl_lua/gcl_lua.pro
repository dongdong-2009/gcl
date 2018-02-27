#-------------------------------------------------
#
# Project created by QtCreator 2014-07-19T00:19:58
#
#-------------------------------------------------

#CCXX define

DEFINES += PROJECT_TEMPLATE_LIB_DLL
#DEFINES += PROJECT_TEMPLATE_LIB_STATIC
DEFINES += LUA_BUILD_AS_DLL
DEFINES += LUA_LIB
#DEFINES += GCL_NO_CCXX

TARGET = gcl_lua_5.3.0
#TARGET = gcl_lua_5.1.5

LIBS += -lgcl_ccxx1

include($$PWD/../../gcl_environment.pri)

include($$GCL_3RD_PATH/lua-5.3.4/src/lua.pri)
