#----------------------------------------------------------------------------
#
# Copyright (c) 2013-2016, Thierry Lelegard
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
# libQtl, Qt utility library.
# Qmake project file for the libQtl library.
#
#----------------------------------------------------------------------------

include(libQtl.pri)

TARGET = Qtl
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../libdvdcss/include

SOURCES += \
    QtlByteBlock.cpp \
    QtlFile.cpp \
    QtlLineEdit.cpp \
    QtlPlainTextLogger.cpp \
    QtlBrowserDialog.cpp \
    QtlProcess.cpp \
    QtlProcessResult.cpp \
    QtlStringList.cpp \
    QtlFixedSizeTableWidget.cpp \
    QtlHexa.cpp \
    QtlButtonGrid.cpp \
    QtlAutoGrid.cpp \
    QtlTranslator.cpp \
    QtlVersion.cpp \
    QtlNewVersionChecker.cpp \
    QtlNewVersionCheckerJson.cpp \
    QtlSimpleWebRequest.cpp \
    QtlTextFileViewer.cpp \
    QtlSysInfo.cpp \
    QtlIsoLanguages.cpp \
    QtlIntValidator.cpp \
    QtlNamedIntComboBox.cpp \
    QtlNamedIntSet.cpp \
    QtlErrorBase.cpp \
    QtlPlainTextEdit.cpp \
    QtlPasswordDialog.cpp \
    QtlTlvTableWidget.cpp \
    QtlDialog.cpp \
    QtlCore.cpp \
    QtlStringUtils.cpp \
    QtlListWidgetUtils.cpp \
    QtlMessageBoxUtils.cpp \
    QtlTableWidgetUtils.cpp \
    QtlSettings.cpp \
    QtlSelectionRectDelegate.cpp \
    QtlOpticalDrive.cpp \
    QtlSynchronousProcess.cpp \
    QtlMediaStreamInfo.cpp \
    QtlDvdTitleSet.cpp \
    QtlDataPull.cpp \
    QtlDvdDataPull.cpp \
    QtlFileDataPull.cpp \
    QtlDvdMedia.cpp \
    QtlDvdDirectory.cpp \
    QtlLayoutUtils.cpp \
    QtlCheckableHeaderView.cpp \
    QtlFileDialogUtils.cpp \
    QtlDvdFile.cpp

HEADERS += \
    QtlByteBlock.h \
    QtlFile.h \
    QtlLineEdit.h \
    QtlLogger.h \
    QtlNullLogger.h \
    QtlPlainTextLogger.h \
    QtlBrowserDialog.h \
    QtlProcess.h \
    QtlProcessResult.h \
    QtlSmartPointer.h \
    QtlSmartPointerTemplate.h \
    QtlStringList.h \
    QtlVariableTemplate.h \
    QtlVariable.h \
    QtlUninitializedException.h \
    QtlFilePathFilterInterface.h \
    QtlFixedSizeTableWidget.h \
    QtlHexa.h \
    images/browser-right.h \
    images/browser-left.h \
    images/browser-home.h \
    QtlIncrement.h \
    QtlButtonGrid.h \
    QtlAutoGrid.h \
    QtlTranslator.h \
    QtlVersion.h \
    QtlNewVersionChecker.h \
    QtlNewVersionCheckerJson.h \
    QtlSimpleWebRequest.h \
    QtlTextFileViewer.h \
    QtlSysInfo.h \
    QtlIsoLanguages.h \
    QtlIntValidator.h \
    QtlNamedIntComboBox.h \
    QtlNamedIntSet.h \
    QtlErrorBase.h \
    QtlPointerList.h \
    QtlPlainTextEdit.h \
    QtlTlv.h \
    QtlTlvList.h \
    QtlTlvListTemplate.h \
    QtlTlvTemplate.h \
    QtlPasswordDialog.h \
    QtlTlvTableWidgetTemplate.h \
    QtlTlvTableWidget.h \
    QtlDialog.h \
    QtlGeometrySettingsInterface.h \
    QtlCore.h \
    QtlStringUtils.h \
    QtlListWidgetUtils.h \
    QtlMessageBoxUtils.h \
    QtlTableWidgetUtils.h \
    QtlSettings.h \
    QtlNumUtils.h \
    QtlWinUtils.h \
    QtlSelectionRectDelegate.h \
    QtlOpticalDrive.h \
    QtlSynchronousProcess.h \
    QtlMediaStreamInfo.h \
    QtlDvdTitleSet.h \
    QtlStdoutLogger.h \
    QtlDataPull.h \
    QtlDvdDataPull.h \
    QtlFileDataPull.h \
    QtlDvdMedia.h \
    QtlDvdFile.h \
    QtlDvdDirectory.h \
    QtlLayoutUtils.h \
    QtlCheckableHeaderView.h \
    QtlFileDialogUtils.h

FORMS += \
    QtlBrowserDialog.ui \
    QtlTextFileViewer.ui

TRANSLATIONS += \
    locale/qtl_fr.ts
