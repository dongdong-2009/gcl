#应用程序类型
#DEFINES += PROJECT_TEMPLATE_APP_CONSOLE

include($$PWD/../gcl_def.pri)
LIBS += -lgcl_script_vxd1
#modbus tcp 规约
include($$GCL_SRC_PATH/vxd/modbus/modbus_tcp.pri)

TARGET = vxd_modbus_tcp
