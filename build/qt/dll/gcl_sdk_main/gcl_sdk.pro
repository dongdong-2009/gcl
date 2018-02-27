
DEFINES += PROJECT_TEMPLATE_LIB_DLL
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE

include($$PWD/../gcl_def.pri)

CONFIG(debug, debug|release) : DESTDIR = $$PWD/../../test/sdk_csharp/bin/Debug
CONFIG(release, debug|release) : DESTDIR = $$PWD/../../test/sdk_csharp/bin/Release
#DESTDIR = $$PWD/../../test/sdk_delphi
#DESTDIR = $$PWD/../../test/sdk_qt/bin
#CONFIG(debug, debug|release) : DESTDIR = $$PWD/../../../deploy/win32/ics_debug
#CONFIG(release, debug|release) : DESTDIR = $$PWD/../../../deploy/win32/ics_release

#CONFIG(debug, debug|release) {
#    TARGET = gcl_sdk_d
#} else {
#    TARGET = gcl_sdk
#}
#公共库
include($$GCL_SRC_PATH/common/common.pri)

include($$GCL_SRC_PATH/gccommon/gccommon.pri)

include($$GCL_SRC_PATH/protocol/psm_protocol.pri)

include($$GCL_SRC_PATH/vxd/ygct_std/sdk_client.pri)

include($$GCL_INCLUDE_PATH/ygct_sdk_inc.pri)

