
include($$PWD/../../gcl_environment.pri)

#include($$CCXX_PATH/ccxx/ccxx.pri)

SOURCES +=  \
#    main.cpp \
#    test_lua001.cpp \
#    test_lua002.cpp \
#    test_lua003.cpp \
    test_userdata1.cpp

LIBS += -lgcl_script1
LIBS += -lgcl_ccxx1
#LIBS += -lgcl_lua_5.3.01
#LIBS += -lgcl_lua_5.1.5


HEADERS += \
    lunar.h

OTHER_FILES += \
    000.lua
