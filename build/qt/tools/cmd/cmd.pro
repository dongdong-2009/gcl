#-------------------------------------------------
#
# Project created by QtCreator 2015-02-11T09:55:20
#
#-------------------------------------------------

#QT       -= core gui

#TARGET = cmd
#TEMPLATE = lib

#DEFINES += CMD_LIBRARY

#SOURCES += cmd.cpp

#HEADERS += cmd.h

DEFINES += PROJECT_TEMPLATE_SUBDIRS

include($$PWD/../../gcl_environment.pri)

OTHER_FILES += \
    $$PWD/cmd.bat \
    $$PWD/cmd_debug.bat \
    $$PWD/cmd_release.bat \
    $$PWD/cmd.sh \

unix {
    message("sh cmd.sh begin...")
    system(sudo sh cmd.sh $$PROJECT_BIN_PATH)
}

win32 | win64 {
    message("sh cmd.bat begin...")
    CONFIG(debug, debug|release) : system(cmd cmd_debug.bat $$PROJECT_BIN_PATH)
    CONFIG(release, debug|release) : system(cmd cmd_release.bat $$PROJECT_BIN_PATH)
}
