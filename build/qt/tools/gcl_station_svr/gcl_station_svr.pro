
LIBS += -lgcl_ccxx1
LIBS += -lgcl_script1

#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/psm/psm.pri)
#
include($$GCL_SRC_PATH/tools/gcl_station_svr/gcl_station_svr.pri)

LIBS += -lpsapi

TARGET = gcl_svr_station
