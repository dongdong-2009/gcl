TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

win32: LIBS += -lwsock32 -lws2_32 -lodbc32 -liphlpapi
unix: LIBS += -ldl -lrt

SOURCES += \
    main.cpp

