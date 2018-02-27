
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/calc_exec.h \
    $$PWD/math_parser.h \
    $$PWD/calc_measure.h \

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/calc_exec.cpp \
    $$PWD/math_parser.cpp \
    $$PWD/calc_measure.cpp \


###***gcl_config
SOURCES += \
    $$PWD/../../common/gcl_config.cpp \
