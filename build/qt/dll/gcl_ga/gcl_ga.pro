
DEFINES += PROJECT_TEMPLATE_LIB_DLL
DEFINES += GCI_API_IMPL_EXPORT

#
include($$PWD/../../gcl_environment.pri)
#
include($$CCXX_PATH/ccxx/ccxx.pri)
#
include($$GCL_SRC_PATH/ga/gcl_ga.pri)
#
include($$GCL_SRC_PATH/psm/psm.pri)
#
include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)

##*** c lang and c++ complier config
QMAKE_LFLAGS   += -static-libgcc
QMAKE_LFLAGS   += -static-libstdc++
QMAKE_CXXFLAGS += -static-libgcc
QMAKE_CXXFLAGS += -static-libstdc++


#CONFIG(debug, debug|release) : DESTDIR = $$PWD/../../../deploy/win32/ics_svr/gcl_svr/bin_qt_d
#CONFIG(release, debug|release) : DESTDIR = $$PWD/../../../deploy/win32/ics_svr/gcl_svr/bin_qt
#CONFIG(debug, debug|release) : DESTDIR = $$PWD/../../test/sdk_csharp/bin/Debug
#CONFIG(release, debug|release) : DESTDIR = $$PWD/../../test/sdk_csharp/bin/Release
#DESTDIR = $$PWD/../../test/bin
#DESTDIR = $$PWD/../../test/sdk_delphi
#DESTDIR = $$PWD/../../test/sdk_qt/bin

#CONFIG(debug, debug|release) {
#    TARGET = gcl_sdk_d
#} else {
#    TARGET = gcl_sdk
#}

TARGET = gcl_sdk

##
#LIBS += -lgcl_ccxx1

