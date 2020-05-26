#-------------------------------------------------
#
# Project created by QtCreator 2015-02-17T17:42:10
#
#-------------------------------------------------

QT  += core gui
QT  += network
QT  += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

TARGET = Plasma
TEMPLATE = app

RC_FILE = fbh_Plasma.rc

SOURCES += src/main.cpp\
        src/widget.cpp \
    src/qcustomplot.cpp \
    src/settingsComPort.cpp \
    src/switch.cpp

HEADERS  += src/widget.h \
    src/qcustomplot.h \
    globalconsts.h \
    src/settingsComPort.h \
    src/switch.h

FORMS    += widget.ui \
    switch.ui \
    src/settingsComPort.ui \
    src/switch.ui

DISTFILES += \
    fbh_Plasma.rc \
    assets/FBH-Logo halbtransparent.png

RESOURCES += \
    assets/bilder.qrc
