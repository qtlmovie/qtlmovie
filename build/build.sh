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
#  Unix script to build the project.
# 
#  By default, build a release version. Use argument "-debug" to build the
#  debug version. Use "-all" to build both debug and release versions.
# 
#  Documentation is not generated here. Use build-doc.sh.
#
#-----------------------------------------------------------------------------

SCRIPT=$(basename $BASH_SOURCE)
usage() { echo >&2 "usage: $SCRIPT [-debug] [-release] [-all]"; exit 1; }
error() { echo >&2 "$SCRIPT: $*"; exit 1; }

# Decode command line.
BUILD_DEBUG=false
BUILD_RELEASE=false
while [[ $# -gt 0 ]]; do
    case $(tr <<<$1 A-Z a-z) in
        -debug) BUILD_DEBUG=true ;;
        -release) BUILD_RELEASE=true ;;
        -all) BUILD_RELEASE=true; BUILD_DEBUG=true ;;
        *) usage ;;
    esac
    shift
done

# Default build is release.
$BUILD_DEBUG || BUILD_RELEASE=true

# Various directories.
SCRIPTDIR=$(cd $(dirname $BASH_SOURCE); pwd)
ROOTDIR=$(dirname $SCRIPTDIR)
SRCDIR=$ROOTDIR/src

# Project file. Must be exactly one in source directory.
PROJECT_FILE=
[[ $(ls $SRCDIR/*.pro 2>/dev/null | wc -l) -eq 1 ]] && PROJECT_FILE=$(ls $SRCDIR/*.pro)
[[ -z "$PROJECT_FILE" ]] && error "unique project file not found in $SRCDIR"
PROJECT_NAME=$(basename $PROJECT_FILE .pro)

# Use parallel processes up to the number of logical processors minus 1.
if [[ "$(uname -s | tr A-Z a-z)" == "darwin" ]]; then
    NCPU=$(sysctl -n hw.ncpu 2>/dev/null)
else
    NCPU=$(nproc 2>/dev/null)
fi
NPROCESS=${NCPU:-1}
[[ $NPROCESS -gt 1 ]] && NPROCESS=$(($NPROCESS - 1))

# Use similar build directory naming as Qt Creator "shadow build" mode.
BUILDDIR_BASE=$ROOTDIR/build-${PROJECT_NAME}-Desktop
BUILDDIR_RELEASE=${BUILDDIR_BASE}-Release
BUILDDIR_DEBUG=${BUILDDIR_BASE}-Debug

# Search for Qt installations out of system directories.
source ${SCRIPTDIR}/QtSetEnvironment.rc

# If the command qmake-qt5 exists, use it. Otherwise, use qmake.
QMAKE=$(which qmake-qt5 2>/dev/null)
QMAKE=${QMAKE:-qmake}

# Build release and/or debug releases.
if $BUILD_RELEASE; then
    mkdir -p $BUILDDIR_RELEASE
    cd $BUILDDIR_RELEASE
    $QMAKE $PROJECT_FILE -r CONFIG+=release && make -j${NPROCESS} --no-print-directory -k
fi
if $BUILD_DEBUG; then
    mkdir -p $BUILDDIR_DEBUG
    cd $BUILDDIR_DEBUG
    $QMAKE $PROJECT_FILE -r CONFIG+=debug && make -j${NPROCESS} --no-print-directory -k
fi
