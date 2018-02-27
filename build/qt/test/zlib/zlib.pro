
DEFINES += PROJECT_TEMPLATE_APP_GUI

#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/ga/gcl_ga.pri)

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DESTDIR = $$PWD/../bin
DESTDIR = $$PWD/zlib

INCLUDEPATH += $$PWD/../../include
INCLUDEPATH += $$PWD/zlib/include

#LIBS += -L$$PWD/../bin
CONFIG(debug, debug|release) : LIBS += -L$$PWD/../../../deploy/win32/ics_svr/gcl_svr/bin_qt_d
CONFIG(release, debug|release) : LIBS += -L$$PWD/../../../deploy/win32/ics_svr/gcl_svr/bin_qt
LIBS += -lgcl_ccxx1

LIBS += -L$$PWD/zlib/lib
LIBS += -lzdll

FORMS += \
    mainwindow.ui

HEADERS += \
    mainwindow.h \

SOURCES += \
    main.cpp \
    mainwindow.cpp \

