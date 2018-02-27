
#应用程序类型
DEFINES += PROJECT_TEMPLATE_APP_GUI
#DEFINES += GM_TRACK
#PROJECT_TARGET_DEPLOY = 1


include($$PWD/../../gcl_environment.pri)
#
include($$CCXX_PATH/ccxx/ccxx_qt.pri)
#
include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)
#
include($$GCL_SRC_PATH/tools/gcl_sharememory_viewer/gcl_sharememory_viewer.pri)

RC_FILE = gcl_sharememory_viewer.rc

TRANSLATIONS = sharememory_viewer.ts

LIBS += -lgcl_ccxx1

TARGET = gcl_tool_sharememory_viewer
