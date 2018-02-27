
LIBS += -lgcl_ccxx1
LIBS += -lgcl_script1

DEFINES += PROJECT_3RDPARTY_QT
#DEFINES += RTDCLIENT_API

contains(DEFINES, RTDCLIENT_API) {
LIBS += -lygct_rtd_client
}


#
include($$PWD/../../gcl_environment.pri)
#
include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)
#
include($$GCL_SRC_PATH/psm/psm.pri)
#
include($$GCL_SRC_PATH/tools/gcl_measure23rd/gcl_measure23rd.pri)
#通用服务
include($$GCL_SRC_PATH/vdi/vdi_common.pri)

TARGET = gcl_tool_measure23rd
