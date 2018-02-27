
DEFINES += PROJECT_TEMPLATE_LIB_DLL

include($$PWD/../../gcl_environment.pri)

LIBS += -lgcl_ccxx1

HEADERS += \
    NetMonitor.h

LIBS += -lWS2_32
