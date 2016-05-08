#!/bin/bash
#-----------------------------------------------------------------------------
#
#  Copyright (c) 2016, Thierry Lelegard
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
#  Unix script to build the DMG package for QtlMovie on Mac OS X.
#
#-----------------------------------------------------------------------------

# This script:
SCRIPT=$(basename $BASH_SOURCE)
error() { echo >&2 "$SCRIPT: $*"; exit 1; }

# Various directories.
SCRIPTDIR=$(cd $(dirname $BASH_SOURCE); pwd)
ROOTDIR=$(dirname $SCRIPTDIR)
SRCDIR=$ROOTDIR/src
BUILDDIR=$ROOTDIR/build-QtlMovie-Desktop-Release

# QtlMovie version is extracted from QtlMovieVersion.h
QTLMOVIE_VERSION=$(grep QTLMOVIE_VERSION $SRCDIR/QtlMovie/QtlMovieVersion.h | sed -e 's/[^"]*"//' -e 's/".*//')

# Rebuilt the QtlMovie executable.
$SCRIPTDIR/build.sh || exit 1

# Build the binary package
echo "Creating QtlMovie-$QTLMOVIE_VERSION.dmg"
APPDIR=$BUILDDIR/QtlMovie/QtlMovie.app
DMGTMP=$BUILDDIR/QtlMovie/QtlMovie.dmg
DMGFILE=$ROOTDIR/installers/QtlMovie-$QTLMOVIE_VERSION.dmg
VOLUME="QtlMovie $QTLMOVIE_VERSION"
rm -f "$DMGTMP" "$DMGFILE"

# Populate Mac tools in the bundle
rm -rf $APPDIR/Contents/MacOS/mactools
tar -C $ROOTDIR --exclude .gitignore --exclude '*.txt' -c -p -f - mactools | tar -C $APPDIR/Contents/MacOS -x -p -f -
strip $APPDIR/Contents/MacOS/QtlMovie $APPDIR/Contents/MacOS/mactools/*

# Deploy Qt requirements in the bundle.
macdeployqt $BUILDDIR/QtlMovie/QtlMovie.app -verbose=1 -no-plugins -always-overwrite || exit 1

# Create an initial disk image (96 MB)
hdiutil create -size 96m -fs HFS+ -volname "$VOLUME" "$DMGTMP"

# Mount the disk image
hdiutil attach "$DMGTMP"
DEVS=$(hdiutil attach "$DMGTMP" | cut -f 1)
DEV=$(echo $DEVS | cut -f 1 -d ' ')

# Copy application structure inside disk image.
tar -C $(dirname "$APPDIR") -c -p -f - $(basename "$APPDIR") | tar -C "/Volumes/$VOLUME" -x -p -f -

# Format the appearance of the DMG in Finder when opened.
mkdir "/Volumes/$VOLUME/.background"
cp $ROOTDIR/build/dmg-background.png "/Volumes/$VOLUME/.background/background.png"
echo '
   tell application "Finder"
     tell disk "'${VOLUME}'"
           open
           set current view of container window to icon view
           set toolbar visible of container window to false
           set statusbar visible of container window to false
           set the bounds of container window to {400, 100, 900, 450}
           set theViewOptions to the icon view options of container window
           set arrangement of theViewOptions to not arranged
           set icon size of theViewOptions to 128
           set background picture of theViewOptions to file ".background:background.png"
           make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
           set position of item "QtlMovie" of container window to {100, 100}
           set position of item "Applications" of container window to {375, 100}
           update without registering applications
           delay 5
           close
     end tell
   end tell
' | osascript

# Unmount the disk image
hdiutil detach $DEV

# Convert the disk image to read-only
hdiutil convert "$DMGTMP" -format UDZO -o "$DMGFILE"
rm -f "$DMGTMP"

# Create a zip file for the Mac tools.
echo "Creating QtlMovie-$QTLMOVIE_VERSION-mactools.zip"
(cd $ROOTDIR; zip -rq9 $ROOTDIR/installers/QtlMovie-$QTLMOVIE_VERSION-mactools.zip mactools -x mactools/.gitignore)

# Create a zip file for the QtlMovie sources.
echo "Creating QtlMovie-$QTLMOVIE_VERSION-src.zip"
TMPDIR=/tmp/qtlmovie-$$
rm -rf $TMPDIR
mkdir -p -m 0755 $TMPDIR/QtlMovie-$QTLMOVIE_VERSION
tar -C $ROOTDIR --exclude '*.dmg' --exclude .git --exclude 'build-QtlMovie-*' -c -p -f - . \
    | tar -C $TMPDIR/QtlMovie-$QTLMOVIE_VERSION -x -p -f -
$TMPDIR/QtlMovie-$QTLMOVIE_VERSION/build/cleanup.sh --deep
rm -f $ROOTDIR/installers/QtlMovie-$QTLMOVIE_VERSION-src.zip
(cd $TMPDIR; zip -rq9 $ROOTDIR/installers/QtlMovie-$QTLMOVIE_VERSION-src.zip QtlMovie-$QTLMOVIE_VERSION)
rm -rf $TMPDIR
