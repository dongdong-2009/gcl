
CCXX_PATH = $$PWD/../../../ccxx


GCL_PATH = $$PWD/../..
GCL_DEPLOY_PATH = $$PWD/../deploy
GCL_QTPRO_PATH = $$PWD
GCL_SRC_PATH = $$GCL_PATH/src
GCL_INCLUDE_PATH = $$GCL_PATH/include
GCL_3RD_PATH = $$GCL_PATH/3rd


INCLUDEPATH += $$CCXX_PATH
INCLUDEPATH += $$GCL_SRC_PATH
INCLUDEPATH += $$GCL_INCLUDE_PATH
INCLUDEPATH += $$GCL_3RD_PATH


PRECOMPILED_HEADER += $$PWD/gcl_version.h


QT       -= core
QT       -= gui

###*** c lang complier config
#QMAKE_CXXFLAGS += -static-libgcc
#QMAKE_LFLAGS   += -static-libgcc
#QMAKE_LFLAGS   += -static-libstdc++

#QMAKE_LFLAGS += -Wunused-variable
QMAKE_LFLAGS += -Wno-unused-parameter
#QMAKE_LFLAGS += -Wsign-compare
#QMAKE_LFLAGS += -Wno-sign-compare


###*** cpp lang complier config
#QMAKE_CXXFLAGS += -static-libgcc
#QMAKE_CXXFLAGS += -static-libstdc++

#QMAKE_CXXFLAGS += -Wunused-variable
QMAKE_CXXFLAGS += -Wno-unused-parameter
#QMAKE_CXXFLAGS += -Wsign-compare
#QMAKE_CXXFLAGS += -Wno-sign-compare

#1):define project template

#PROJECT_TEMPLATE_APP_GUI
#PROJECT_TEMPLATE_APP_CONSOLE
#PROJECT_TEMPLATE_LIB_DLL
#PROJECT_TEMPLATE_LIB_STATIC

! contains(DEFINES, PROJECT_TEMPLATE_APP_GUI) : ! contains(DEFINES, PROJECT_TEMPLATE_APP_CONSOLE) : ! contains(DEFINES, PROJECT_TEMPLATE_LIB_DLL) : ! contains(DEFINES, PROJECT_TEMPLATE_LIB_STATIC) : ! contains(DEFINES, PROJECT_TEMPLATE_SUBDIRS){
DEFINES += PROJECT_TEMPLATE_APP_CONSOLE
}

#2):define process

### project template ###

contains(DEFINES, PROJECT_TEMPLATE_APP_CONSOLE) {
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
DEFINES += GM_PROJECT_TYPE=1
}

contains(DEFINES, PROJECT_TEMPLATE_APP_GUI) {
TEMPLATE = app
DEFINES += PROJECT_3RDPARTY_QT
DEFINES += GM_PROJECT_TYPE=2
}

contains(DEFINES, PROJECT_TEMPLATE_LIB_DLL) {
TEMPLATE = lib
CONFIG += dll
DEFINES += GM_PROJECT_TYPE=3
}

contains(DEFINES, PROJECT_TEMPLATE_LIB_STATIC) {
TEMPLATE = lib
CONFIG += staticlib
DEFINES += GM_PROJECT_TYPE=4
}

contains(DEFINES, PROJECT_TEMPLATE_SUBDIRS) {
TEMPLATE = subdirs
}

CONFIG(debug, debug|release):DEFINES += GM_DEBUG


### path
#* var
#win32:!wince*:
#mac:!qpa:
#embedded:
#symbian:
#irix-cc*:
#unix|win32-g++*:
#x11:
#win32-msvc*|win32-icc:
#solaris-cc*:
#wince*:

#define project_bin_path : PROJECT_TARGET_DEPLOY = 1 or PROJECT_BIN_PATH = "$$PWD/../../bin

isEmpty( PROJECT_DEPLOY_DIR ) {
#    PROJECT_DEPLOY_DIR=ics_fes/gcl_fes
#    PROJECT_DEPLOY_DIR=ics_svr/gcl_svr
#    PROJECT_DEPLOY_DIR=ics_3rd/gcl_3rd
}

#isEmpty( PROJECT_DEPLOY_NAME ) {
#    CONFIG(debug, debug|release) : PROJECT_DEPLOY_NAME = gcl_debug/bin
#    CONFIG(release, debug|release) : PROJECT_DEPLOY_NAME = gcl_release/bin

