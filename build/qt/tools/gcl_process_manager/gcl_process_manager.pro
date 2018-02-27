
#应用程序类型
DEFINES += PROJECT_TEMPLATE_APP_GUI
#DEFINES += GM_TRACK
#PROJECT_TARGET_DEPLOY = 1

include($$PWD/../../gcl_environment.pri)

include($$GCL_SRC_PATH/tools/gcl_process_manager/gcl_process_manager.pri)

win32: LIBS += -lwsock32 -lws2_32 -lodbc32 -lPsapi
LIBS += -lgcl_ccxx1


OTHER_FILES += \
    config.xml \
    ReadMe.ini

RESOURCES += \
    $$GCL_SRC_PATH/resource/images.qrc

RC_FILE = gcl_process_manager.rc

