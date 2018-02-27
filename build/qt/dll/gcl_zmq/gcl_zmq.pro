#-------------------------------------------------
#
# Project created by QtCreator 2014-07-27T08:27:59
#
#-------------------------------------------------

#QT       -= core gui

#TARGET = gcl_zmq
#TEMPLATE = lib

#DEFINES += GCL_ZMQ_LIBRARY

#SOURCES += gcl_zmq.cpp

#HEADERS += gcl_zmq.h\
#        gcl_zmq_global.h

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
DEFINES += PROJECT_TEMPLATE_LIB_DLL
DEFINES += LUA_BUILD_AS_DLL

include($$PWD/../gcl_def.pri)

#DEFINES += GC_RTSHM_EXPORTS

TARGET = gcl_zmq
#TARGET = gcl_lua_5.3.0

#include($$GCL_SRC_PATH/3party/lua-5.1.5/src/lua.pri)
include($$CCXX_PATH/ccxx/lua-5.3.4/src/lua.pri)
