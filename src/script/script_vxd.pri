
LUA_PATH = $$PWD/../3party/lua5.3.0
LUA_SRC_PATH = $$LUA_PATH/src


INCLUDEPATH += $$LUA_SRC_PATH


HEADERS += \
    $$PWD/gcl_script_vxd.h \
    $$PWD/gcl_script_global.h \
    $$PWD/gcl_lua_global.h \
    $$PWD/gcl_lua_measure.h \
    $$PWD/gcl_lua_ga.h \
    $$PWD/gcl_lua_db.h \
    $$PWD/gcl_lua_vxd.h \
    $$PWD/gcl_lua_efile.h \
    $$PWD/gcl_lua_xml.h \
    $$PWD/gcl_lua_common.h \
    $$PWD/ccxx_lua_application.h \
    $$PWD/ccxx_lua_filesystem.h \
    $$PWD/ccxx_lua_time.h \
    $$PWD/gcl_lua_psm.h \
    $$PWD/gcl_lua_shm.h \
    $$PWD/gcl_lua_business.h \
    $$PWD/gcl_lua_base.h \
    $$PWD/scriptproxy.h \
    $$PWD/gcl_lua_fs.h \
    $$PWD/gcl_lua_peg.h \
    $$PWD/gcl_lua_code.h \
    $$PWD/ccxx_lua_tring.h \
    $$PWD/gcl_shm_server.h \
    $$PWD/gcl_measure_server.h

SOURCES += \
    $$PWD/gcl_script_vxd.cpp \
    $$PWD/gcl_lua_global.cpp \
    $$PWD/gcl_lua_measure.cpp \
    $$PWD/gcl_lua_ga.cpp \
    $$PWD/gcl_lua_db.cpp \
    $$PWD/gcl_lua_vxd.cpp \
    $$PWD/gcl_lua_efile.cpp \
    $$PWD/gcl_lua_xml.cpp \
    $$PWD/gcl_lua_common.cpp \
    $$PWD/ccxx_lua_application.cpp \
    $$PWD/ccxx_lua_filesystem.cpp \
    $$PWD/ccxx_lua_time.cpp \
    $$PWD/gcl_lua_psm.cpp \
    $$PWD/gcl_lua_shm.cpp \
    $$PWD/gcl_lua_business.cpp \
    $$PWD/gcl_lua_base.cpp \
    $$PWD/scriptproxy.cpp \
    $$PWD/gcl_lua_fs.cpp \
    $$PWD/gcl_lua_peg.cpp \
    $$PWD/gcl_lua_code.cpp \
    $$PWD/ccxx_lua_tring.cpp \
    $$PWD/gcl_shm_server.cpp \
    $$PWD/gcl_measure_server.cpp

