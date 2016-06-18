# qmake include file for projects using libdvdcss

libdvdcss {
    LIBS += -L../libdvdcss -ldvdcss
    macx:LIBS += -framework CoreFoundation
    PRE_TARGETDEPS += ../libdvdcss/libdvdcss.a
    INCLUDEPATH += ../libdvdcss/include
    DEPENDPATH += ../libdvdcss
}
