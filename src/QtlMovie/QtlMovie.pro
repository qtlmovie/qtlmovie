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
# Qmake project file for the QtlMovie application.
#
#----------------------------------------------------------------------------

CONFIG += qts qtl
include(../QtlCommon.pri)

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
    QtlMovieStreamInfo.cpp \
    QtlMovieFFmpeg.cpp \
    QtlMovieFFprobeTags.cpp \
    QtlMovieDvd.cpp \
    QtlMovieTeletextSearch.cpp \
    QtlMovieTsDemux.cpp \
    QtlMovieMkisofsProcess.cpp \
    QtlMovieGrowisofsProcess.cpp

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
    QtlMovieStreamInfo.h \
    QtlMovieFFmpeg.h \
    QtlMovieFFprobeTags.h \
    QtlMovieDvd.h \
    QtlMovieTeletextSearch.h \
    QtlMovieTsDemux.h \
    QtlMovieMkisofsProcess.h \
    QtlMovieGrowisofsProcess.h

FORMS += QtlMovieMainWindow.ui \
    QtlMovieEditSettings.ui \
    QtlMovieInputFilePropertiesDialog.ui \
    QtlMovieAudioTestDialog.ui \
    QtlMovieAboutMediaTools.ui

RESOURCES += QtlMovie.qrc

win32:RC_FILE += QtlMovie.rc

OTHER_FILES += \
    help/qtlmovie.html \
    help/qtlmovie-user.html \
    help/qtlmovie-build.html \
    help/default.css
