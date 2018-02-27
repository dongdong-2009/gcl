
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


include($$PWD/../../gcl_environment.pri)

#所支持的规约
include($$GCL_SRC_PATH/protocol/protocol.pri)

#所支持的业务
include($$GCL_SRC_PATH/business/business.pri)

#
include($$GCL_SRC_PATH/qtwincontrol/qtwincontrol.pri)

#
SOURCES += \
    $$GCL_SRC_PATH/tools/gcl_configurator/main.cpp \
    $$GCL_SRC_PATH/tools/gcl_configurator/configurator_win.cpp \

HEADERS  += \
    $$GCL_SRC_PATH/tools/gcl_configurator/configurator_win.h \

RESOURCES += \
    $$GCL_SRC_PATH/resource/images.qrc

FORMS += \
    $$GCL_SRC_PATH/tools/gcl_configurator/configurator_win.ui \

RC_FILE = gcl_configurator.rc

TRANSLATIONS = configurator.ts

LIBS += -lgcl_vdi1
LIBS += -lgcl_ccxx1

