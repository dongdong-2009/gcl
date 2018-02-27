
DEFINES += GMAC_SHARE_MEMORY

LIBS += -lgcl_vdi1
LIBS += -lgcl_ccxx1
LIBS += -lgcl_script_vxd1

include($$PWD/../gcl_environment.pri)

#虚拟设备驱动
include($$GCL_SRC_PATH/vxd/vxd.pri)


