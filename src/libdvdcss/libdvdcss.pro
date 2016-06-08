# qmake configuration file to build libdvdcss

TARGET = dvdcss
TEMPLATE = lib
CONFIG += staticlib

CONFIG -= qt
CONFIG -= debug_and_release
CONFIG *= largefile
CONFIG -= warn_off
CONFIG *= warn_on

INCLUDEPATH += include
unix|mingw|gcc:QMAKE_CFLAGS_WARN_ON = -Wall -Wextra
win32:QMAKE_CFLAGS += -mwin32
win32:DEFINES -= UNICODE

SOURCES += \
    src/css.c \
    src/device.c \
    src/error.c \
    src/ioctl.c \
    src/libdvdcss.c

HEADERS += \
    src/common.h \
    src/config.h \
    src/css.h \
    src/csstables.h \
    src/device.h \
    src/ioctl.h \
    src/libdvdcss.h \
    include/dvdcss.h
