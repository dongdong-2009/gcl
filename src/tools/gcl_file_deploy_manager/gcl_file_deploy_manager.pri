
QT       += gui
CONFIG += designer

HEADERS += \
    $$PWD/droparea.h \
    $$PWD/dropsitewindow.h \
    $$PWD/fdvmanagerwin.h \
    $$PWD/fdcmanagerwin.h \
    $$PWD/fdv.h \
    $$PWD/fdc.h \
    $$PWD/mainwindow.h \
    $$PWD/areaviewwin.h \
    $$PWD/messageboxwin.h \
    $$PWD/pra.h \
    $$PWD/pramanagerwin.h \
    $$PWD/praupdownwin.h \
    $$PWD/praui.h \
    $$PWD/praupdownterminalplugin.h \
    $$PWD/dialog_fdc_quick_input.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/droparea.cpp \
    $$PWD/dropsitewindow.cpp \
    $$PWD/fdvmanagerwin.cpp \
    $$PWD/fdcmanagerwin.cpp \
    $$PWD/fdv.cpp \
    $$PWD/fdc.cpp \
    $$PWD/areaviewwin.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/messageboxwin.cpp \
    $$PWD/pra.cpp \
    $$PWD/pramanagerwin.cpp \
    $$PWD/praupdownwin.cpp \
    $$PWD/praui.cpp \
    $$PWD/praupdownterminalplugin.cpp \
    $$PWD/dialog_fdc_quick_input.cpp

FORMS += \
    $$PWD/../../qtwincontrol/myinputpanel.ui \
    $$PWD/fdvmanagerwin.ui \
    $$PWD/fdcmanagerwin.ui \
    $$PWD/areaviewwin.ui \
    $$PWD/mainwindow.ui \
    $$PWD/messageboxwin.ui \
    $$PWD/pramanagerwin.ui \
    $$PWD/praupdownwin.ui \
    $$PWD/dialog_fdc_quick_input.ui


HEADERS += \
    $$PWD/../../qtwincontrol/describe_win.h \
    $$PWD/../../qtwincontrol/dialog_win.h \
    $$PWD/../../qtwincontrol/simple_win.h \
    $$PWD/../../qtwincontrol/table_win.h \
    $$PWD/../../qtwincontrol/hex_spin_box.h \
    $$PWD/../../qtwincontrol/myinputpanel.h \
    $$PWD/../../psm/psm_explain.h \


SOURCES += \
    $$PWD/../../qtwincontrol/describe_win.cpp \
    $$PWD/../../qtwincontrol/dialog_win.cpp \
    $$PWD/../../qtwincontrol/simple_win.cpp \
    $$PWD/../../qtwincontrol/table_win.cpp \
    $$PWD/../../qtwincontrol/hex_spin_box.cpp \
    $$PWD/../../qtwincontrol/myinputpanel.cpp \
    $$PWD/../../psm/psm_explain.cpp \

DISTFILES += \
    $$PWD/readme.txt \
    $$PWD/dropsite.desktop \

SUBDIRS += \
    $$PWD/dropsite.pro




