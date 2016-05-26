#----------------------------------------------------------------------------
#
# Copyright (c) 2013, Thierry Lelegard
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
#
#----------------------------------------------------------------------------
#
# Qmake project file for the QtlMovie unitary tests.
#
#----------------------------------------------------------------------------

CONFIG += qts qtl
include(../Qts/Qts.pri)

TARGET = UnitTest
TEMPLATE = app

# QTest configuration
QT += testlib
QT -= gui
CONFIG += console
CONFIG -= app_bundle
DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    QtlVariableTest.cpp \
    QtlSmartPointerTest.cpp \
    QtsData.cpp \
    QtsSectionTest.cpp \
    QtlByteBlockTest.cpp \
    QtsSectionDemuxTest.cpp \
    main.cpp \
    QtlTest.cpp \
    QtsProgramMapTableTest.cpp \
    QtlHexaTest.cpp \
    QtlVersionTest.cpp \
    QtlIsoLanguagesTest.cpp \
    QtlOpticalDriveTest.cpp \
    QtlStringUtilsTest.cpp \
    QtlFileSystemInfoTest.cpp \
    QtsTeletextDemuxTest.cpp

HEADERS += \
    QtlTest.h \
    QtsData.h \
    QtsData/psi_tot_tnt_sections.h \
    QtsData/psi_tot_tnt_packets.h \
    QtsData/psi_tdt_tnt_sections.h \
    QtsData/psi_tdt_tnt_packets.h \
    QtsData/psi_pmt_planete_sections.h \
    QtsData/psi_pmt_planete_packets.h \
    QtsData/psi_pat_r4_sections.h \
    QtsData/psi_pat_r4_packets.h \
    QtsData/psi_nit_tntv23_sections.h \
    QtsData/psi_nit_tntv23_packets.h \
    QtsData/psi_cat_r6_sections.h \
    QtsData/psi_cat_r6_packets.h \
    QtsData/psi_cat_r3_sections.h \
    QtsData/psi_cat_r3_packets.h \
    QtsData/psi_bat_tvnum_sections.h \
    QtsData/psi_bat_tvnum_packets.h \
    QtsData/psi_bat_cplus_sections.h \
    QtsData/psi_bat_cplus_packets.h \
    QtsData/psi_sdt_r6_packets.h \
    QtsData/psi_sdt_r6_sections.h

RESOURCES += \
    UnitTest.qrc
