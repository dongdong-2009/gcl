#-------------------------------------------------
#
# Project created by QtCreator 2014-06-26T18:11:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DEFINES += GM_GCL_GA

TEMPLATE = app

INCLUDEPATH += $$PWD/../../../../include

MY_DEPLOY_PATH = $$PWD/../../../deploy

CONFIG(debug, debug|release) : MY_DESTDIR_PATH = $$MY_DEPLOY_PATH/bin_win32_qt_d
CONFIG(release, debug|release) : MY_DESTDIR_PATH = $$MY_DEPLOY_PATH/bin_win32_qt

DESTDIR = $$MY_DESTDIR_PATH
LIBS += -L$$MY_DESTDIR_PATH

message($$MY_DESTDIR_PATH)

SOURCES += \
    $$PWD/../sdk_qt/main.cpp\
    $$PWD/../sdk_qt/sdk_test_win.cpp \
    $$PWD/../sdk_qt/sdk_measure_win.cpp \
    $$PWD/../sdk_qt/sdk_common.cpp \
    $$PWD/../sdk_qt/cxglobal.cpp \
    $$PWD/../sdk_qt/cxstring.cpp \
    $$PWD/../sdk_qt/cxtime.cpp \
    $$PWD/../sdk_qt/cxfile.cpp \
    $$PWD/../sdk_qt/cxcrc.cpp \

HEADERS  += \
    $$PWD/../sdk_qt/sdk_test_win.h \
    $$PWD/../sdk_qt/sdk_measure_win.h \
    $$PWD/../sdk_qt/cxglobal.h \
    $$PWD/../sdk_qt/cxstring.h \
    $$PWD/../sdk_qt/cxtime.h \
    $$PWD/../sdk_qt/cxfile.h \
    $$PWD/../sdk_qt/cxcrc.h \
    $$PWD/../sdk_qt/sdk_common.h \

FORMS    += \
    $$PWD/../sdk_qt/sdktestwin.ui \

RC_FILE = sdk_ga_qt.rc

TRANSLATIONS = sdk_ga_qt.ts

LIBS += -lgcl_sdk1

TARGET = gcl_tool_sdk

DEFINES += GM_GCL_GA
