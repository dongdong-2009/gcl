
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
include($$GCL_SRC_PATH/tools/psm_kits/psm_kits.pri)

RESOURCES += \
    $$GCL_SRC_PATH/resource/images.qrc

RC_FILE = psm_kits.rc

TRANSLATIONS = psm_kits.ts

LIBS += -lgcl_ccxx1

