
#应用程序类型
DEFINES += PROJECT_TEMPLATE_APP_GUI
#DEFINES += GM_TRACK
#PROJECT_TARGET_DEPLOY = 1


include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/tools/user_tool/user_tool.pri)

RC_FILE = user_tool.rc

LIBS += -lgcl_ccxx1

TARGET = gcl_user_tool