#    ! contains(DEFINES, CCXX_QT) : ! contains(DEFINES, PROJECT_3RDPARTY_QT) {
#        CONFIG(debug, debug|release) : PROJECT_DEPLOY_NAME = $$PROJECT_DEPLOY_DIR/bin_d
#        CONFIG(release, debug|release) : PROJECT_DEPLOY_NAME = $$PROJECT_DEPLOY_DIR/bin
#    } else {
#        CONFIG(debug, debug|release) : PROJECT_DEPLOY_NAME = $$PROJECT_DEPLOY_DIR/bin_qt_d
#        CONFIG(release, debug|release) : PROJECT_DEPLOY_NAME = $$PROJECT_DEPLOY_DIR/bin_qt
#    }

#} else {
#    CONFIG(debug, debug|release) : PROJECT_DEPLOY_NAME = $$join(TARGET,,,_debug)
#    CONFIG(release, debug|release) : PROJECT_DEPLOY_NAME = $$join(TARGET,,,_release)
#}

win32 {
    CONFIG(debug, debug|release) : GCL_BIN_PATH = $$GCL_DEPLOY_PATH/$$PROJECT_DEPLOY_DIR/bin_win32_qt_d
    CONFIG(release, debug|release) : GCL_BIN_PATH = $$GCL_DEPLOY_PATH/$$PROJECT_DEPLOY_DIR/bin_win32_qt
    GCL_LIB_PATH = $$PWD/win32
    DEFINES += GCL_OS_WIN32
}
unix {
    CONFIG(debug, debug|release) : GCL_BIN_PATH = $$GCL_DEPLOY_PATH/$$PROJECT_DEPLOY_DIR/bin_unix_qt_d
    CONFIG(release, debug|release) : GCL_BIN_PATH = $$GCL_DEPLOY_PATH/$$PROJECT_DEPLOY_DIR/bin_unix_qt
    GCL_LIB_PATH = $$PWD/unix
    DEFINES += GCL_OS_UNIX
}
x11 {
    CONFIG(debug, debug|release) : GCL_BIN_PATH = $$GCL_DEPLOY_PATH/$$PROJECT_DEPLOY_DIR/bin_x11_qt_d
    CONFIG(release, debug|release) : GCL_BIN_PATH = $$GCL_DEPLOY_PATH/$$PROJECT_DEPLOY_DIR/bin_x11_qt
    GCL_LIB_PATH = $$PWD/x11
    DEFINES += GCL_OS_X11
}


#### project destdir ###
isEmpty( PROJECT_BIN_PATH ) {
    PROJECT_BIN_PATH = $$GCL_BIN_PATH
}
DESTDIR = $$PROJECT_BIN_PATH
LIBS += -L$$PROJECT_BIN_PATH


#### project version ###
include($$PWD/gcl_version.pri)
isEmpty( PROJECT_VERSION_MAJOR ) {
    PROJECT_VERSION_MAJOR = 1.2
}
isEmpty( PROJECT_VERSION_COMPILE ) {
    PROJECT_VERSION_COMPILE = 0
}
VERSION = $${PROJECT_VERSION_MAJOR}.$${PROJECT_VERSION_COMPILE}


isEmpty( PROJECT_LIB_PATH ) {
    PROJECT_LIB_PATH = $$GCL_LIB_PATH
}
LIBS += -L$$PROJECT_LIB_PATH


### project 3rdparty qt ###

#!contains(DEFINES, GCL_NO_CCXX)
#{
#    LIBS += -lgcl_ccxx
#    contains(DEFINES, PROJECT_3RDPARTY_QT) {
#        LIBS += -lgcl_ccxx_qt
#    } else {
#        LIBS += -lgcl_ccxx_standard1
#    }
#}

contains(DEFINES, PROJECT_3RDPARTY_QT)
{
    QT += core
    contains(DEFINES, PROJECT_TEMPLATE_APP_GUI)
    {
        QT += gui
        greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
        CODECFORTR = UTF-8
        CODECFORSRC = UTF-8
    }
}


### platform lib ###
win32: LIBS += -lwsock32 -lws2_32 -lodbc32 -lOle32
unix: LIBS += -ldl -lrt

