
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

#
include($$PWD/../../gcl_environment.pri)
#
include($$CCXX_PATH/ccxx/ccxx_qt.pri)
#
include($$GCL_SRC_PATH/tools/gcl_configself/gcl_configself.pri)

RESOURCES += \
    $$GCL_SRC_PATH/resource/images.qrc

RC_FILE = gcl_config.rc

TRANSLATIONS = configself.ts

LIBS += -lgcl_ccxx1

DEFINES += GM_PI_SettingPlugin
#DEFINES += GM_PI_TerminalPlugin
#DEFINES += GM_PI_CreatorPlugin

TARGET = gcl_gcl_config_tool
