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
# Qts, the Qt MPEG Transport Stream library.
# Qmake project file for the Qts library.
#
#----------------------------------------------------------------------------

CONFIG += qtl
include(../QtlCommon.pri)

TARGET = Qts
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    QtsCore.cpp \
    QtsCrc32.cpp \
    QtsDescriptor.cpp \
    QtsPesDemux.cpp \
    QtsPesPacket.cpp \
    QtsSection.cpp \
    QtsTable.cpp \
    QtsTsPacket.cpp \
    QtsTsFile.cpp \
    QtsSectionDemux.cpp \
    QtsDemux.cpp \
    QtsTeletextDescriptor.cpp \
    QtsAbstractDescriptor.cpp \
    QtsPsiUtils.cpp \
    QtsDescriptorList.cpp \
    QtsPrivateDataSpecifierDescriptor.cpp \
    QtsProgramAssociationTable.cpp \
    QtsAbstractTable.cpp \
    QtsAbstractLongTable.cpp \
    QtsProgramMapTable.cpp \
    QtsStandaloneTableDemux.cpp

HEADERS += \
    QtsCore.h \
    QtsCrc32.h \
    QtsDescriptor.h \
    QtsPesDemux.h \
    QtsPesHandlerInterface.h \
    QtsPesPacket.h \
    QtsSection.h \
    QtsSectionHandlerInterface.h \
    QtsTable.h \
    QtsTableHandlerInterface.h \
    QtsTsPacket.h \
    QtsTsFile.h \
    QtsAbstractTable.h \
    QtsAbstractLongTable.h \
    QtsSectionDemux.h \
    QtsDemux.h \
    QtsAbstractDescriptor.h \
    QtsTeletextDescriptor.h \
    QtsPsiUtils.h \
    QtsDescriptorList.h \
    QtsPrivateDataSpecifierDescriptor.h \
    QtsProgramAssociationTable.h \
    QtsProgramMapTable.h \
    QtsStandaloneTableDemux.h
