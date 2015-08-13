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
# Common qmake definition file.
# Must be included from all projects using the Qtl library.
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
win32: QT *= axcontainer

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

# Applications using the Qtl library depends on it.

qtl {
    LIBS += -L../Qtl -lQtl
    PRE_TARGETDEPS += ../Qtl/libQtl.a
    INCLUDEPATH += ../Qtl
    DEPENDPATH += ../Qtl
}

# Applications using OpenSSL shall use "CONFIG += openssl".
# On Windows, assume default location used by binary installers from
# http://slproweb.com/products/Win32OpenSSL.html.

openssl {
    mingw {
        OPENSSLDIR = C:/OpenSSL-Win32
        LIBS += $$OPENSSLDIR/lib/MinGW/ssleay32.a $$OPENSSLDIR/lib/MinGW/libeay32.a
        INCLUDEPATH += $$OPENSSLDIR/include
    }
    else {
        LIBS += -lssl -lcrypto
    }
}
