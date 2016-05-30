#!/bin/bash
#
# This script builds a binary RPM and a source RPM for CCExtractor.
# It is designed for Fedora and Red Hat Enterprise Linux.
# It may work on other rpm-based distros.
#

# Description of CCExtractor:
CCEXTRACTOR_VERSION=${1:-0.80}
CCEXTRACTOR_RELEASE=0
CCEXTRACTOR_SRC=ccextractor.src.${CCEXTRACTOR_VERSION}.zip
CCEXTRACTOR_URL=http://sourceforge.net/projects/ccextractor/files/ccextractor/${CCEXTRACTOR_VERSION}/${CCEXTRACTOR_SRC}
CCEXTRACTOR_PATCH=ccextractor.${CCEXTRACTOR_VERSION}-linux.patch

# This script:
SCRIPT=$(basename $BASH_SOURCE)
SCRIPTDIR=$(cd $(dirname $BASH_SOURCE); pwd)
ROOTDIR=$(dirname $SCRIPTDIR)
INSTDIR=$ROOTDIR/installers
error() { echo >&2 "$SCRIPT: $*"; exit 1; }

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

# Download CCExtractor sources if not yet present.
[ -e $INSTDIR/$CCEXTRACTOR_SRC ] || wget --no-check-certificate -O $INSTDIR/$CCEXTRACTOR_SRC $CCEXTRACTOR_URL
cp -f $INSTDIR/$CCEXTRACTOR_SRC $RPMBUILD/SOURCES/$CCEXTRACTOR_SRC

# The content of the zip file should start with ccextractor.<version>
ZIPROOT=$(unzip -l $RPMBUILD/SOURCES/$CCEXTRACTOR_SRC | grep / | grep -v Archive: | sed -e 's/.* //' -e 's,/.*,,' | sort -u)
if [ "$ZIPROOT" != "ccextractor.$CCEXTRACTOR_VERSION" ]; then
    # Expand the zip file in a temporary directory.
    TMPDIR=/tmp/tmp.$$
    mkdir -p $TMPDIR/ccextractor.$CCEXTRACTOR_VERSION
    (cd $TMPDIR; unzip -q $RPMBUILD/SOURCES/$CCEXTRACTOR_SRC)
    # Move files into ccextractor.<version>
    for dir in $(find $TMPDIR -maxdepth 1 -type d ! -path $TMPDIR ! -name ccextractor.${CCEXTRACTOR_VERSION}); do
        mv $dir/* $TMPDIR/ccextractor.$CCEXTRACTOR_VERSION
    done
    # Rebuild a zip file.
    (cd $TMPDIR; zip -r -q $CCEXTRACTOR_SRC ccextractor.$CCEXTRACTOR_VERSION)
    mv $TMPDIR/$CCEXTRACTOR_SRC $RPMBUILD/SOURCES/$CCEXTRACTOR_SRC
    rm -rf $TMPDIR
fi

# Get patch if present
if [ -e $SCRIPTDIR/$CCEXTRACTOR_PATCH ]; then
    cp $SCRIPTDIR/$CCEXTRACTOR_PATCH $RPMBUILD/SOURCES/
    PATCHVAR=patch
else
    PATCHVAR=nopatch
fi

# Build CCExtractor rpm
rpmbuild -ba -D "version ${CCEXTRACTOR_VERSION}" -D "release ${CCEXTRACTOR_RELEASE}${DISTRO}" -D "$PATCHVAR $CCEXTRACTOR_PATCH" $SCRIPTDIR/ccextractor.spec
cp -f \
    $RPMBUILD/RPMS/*/ccextractor-${CCEXTRACTOR_VERSION}-${CCEXTRACTOR_RELEASE}${DISTRO}.*.rpm \
    $RPMBUILD/SRPMS/ccextractor-${CCEXTRACTOR_VERSION}-${CCEXTRACTOR_RELEASE}${DISTRO}.src.rpm \
    $ROOTDIR/installers
