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
# Qmake project file for the QtlMovie application.
#
#----------------------------------------------------------------------------

CONFIG += libQts libQtl
include(../libQts/libQts.pri)

# The application uses QtMultimedia and, on Windows with Qt >= 5.2, winextras.
QT += multimedia
win32:greaterThan(QT_MAJOR_VERSION, 4) {
    greaterThan(QT_MAJOR_VERSION, 5)|greaterThan(QT_MINOR_VERSION, 1): QT += winextras
}

TARGET = QtlMovie
TEMPLATE = app

SOURCES += main.cpp \
    QtlMovieMainWindow.cpp \
    QtlMovieInputFile.cpp \
    QtlMovieOutputFile.cpp \
    QtlMovieExecFile.cpp \
    QtlMovieEditSettings.cpp \
    QtlMovieSettings.cpp \
    QtlMovieInputFilePropertiesDialog.cpp \
    QtlMovieAudioTestDialog.cpp \
    QtlMovieFFmpegProcess.cpp \
    QtlMovieAboutMediaTools.cpp \
    QtlMovieProcess.cpp \
    QtlMovieJob.cpp \
    QtlMovieAction.cpp \
    QtlMovieDvdAuthorProcess.cpp \
    QtlMovieDeleteAction.cpp \
    QtlMovieFFmpeg.cpp \
    QtlMovieFFprobeTags.cpp \
    QtlMovieTeletextSearch.cpp \
    QtlMovieTsDemux.cpp \
    QtlMovieMkisofsProcess.cpp \
    QtlMovieGrowisofsProcess.cpp \
    QtlMovieClosedCaptionsSearch.cpp \
    QtlMovieCcExtractorProcess.cpp \
    QtlMovieFFmpegVolumeDetect.cpp \
    QtlMovieSettingsMigration.cpp \
    QtlMovieEditTask.cpp \
    QtlMovieTask.cpp \
    QtlMovieEditTaskDialog.cpp \
    QtlMovieTaskList.cpp \
    QtlMovieDeviceProfile.cpp \
    QtlMovieTeletextExtract.cpp \
    QtlMovieMainWindowBase.cpp \
    QtlMovieDvdExtractionSession.cpp \
    QtlMovieDvdExtractionWindow.cpp \
    QtlMovieCleanupSubtitles.cpp

HEADERS += \
    QtlMovieMainWindow.h \
    QtlMovieInputFile.h \
    QtlMovieOutputFile.h \
    QtlMovieExecFile.h \
    QtlMovieEditSettings.h \
    QtlMovieSettings.h \
    QtlMovieInputFilePropertiesDialog.h \
    QtlMovieAudioTestDialog.h \
    QtlMovieFFmpegProcess.h \
    QtlMovieVersion.h \
    QtlMovieAboutMediaTools.h \
    QtlMovie.h \
    QtlMovieProcess.h \
    QtlMovieJob.h \
    QtlMovieAction.h \
    QtlMovieDvdAuthorProcess.h \
    QtlMovieDeleteAction.h \
    QtlMovieFFmpeg.h \
    QtlMovieFFprobeTags.h \
    QtlMovieTeletextSearch.h \
    QtlMovieTsDemux.h \
    QtlMovieMkisofsProcess.h \
    QtlMovieGrowisofsProcess.h \
    QtlMovieClosedCaptionsSearch.h \
    QtlMovieCcExtractorProcess.h \
    QtlMovieFFmpegVolumeDetect.h \
    QtlMovieSettingsMigration.h \
    QtlMovieEditTask.h \
    QtlMovieTask.h \
    QtlMovieEditTaskDialog.h \
    QtlMovieTaskList.h \
    QtlMovieDeviceProfile.h \
    QtlMovieTeletextExtract.h \
    QtlMovieMainWindowBase.h \
    QtlMovieDvdExtractionSession.h \
    QtlMovieDvdExtractionWindow.h \
    QtlMovieCleanupSubtitles.h

FORMS += QtlMovieMainWindow.ui \
    QtlMovieEditSettings.ui \
    QtlMovieInputFilePropertiesDialog.ui \
    QtlMovieAudioTestDialog.ui \
    QtlMovieAboutMediaTools.ui \
    QtlMovieEditTask.ui \
    QtlMovieDvdExtractionWindow.ui

RESOURCES += QtlMovie.qrc

win32:RC_FILE += QtlMovie.rc
mac:ICON = images/qtlmovie.icns

OTHER_FILES += \
    help/qtlmovie.html \
    help/qtlmovie-user.html \
    help/qtlmovie-build.html \
    help/default.css

TRANSLATIONS += \
    locale/qtlmovie_fr.ts

DISTFILES += \
    ../HEADER.txt
