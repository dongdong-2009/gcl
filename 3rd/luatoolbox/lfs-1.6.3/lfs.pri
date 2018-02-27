#LUA_TOOL_BOX_PATH = $$GCL_3RD_PATH/luatoolbox
#LUA_LFC_PATH = $$LUA_PATH/lfs-1.6.3

#INCLUDEPATH += $$LUA_LFC_PATH
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/lfs.h \


SOURCES += \
    $$PWD/lfs.c \


