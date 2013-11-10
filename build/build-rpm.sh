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
#  Unix script to build the RPM package for QtlMovie.
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
QTLMOVIE_RELEASE=0

# User's rpmbuild environment:
RPMBUILD=$HOME/rpmbuild
if [ ! -d $RPMBUILD ]; then
    [ -z "$(which rpmdev-setuptree 2>/dev/null)" ] && error "rpmdev-setuptree not found, install package rpmdevtools"
    rpmdev-setuptree
    [ ! -d $RPMBUILD ] && error "$RPMBUILD not found"
fi

# Distro suffix for rpm files.
FC_DISTRO=$(grep " release " /etc/fedora-release 2>/dev/null | sed -e 's/^.* release \([0-9\.]*\) .*$/\1/')
RH_DISTRO=$(grep " release " /etc/redhat-release 2>/dev/null | sed -e 's/^.* release \([0-9\.]*\) .*$/\1/')
if [ -n "$FC_DISTRO" ]; then
    DISTRO=".fc$FC_DISTRO"
elif [ -n "$RH_DISTRO" ]; then
    DISTRO=".rh$RH_DISTRO"
else
    DISTRO=
fi

# Build a source archive of the project source.
QTLMOVIE_SRC=QtlMovie-${QTLMOVIE_VERSION}-src.zip
TMPDIR=/tmp/qtlmovie-rpmbuild-$$
TMPROOT=$TMPDIR/QtlMovie-${QTLMOVIE_VERSION}
mkdir -p $TMPROOT || exit 1
tar -C $ROOTDIR -cpf - --exclude=.git --exclude="*.exe" --exclude="*.rpm" . | tar -C $TMPROOT -xpf -
$TMPROOT/build/cleanup.sh >/dev/null
rm -f $BUILDDIR/$QTLMOVIE_SRC
(cd $TMPDIR; zip -r -9 -q $BUILDDIR/$QTLMOVIE_SRC QtlMovie-${QTLMOVIE_VERSION})

# Build QtlMovie rpm
cp -f $BUILDDIR/$QTLMOVIE_SRC $RPMBUILD/SOURCES/$QTLMOVIE_SRC
rpmbuild -ba -D "version ${QTLMOVIE_VERSION}" -D "release ${QTLMOVIE_RELEASE}${DISTRO}" $SCRIPTDIR/QtlMovie.spec
cp -f \
    $RPMBUILD/RPMS/*/qtlmovie-${QTLMOVIE_VERSION}-${QTLMOVIE_RELEASE}${DISTRO}.*.rpm \
    $RPMBUILD/SRPMS/qtlmovie-${QTLMOVIE_VERSION}-${QTLMOVIE_RELEASE}${DISTRO}.src.rpm \
    $BUILDDIR
