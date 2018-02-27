#-------------------------------------------------
#
# Project created by QtCreator 2014-02-13T09:37:29
#
#-------------------------------------------------

include($$PWD/../ICS.pri)

QT       += xml

TARGET = graphics
TEMPLATE = lib

DEFINES += GCL_GRAPHICS_LIBRARY

SOURCES += \
    gritemdatainterface.cpp \
    grglobalparameter.cpp \
    grarcitemdata.cpp \
    grbezieritemdata.cpp \
    grbussitemdata.cpp \
    fillstyle.cpp \
    grpolylineitemdata.cpp \
    grcompositefigure.cpp \
    groupstate.cpp \
    grconnectlineitemdata.cpp \
    grconnectpointitemdata.cpp \
    grdeviceitemdata.cpp \
    grellipseitemdata.cpp \
    grlineitemdata.cpp \
    grpolygonitemdata.cpp \
    grrectangleitemdata.cpp \
    grtextitemdata.cpp \
    grdomparser.cpp \
    grpage.cpp

HEADERS += \
        graphics_global.h \
    gritemdatainterface.h \
    grglobalparameter.h \
    grarcitemdata.h \
    grbezieritemdata.h \
    grbussitemdata.h \
    fillstyle.h \
    grpolylineitemdata.h \
    grcompositefigure.h \
    groupstate.h \
    grconnectlineitemdata.h \
    grconnectpointitemdata.h \
    grdeviceitemdata.h \
    grellipseitemdata.h \
    grlineitemdata.h \
    grpolygonitemdata.h \
    grrectangleitemdata.h \
    grtextitemdata.h \
    grdomparser.h \
    grpage.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
