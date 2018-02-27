
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/bus_terminal.h \
    $$PWD/bus_data_deal.h \
    $$PWD/bus_measure_publish.h \
    $$PWD/bus_strings.h \
    $$PWD/bus_measure_control.h \
    $$PWD/bus_general_access.h \

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/bus_terminal.cpp \
    $$PWD/bus_data_deal.cpp \
    $$PWD/bus_measure_publish.cpp \
    $$PWD/bus_measure_control.cpp \
    $$PWD/bus_general_access.cpp \


HEADERS += \
    $$PWD/../../vdi/psm_command.h \
    $$PWD/../../psm/psm_explain.h \

SOURCES += \
    $$PWD/../../psm/psm_explain.cpp \
    $$PWD/../../common/gcl_config.cpp \

DISTFILES += \
    $$PWD/readme.txt

