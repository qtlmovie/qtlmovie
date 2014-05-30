#!/bin/bash
#
# This script builds a binary .deb for CCextractor.
# It is designed for Ubuntu and Debian Linux.
#

# Description of Ccextractor:
CCEXTRACTOR_VERSION=${1:-0.69}
CCEXTRACTOR_SRC=ccextractor.src.${CCEXTRACTOR_VERSION}.zip
CCEXTRACTOR_URL=http://sourceforge.net/projects/ccextractor/files/ccextractor/${CCEXTRACTOR_VERSION}/${CCEXTRACTOR_SRC}

# This script:
SCRIPT=$(basename $BASH_SOURCE)
SCRIPTDIR=$(cd $(dirname $BASH_SOURCE); pwd)

# Other directories:
ROOTDIR=$(dirname $SCRIPTDIR)
INSTDIR=$ROOTDIR/installers
TMPDIR=/tmp/ccextractor-$$

# Error and cleanup handling
cleanup() { rm -rf $TMPDIR; exit $1; }
error() { echo >&2 "$SCRIPT: $*"; cleanup 1; }
trap "cleanup 1" SIGINT

# Download Ccextractor sources if not yet present.
[ -e $INSTDIR/$CCEXTRACTOR_SRC ] || wget -O $INSTDIR/$CCEXTRACTOR_SRC $CCEXTRACTOR_URL

# Build ccextractor in a temporary directory.
rm -rf $TMPDIR
mkdir -p -m 0755 $TMPDIR/build
(cd $TMPDIR/build; unzip -q $INSTDIR/$CCEXTRACTOR_SRC)
(cd $TMPDIR/build/ccextractor.$CCEXTRACTOR_VERSION/linux; ./build) || error "Error building ccextractor"

# Build a temporary system root containing the installation
mkdir -p -m 0755 $TMPDIR/sys/DEBIAN $TMPDIR/sys/usr/bin
sed -e "s/{VERSION}/$CCEXTRACTOR_VERSION/" \
    -e "s/{ARCH}/$(dpkg-architecture -qDEB_BUILD_ARCH)/" \
    $SCRIPTDIR/ccextractor.control >$TMPDIR/sys/DEBIAN/control
install -m 0755 $SCRIPTDIR/ccextractor.postinst $TMPDIR/sys/DEBIAN/postinst
install -m 0755 $TMPDIR/build/ccextractor.$CCEXTRACTOR_VERSION/linux/ccextractor $TMPDIR/sys/usr/bin
strip $TMPDIR/sys/usr/bin/ccextractor

# Build the binary package
dpkg --build $TMPDIR/sys $INSTDIR
cleanup 0
