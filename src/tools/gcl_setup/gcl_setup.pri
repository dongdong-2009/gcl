
SOURCES += \
    $$PWD/main.cpp \
    $$PWD/setup_win.cpp \


FORMS += \
    $$PWD/setup_win.ui

HEADERS += \
    $$PWD/setup_win.h \


QT       += gui


CONFIG += designer


FORMS += \
    $$PWD/../../qtwincontrol/myinputpanel.ui


HEADERS += \
    $$PWD/../../qtwincontrol/describe_win.h \
    $$PWD/../../qtwincontrol/dialog_win.h \
    $$PWD/../../qtwincontrol/simple_win.h \
    $$PWD/../../qtwincontrol/table_win.h \
    $$PWD/../../qtwincontrol/hex_spin_box.h \
    $$PWD/../../qtwincontrol/myinputpanel.h \
    $$PWD/../../qtwincontrol/myinputpanelcontext.h


SOURCES += \
    $$PWD/../../qtwincontrol/describe_win.cpp \
    $$PWD/../../qtwincontrol/dialog_win.cpp \
    $$PWD/../../qtwincontrol/simple_win.cpp \
    $$PWD/../../qtwincontrol/table_win.cpp \
    $$PWD/../../qtwincontrol/hex_spin_box.cpp \
    $$PWD/../../qtwincontrol/myinputpanel.cpp \
    $$PWD/../../qtwincontrol/myinputpanelcontext.cpp
