#!/bin/bash
#
# This script builds a binary RPM and a source RPM for CCExtractor.
# It is designed for Fedora and Red Hat Enterprise Linux.
# It may work on other rpm-based distros.
#

# Description of CCExtractor:
CCEXTRACTOR_VERSION=0.67
CCEXTRACTOR_RELEASE=0
CCEXTRACTOR_SRC=ccextractor.src.${CCEXTRACTOR_VERSION}.zip
CCEXTRACTOR_URL=http://sourceforge.net/projects/ccextractor/files/ccextractor/${CCEXTRACTOR_VERSION}/${CCEXTRACTOR_SRC}

# This script:
SCRIPT=$(basename $BASH_SOURCE)
SCRIPTDIR=$(dirname $BASH_SOURCE)
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
[ -e $RPMBUILD/SOURCES/$CCEXTRACTOR_SRC ] || wget -O $RPMBUILD/SOURCES/$CCEXTRACTOR_SRC $CCEXTRACTOR_URL

# Build CCExtractor rpm
rpmbuild -ba -D "version ${CCEXTRACTOR_VERSION}" -D "release ${CCEXTRACTOR_RELEASE}${DISTRO}" $SCRIPTDIR/ccextractor.spec
cp -f \
    $RPMBUILD/RPMS/*/ccextractor-${CCEXTRACTOR_VERSION}-${CCEXTRACTOR_RELEASE}${DISTRO}.*.rpm \
    $RPMBUILD/SRPMS/ccextractor-${CCEXTRACTOR_VERSION}-${CCEXTRACTOR_RELEASE}${DISTRO}.src.rpm \
    $SCRIPTDIR
