#-------------------------------------------------
#
# Project created by QtCreator 2017-02-27T09:28:55
#
#-------------------------------------------------

#QT       += core gui

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#TARGET = cyg_tools
#TEMPLATE = app

#QMAKE_CXXFLAGS += -std=c++0x

#SOURCES += main.cpp\
#        mainwindow.cpp

#HEADERS  += mainwindow.h \
#    newspaper.h \
#    read.h

#FORMS    += mainwindow.ui \
#    cygtool.ui

#DISTFILES +=

###############################################################
#应用程序类型
DEFINES += PROJECT_TEMPLATE_APP_GUI
#DEFINES += GM_TRACK
#PROJECT_TARGET_DEPLOY = 1


#win32 {
#PROJECT_BIN_PATH = $$PWD/../../../deploy/win32/gcl_qt_debug
#}
#unix {
#PROJECT_BIN_PATH = $$PWD/../../../deploy/win32/gcl_qt_debug
#}
#x11 {
#PROJECT_BIN_PATH = $$PWD/../../../deploy/win32/gcl_qt_debug
#}


#LIBS += -L$$PWD/../../../deploy/win32/gcl_qt_debug

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/tools/gcl_cygtools/gcl_cygtools.pri)

RESOURCES += \
    $$GCL_SRC_PATH/resource/images.qrc

#RC_FILE = gcl_cygtools.rc

#TRANSLATIONS = configself.ts

LIBS += -lgcl_ccxx1

#DEFINES += GM_PI_SettingPlugin
#DEFINES += GM_PI_TerminalPlugin
DEFINES += GM_PI_CreatorPlugin

TARGET = gcl_cygtools
