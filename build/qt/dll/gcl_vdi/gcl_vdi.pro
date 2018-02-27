
DEFINES += PROJECT_TEMPLATE_LIB_DLL
#DEFINES += PROJECT_TEMPLATE_LIB_STATIC

#
include($$PWD/../../gcl_environment.pri)

TARGET = gcl_vdi

DEFINES += GM_BUILD_GCL_VDI_LIB

#
include($$GCL_SRC_PATH/vdi/vdi.pri)

#
LIBS += -lgcl_ccxx1

