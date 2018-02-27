
DEFINES += PROJECT_TEMPLATE_LIB_DLL
#DEFINES += PROJECT_TEMPLATE_LIB_STATIC
DEFINES += GC_RTSHM_EXPORTS

TARGET = gcl_rt_shm

LIBS += -lgcl_ccxx1

include($$PWD/../../gcl_environment.pri)

include($$GCL_SRC_PATH/gcsharemem/gcrtsharememlib.pri)
include($$GCL_INCLUDE_PATH/shm_inc.pri)

