
DEFINES += PROJECT_TEMPLATE_LIB_DLL
DEFINES += GCI_API_IMPL_EXPORT

LIBS += -lgcl_vdi1
LIBS += -lgcl_ccxx1

include($$PWD/../../gcl_environment.pri)

#CONFIG(debug, debug|release) : DESTDIR = $$PWD/../../test/sdk_csharp/bin/Debug
#CONFIG(release, debug|release) : DESTDIR = $$PWD/../../test/sdk_csharp/bin/Release

#DESTDIR = $$PWD/../../test/sdk_delphi
#DESTDIR = $$PWD/../../test/sdk_qt/bin
#DESTDIR = $$PWD/../../test/sdk_asm570/bin
#CONFIG(debug, debug|release) : DESTDIR = $$PWD/../../../deploy/win32/ics_debug
#CONFIG(release, debug|release) : DESTDIR = $$PWD/../../../deploy/win32/ics_release
#CONFIG(debug, debug|release) : DESTDIR = $$PWD/../../../deploy/win32/kits_debug
#CONFIG(release, debug|release) : DESTDIR = $$PWD/../../../deploy/win32/kits_release

#CONFIG(debug, debug|release) {
#    TARGET = gcl_sdk_d
#} else {
#    TARGET = gcl_sdk
#}

#
include($$GCL_SRC_PATH/vxd/ygct_std/sdk_client.pri)
#
include($$GCL_INCLUDE_PATH/ygct_sdk_inc.pri)
