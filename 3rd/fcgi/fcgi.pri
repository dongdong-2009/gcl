
INCLUDEPATH += $$PWD/include

HEADERS += \
    $$PWD/include/fastcgi.h \
    $$PWD/include/fcgi_config.h \
#    $$PWD/include/fcgi_config_x86.h \
    $$PWD/include/fcgi_stdio.h \
    $$PWD/include/fcgiapp.h \
    $$PWD/include/fcgimisc.h \
    $$PWD/include/fcgio.h \
    $$PWD/include/fcgios.h

SOURCES += \
    $$PWD/libfcgi/fcgio.cpp \
    $$PWD/libfcgi/fcgi_stdio.c \
    $$PWD/libfcgi/fcgiapp.c \

win32 {
    SOURCES += \
        $$PWD/libfcgi/os_win32.c
}
#unix {
#    SOURCES += \
#        $$PWD/libfcgi/os_unix.c
#}

