#!/bin/bash
#
# This script builds a binary RPM and a source RPM for Telxcc.
# It is designed for Fedora and Red Hat Enterprise Linux.
# It may work on other rpm-based distros.
#

# Description of Telxcc:
TELXCC_VERSION=2.5.3
TELXCC_RELEASE=0
TELXCC_SRC=telxcc-${TELXCC_VERSION}.zip
TELXCC_URL=https://github.com/forers/telxcc/archive/master.zip

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

# Download Telxcc sources if not yet present.
[ -e $INSTDIR/$TELXCC_SRC ] || wget -O $INSTDIR/$TELXCC_SRC $TELXCC_URL
cp -f $INSTDIR/$TELXCC_SRC $RPMBUILD/SOURCES/$TELXCC_SRC

# Build Telxcc rpm
rpmbuild -ba -D "version ${TELXCC_VERSION}" -D "release ${TELXCC_RELEASE}${DISTRO}" $SCRIPTDIR/telxcc.spec
cp -f \
    $RPMBUILD/RPMS/*/telxcc-${TELXCC_VERSION}-${TELXCC_RELEASE}${DISTRO}.*.rpm \
    $RPMBUILD/SRPMS/telxcc-${TELXCC_VERSION}-${TELXCC_RELEASE}${DISTRO}.src.rpm \
    $ROOTDIR/installers
