
#应用程序类型
DEFINES += PROJECT_TEMPLATE_APP_GUI
#DEFINES += GM_TRACK
#PROJECT_TARGET_DEPLOY = 1

#
include($$PWD/../../gcl_environment.pri)
#
include($$CCXX_PATH/ccxx/ccxx_qt.pri)
#
include($$GCL_SRC_PATH/tools/gcl_file_deploy_manager/gcl_file_deploy_manager.pri)

RESOURCES += \
    $$GCL_SRC_PATH/resource/images.qrc

RC_FILE = gcl_file_deploy_manager.rc

TRANSLATIONS = station_manager.ts

LIBS += -lgcl_ccxx1

DEFINES += GM_PI_TerminalPlugin

TARGET = gcl_station_manager
