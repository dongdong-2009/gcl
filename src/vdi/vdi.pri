
include($$GCL_SRC_PATH/psm/psm.pri)
include($$GCL_SRC_PATH/shm/gcl_sharememory.pri)

include($$PWD/vdi_measure.pri)
include($$PWD/vdi_worker.pri)
include($$PWD/vdi_protocol.pri)
include($$PWD/vdi_terminal.pri)
include($$PWD/vdi_cycle.pri)
include($$PWD/vdi_cases.pri)
include($$PWD/vdi_business.pri)
include($$PWD/vdi_task.pri)
include($$PWD/vdi_fix_measure.pri)
include($$PWD/vdi_common.pri)
include($$PWD/vdi_file.pri)

HEADERS += \
    $$PWD/../common/gcl_config.h \
    $$PWD/../common/gcl_macro.h \
    $$PWD/../common/gcl_resource.h \
    $$PWD/../common/gcl_struct.h \

###***gcl_config
SOURCES += \
    $$PWD/../common/gcl_config.cpp \
