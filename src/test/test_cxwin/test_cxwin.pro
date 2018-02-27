
#应用程序类型
DEFINES += PROJECT_TEMPLATE_APP_GUI
#DEFINES += GM_TRACK
#PROJECT_TARGET_DEPLOY = 1


include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/test/test_cxwin/test_cxwin.pri)

RC_FILE = test_cxwin.rc

TRANSLATIONS = test_cxwin.ts

LIBS += -lgcl_ccxx1

TARGET = test_cxwin
