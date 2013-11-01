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
# Common qmake definition file. Must be included from all projects.
#
#----------------------------------------------------------------------------

# Qt configuration options.

CONFIG *= qt
CONFIG *= thread
CONFIG *= largefile
CONFIG -= debug_and_release

QT *= core
QT *= gui
QT *= widgets
QT *= network
QT *= multimedia

# Determine build pass (release or debug).

build_pass:CONFIG(debug, debug|release):PASS = debug
else:build_pass:CONFIG(release, debug|release):PASS = release

# Define the symbol DEBUG in debug mode.

build_pass:CONFIG(debug, debug|release):DEFINES += DEBUG

# Enforce compilation warnings.

CONFIG -= warn_off
CONFIG *= warn_on

# GCC specific options.
# Unfortunately, there is no reliable way of checking that we use gcc.
# Here, we assume that all flavors of Unix use GCC (far from perfect).

unix|mingw|gcc {
    # Specific paranoid warnings setup with GCC.
    QMAKE_CXXFLAGS_WARN_ON = -Werror -Wall -Wextra -Wno-unused-parameter -Woverloaded-virtual
    # Do not assume strict aliasing.
    QMAKE_CXXFLAGS += -fno-strict-aliasing
}

# Custom variable QMAKE_LRELEASE points to the translation file compiler.

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

# Translation files handling.

updateqm.input = TRANSLATIONS
updateqm.output = locale/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm locale/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm

# If qts is in the config, add dependencies.

qts {
    CONFIG *= qtl # Qts depends on Qtl
    LIBS += -L../Qts/$$PASS/ -lQts
    PRE_TARGETDEPS += ../Qts/$$PASS/libQts.a
    INCLUDEPATH += ../Qts
    DEPENDPATH += ../Qts
}

# If qtl is in the config, add dependencies.

qtl {
    LIBS += -L../Qtl/$$PASS/ -lQtl
    PRE_TARGETDEPS += ../Qtl/$$PASS/libQtl.a
    INCLUDEPATH += ../Qtl
    DEPENDPATH += ../Qtl
}
