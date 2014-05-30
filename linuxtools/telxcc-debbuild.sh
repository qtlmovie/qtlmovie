#!/bin/bash
#
# This script builds a binary .deb for Telxcc.
# It is designed for Ubuntu and Debian Linux.
#

# Description of Telxcc:
TELXCC_VERSION=${1:-2.5.3}
TELXCC_SRC=telxcc-${TELXCC_VERSION}.zip
TELXCC_URL=https://github.com/forers/telxcc/archive/master.zip

# This script:
SCRIPT=$(basename $BASH_SOURCE)
SCRIPTDIR=$(cd $(dirname $BASH_SOURCE); pwd)

# Other directories:
ROOTDIR=$(dirname $SCRIPTDIR)
INSTDIR=$ROOTDIR/installers
TMPDIR=/tmp/telxcc-$$

# Error and cleanup handling
cleanup() { rm -rf $TMPDIR; exit $1; }
error() { echo >&2 "$SCRIPT: $*"; cleanup 1; }
trap "cleanup 1" SIGINT

# Download Telxcc sources if not yet present.
[ -e $INSTDIR/$TELXCC_SRC ] || wget -O $INSTDIR/$TELXCC_SRC $TELXCC_URL

# Build telxcc in a temporary directory.
rm -rf $TMPDIR
mkdir -p -m 0755 $TMPDIR/build
(cd $TMPDIR/build; unzip -q $INSTDIR/$TELXCC_SRC)
make -C $TMPDIR/build/telxcc-master || error "Error building telxcc"

# Build a temporary system root containing the installation
mkdir -p -m 0755 \
    $TMPDIR/sys/DEBIAN \
    $TMPDIR/sys/usr/bin \
    $TMPDIR/sys/usr/share/man/man1
sed -e "s/{VERSION}/$TELXCC_VERSION/" \
    -e "s/{ARCH}/$(dpkg-architecture -qDEB_BUILD_ARCH)/" \
    $SCRIPTDIR/telxcc.control >$TMPDIR/sys/DEBIAN/control
install -m 0755 $SCRIPTDIR/telxcc.postinst $TMPDIR/sys/DEBIAN/postinst
install -m 0755 $TMPDIR/build/telxcc-master/telxcc $TMPDIR/sys/usr/bin
strip $TMPDIR/sys/usr/bin/telxcc
install -m 0644 $TMPDIR/build/telxcc-master/telxcc.1 $TMPDIR/sys/usr/share/man/man1

# Build the binary package
dpkg --build $TMPDIR/sys $INSTDIR
cleanup 0
