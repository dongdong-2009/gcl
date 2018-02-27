
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/gcl_lua_dispatch.h \

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/gcl_lua_dispatch.cpp \

###*** script.ga
LUA_PATH = $$PWD/../3party/lua5.3.0
LUA_SRC_PATH = $$LUA_PATH/src
INCLUDEPATH += $$LUA_SRC_PATH

HEADERS += \
    $$PWD/../../script/gcl_lua_ga.h \

SOURCES += \
    $$PWD/../../script/gcl_lua_ga.cpp \

###***gcl_config
SOURCES += \
    $$PWD/../../common/gcl_config.cpp \

#
include($$GCL_SRC_PATH/psm/psm.pri)
#
include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)
