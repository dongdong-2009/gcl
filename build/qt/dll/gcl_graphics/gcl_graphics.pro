
DEFINES += PROJECT_TEMPLATE_LIB_DLL
#DEFINES += PROJECT_TEMPLATE_LIB_STATIC

LIBS += -lgcl_ccxx1

DEFINES += PROJECT_3RDPARTY_QT

include($$PWD/../../gcl_environment.pri)

QT       += xml

#
include($$GCL_SRC_PATH/qtgraphics/qtgraphics.pri)

