
include($$PWD/../../gcl_environment.pri)

#include($$CCXX_PATH/ccxx/ccxx.pri)
#
include($$GCL_SRC_PATH/ga/gcl_ga.pri)

include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)
#
include($$PWD/../../../cc3rd/fcgi/fcgi.pri)
#
include($$PWD/../../../cc3rd/cgicc/cgicc.pri)
#
include($$GCL_SRC_PATH/main/aid/aid.pri)


LIBS += -lgcl_ccxx1

