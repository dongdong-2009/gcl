
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/ntp_server.h \

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/ntp_server.cpp \


HEADERS += \
    $$PWD/../../vdi/psm_command.h \
    $$PWD/../../psm/psm_explain.h \

SOURCES += \
    $$PWD/../../psm/psm_explain.cpp \
    $$PWD/../../common/gcl_config.cpp \

DISTFILES += \
    $$PWD/readme.txt

