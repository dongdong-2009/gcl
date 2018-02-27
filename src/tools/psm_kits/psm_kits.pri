
SOURCES += \
    $$PWD/main.cpp \
    $$PWD/psm_kits_win.cpp


QT       += gui


CONFIG += designer


FORMS += \
    $$PWD/../../qtwincontrol/myinputpanel.ui \
    $$PWD/psm_kits_win.ui


HEADERS += \
    $$PWD/../../qtwincontrol/describe_win.h \
    $$PWD/../../qtwincontrol/dialog_win.h \
    $$PWD/../../qtwincontrol/simple_win.h \
    $$PWD/../../qtwincontrol/table_win.h \
    $$PWD/../../qtwincontrol/hex_spin_box.h \
    $$PWD/../../qtwincontrol/myinputpanel.h \
    $$PWD/../../qtwincontrol/myinputpanelcontext.h \
    $$PWD/psm_kits_win.h


SOURCES += \
    $$PWD/../../qtwincontrol/describe_win.cpp \
    $$PWD/../../qtwincontrol/dialog_win.cpp \
    $$PWD/../../qtwincontrol/simple_win.cpp \
    $$PWD/../../qtwincontrol/table_win.cpp \
    $$PWD/../../qtwincontrol/hex_spin_box.cpp \
    $$PWD/../../qtwincontrol/myinputpanel.cpp \
    $$PWD/../../qtwincontrol/myinputpanelcontext.cpp
