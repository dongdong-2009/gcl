
#应用程序类型
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE

include($$PWD/../gcl_def.pri)
#modbus tcp 规约
include($$GCL_SRC_PATH/vxd/vxd_frame/vxd_frame.pri)

TARGET = vxd_frame
