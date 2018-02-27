#应用程序类型
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE

include($$PWD/../gcl_def.pri)

#通讯协议 104
include($$GCL_SRC_PATH/vxd/iec104/iec104.pri)

TARGET = vxd_iec104
