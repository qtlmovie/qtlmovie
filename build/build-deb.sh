#!/bin/bash
#-----------------------------------------------------------------------------
# 
#  Copyright (c) 2013, Thierry Lelegard
#  All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
# 
#  1. Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer. 
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution. 
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGE.
# 
#-----------------------------------------------------------------------------
# 
#  Unix script to build the DEB package for QtlMovie.
# 
#-----------------------------------------------------------------------------

# This script:
SCRIPT=$(basename $BASH_SOURCE)
error() { echo >&2 "$SCRIPT: $*"; exit 1; }

# Various directories.
SCRIPTDIR=$(cd $(dirname $BASH_SOURCE); pwd)
ROOTDIR=$(dirname $SCRIPTDIR)
SRCDIR=$ROOTDIR/src
HELPDIR=$ROOTDIR/help
BUILDDIR=$ROOTDIR/build-QtlMovie-Desktop-Release

# QtlMovie version is extracted from QtlMovieVersion.h
QTLMOVIE_VERSION=$(grep QTLMOVIE_VERSION $SRCDIR/QtlMovie/QtlMovieVersion.h | sed -e 's/[^"]*"//' -e 's/".*//')

# Rebuilt the QtlMovie executable.
$SCRIPTDIR/build.sh || exit 1

# Build a temporary system root containing the installation
TMPDIR=/tmp/qtlmovie-$$
ICON_SIZES="16 24 32 48 64 72 96 128 256 512"
rm -rf $TMPDIR
mkdir -p -m 0755 \
    $TMPDIR/DEBIAN \
    $TMPDIR/usr/bin \
    $TMPDIR/usr/share/qt5/translations \
    $TMPDIR/usr/share/applications \
    $TMPDIR/usr/share/doc/qtlmovie
for sz in $ICON_SIZES; do
    mkdir -p -m 0755 $TMPDIR/usr/share/icons/hicolor/${sz}x${sz}/apps
done
sed -e "s/{VERSION}/$QTLMOVIE_VERSION/" \
    -e "s/{ARCH}/$(dpkg-architecture -qDEB_BUILD_ARCH)/" \
    $SCRIPTDIR/QtlMovie.control >$TMPDIR/DEBIAN/control
install -m 0755 $SCRIPTDIR/QtlMovie.postinst $TMPDIR/DEBIAN/postinst
install -m 0755 $BUILDDIR/QtlMovie/QtlMovie $TMPDIR/usr/bin
strip $TMPDIR/usr/bin/QtlMovie
install -m 0644 \
    $BUILDDIR/libQtl/locale/qtl_fr.qm \
    $BUILDDIR/libQts/locale/qts_fr.qm \
    $BUILDDIR/QtlMovie/locale/qtlmovie_fr.qm \
    $TMPDIR/usr/share/qt5/translations
install -m 0644 $SCRIPTDIR/QtlMovie.desktop $TMPDIR/usr/share/applications
for sz in $ICON_SIZES; do
    install -m 0644 $SRCDIR/QtlMovie/images/qtlmovie-${sz}.png $TMPDIR/usr/share/icons/hicolor/${sz}x${sz}/apps/qtlmovie.png
done
install -m 0644 $HELPDIR/* $TMPDIR/usr/share/doc/qtlmovie

# Build the binary package
dpkg --build $TMPDIR $ROOTDIR/installers

# Cleanup the temporary root.
rm -rf $TMPDIR
