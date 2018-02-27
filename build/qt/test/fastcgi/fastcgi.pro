
include($$PWD/../../gcl_environment.pri)

#include($$CCXX_PATH/ccxx/ccxx.pri)
#
include($$GCL_SRC_PATH/psm/psm.pri)
#
include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)
#
include($$GCL_3RD_PATH/fcgi/fcgi.pri)
#
include($$GCL_3RD_PATH/cgicc/cgicc.pri)


SOURCES +=  \
#    main.cpp \
#

SOURCES +=  \
#    main1.cpp \
#    $$PWD/../../../cc3rd/cgicc/contrib/fcgi-test.cpp \

SOURCES +=  \
    cgi-fcgi.c \

LIBS += -lgcl_ccxx1

