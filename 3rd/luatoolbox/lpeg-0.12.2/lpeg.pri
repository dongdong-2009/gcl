#LUA_TOOL_BOX_PATH = $$PWD/../3party/luatoolbox
#LUA_LPEG_PATH = $$LUA_PATH/lpeg-0.12.2

#INCLUDEPATH += $$LUA_LPEG_PATH

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/lptree.h \
    $$PWD/lpcap.h \
    $$PWD/lpcode.h \
    $$PWD/lpprint.h \
    $$PWD/lptypes.h \
    $$PWD/lpvm.h \
    $$PWD/lpeg.h

SOURCES += \
    $$PWD/lptree.c \
    $$PWD/lpcap.c \
    $$PWD/lpcode.c \
    $$PWD/lpprint.c \
    $$PWD/lpvm.c

