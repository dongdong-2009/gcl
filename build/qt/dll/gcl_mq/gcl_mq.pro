#-------------------------------------------------
#
# Project created by QtCreator 2014-07-19T00:19:58
#
#-------------------------------------------------

#CCXX define

DEFINES += PROJECT_TEMPLATE_LIB_DLL
DEFINES += DLL_EXPORT

include($$PWD/../../gcl_environment.pri)

TARGET = gcl_zmq_3.2.2

include($$GCL_SRC_PATH/3party/zeromq-3.2.2/src/zeromq.pri)
#include($$GCL_SRC_PATH/3party/zeromq-3.2.2/include/zeromqinc.pri)
