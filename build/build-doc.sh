#!/bin/bash
#-----------------------------------------------------------------------------
# 
#  Copyright (c) 2013-2017, Thierry Lelegard
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
#  Unix script to build the documentation using Doxygen.
# 
#-----------------------------------------------------------------------------

SCRIPT=$(basename $BASH_SOURCE)
SCRIPTDIR=$(cd $(dirname $BASH_SOURCE); pwd)

usage() { echo >&2 "usage: $SCRIPT [-n]"; exit 1; }
error() { echo >&2 "$SCRIPT: $*"; exit 1; }

ROOTDIR=$(dirname $SCRIPTDIR)
SRCDIR="$ROOTDIR/src"
DOXYDIR="$ROOTDIR/build-doxygen/html"
INDEX="$DOXYDIR/index.html"

# With option "-n", do not try to open a browser to view the generated files.
[[ "$1" == "-n" ]] && OPEN_BROWSER=false || OPEN_BROWSER=true

# QtlMovie version is extracted from QtlMovieVersion.h
export PROJECT_NUMBER=$(grep QTLMOVIE_VERSION $SRCDIR/QtlMovie/QtlMovieVersion.h | sed -e 's/[^"]*"//' -e 's/".*//')

# Generate documentation.
cd $(dirname $BASH_SOURCE)
echo Running Doxygen...
doxygen || exit
[[ -f "$INDEX" ]] || error "$INDEX not built"

# Try to find a browser to open the generated documentation.
# Safari and Firefox automatically reuse an existing instance, if one is open.
if $OPEN_BROWSER; then
    if [[ "$(uname -s | tr A-Z a-z)" == "darwin" ]]; then # macOS
        open -a Safari "$INDEX"
    else
        FIREFOX=$(which firefox 2>/dev/null)
        [[ -n "$FIREFOX" ]] && "$FIREFOX" "$INDEX" &
    fi
fi
