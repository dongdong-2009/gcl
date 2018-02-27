
QT       += gui
CONFIG += designer


SOURCES += \
    $$PWD/main.cpp \
    $$PWD/configself_win.cpp \
    $$PWD/creator_plugin.cpp \
    $$PWD/terminal_plugin.cpp \
    $$PWD/setting_plugin.cpp


FORMS += \
    $$PWD/configself_win.ui

HEADERS += \
    $$PWD/configself_win.h \
    $$PWD/creator_plugin.h \
    $$PWD/terminal_plugin.h \
    $$PWD/setting_plugin.h


FORMS += \
    $$PWD/../../qtwincontrol/myinputpanel.ui


HEADERS += \
    $$PWD/../../qtwincontrol/describe_win.h \
    $$PWD/../../qtwincontrol/dialog_win.h \
    $$PWD/../../qtwincontrol/simple_win.h \
    $$PWD/../../qtwincontrol/table_win.h \
    $$PWD/../../qtwincontrol/hex_spin_box.h \
    $$PWD/../../qtwincontrol/myinputpanel.h \
#    $$PWD/../../qtwincontrol/myinputpanelcontext.h


SOURCES += \
    $$PWD/../../qtwincontrol/describe_win.cpp \
    $$PWD/../../qtwincontrol/dialog_win.cpp \
    $$PWD/../../qtwincontrol/simple_win.cpp \
    $$PWD/../../qtwincontrol/table_win.cpp \
    $$PWD/../../qtwincontrol/hex_spin_box.cpp \
    $$PWD/../../qtwincontrol/myinputpanel.cpp \
#    $$PWD/../../qtwincontrol/myinputpanelcontext.cpp

