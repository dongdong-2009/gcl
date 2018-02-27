
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/gcl_ga.h \
    $$PWD/ga_api.h

SOURCES += \
    $$PWD/gcl_ga.cpp \
    $$PWD/ga_api.cpp

contains(DEFINES, PROJECT_TEMPLATE_APP_CONSOLE) {
SOURCES += \
    $$PWD/main.cpp
}
