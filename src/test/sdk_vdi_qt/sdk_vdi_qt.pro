#-------------------------------------------------
#
# Project created by QtCreator 2014-06-26T18:11:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += GM_GCL_VDI

TARGET = sdk_vdi_qt
TEMPLATE = app

DESTDIR = $$PWD/../bin

INCLUDEPATH += $$PWD/../../include

LIBS += -L$$PWD/../bin
LIBS += -lgcl_sdk1

SOURCES += \
    $$PWD/../sdk_qt/main.cpp\
    $$PWD/../sdk_qt/sdk_test_win.cpp \
    $$PWD/../sdk_qt/sdk_measure_win.cpp \
    $$PWD/../sdk_qt/sdk_common.cpp \
    $$PWD/../sdk_qt/sdk_string.cpp \
    $$PWD/../sdk_qt/sdk_time.cpp \

HEADERS  += \
    $$PWD/../sdk_qt/sdk_test_win.h \
    $$PWD/../sdk_qt/sdk_measure_win.h \
    $$PWD/../sdk_qt/sdk_common.h \
    $$PWD/../sdk_qt/sdk_string.h \
    $$PWD/../sdk_qt/sdk_global.h \
    $$PWD/../sdk_qt/sdk_time.h \

FORMS    += \
    $$PWD/../sdk_qt/sdktestwin.ui \
