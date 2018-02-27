#-------------------------------------------------
#
# Project created by QtCreator 2014-09-18T17:45:35
#
#-------------------------------------------------


DEFINES += PROJECT_TEMPLATE_LIB_DLL
DEFINES += GCI_API_IMPL_EXPORT
DEFINES += GMAC_SHARE_MEMORY


include($$PWD/../../gcl_environment.pri)

LIBS += -lgcl_vdi1
LIBS += -lgcl_ccxx1

TARGET = gcl_shm_sdk

include($$GCL_SRC_PATH/vxd/shm_protocol.pri)

include($$GCL_SRC_PATH/vxd/shm_sdk_client.pri)

include($$GCL_INCLUDE_PATH/shm_sdk_inc.pri)
