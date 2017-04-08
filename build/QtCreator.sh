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
#  Unix script to start Qt Creator.
#  If an argument is provided, it is used as project file. If no argument is
#  provided and one single .pro file exists in the same directory as this
#  script or in src or ../src subdirectory, it is used as project file.
#
#-----------------------------------------------------------------------------

# Setup Qt environment.
SCRIPT_DIR=$(cd $(dirname $0); pwd)
source $SCRIPT_DIR/QtSetEnvironment.rc

# Search project file.
PROJ=$1
if [[ -z "$PROJ" && $(ls $SCRIPT_DIR/*.pro 2>/dev/null | wc -l) -eq 1 ]]; then
    # No command line argument and one single project file.
    PROJ=$(ls $SCRIPT_DIR/*.pro)
fi
if [[ -z "$PROJ" && $(ls $SCRIPT_DIR/src/*.pro 2>/dev/null | wc -l) -eq 1 ]]; then
    # No project file so far and one single in src.
    PROJ=$(ls $SCRIPT_DIR/src/*.pro)
fi
if [[ -z "$PROJ" && $(ls $SCRIPT_DIR/../src/*.pro 2>/dev/null | wc -l) -eq 1 ]]; then
    # No project file so far and one single in ../src.
    PROJ=$(ls $SCRIPT_DIR/../src/*.pro)
fi

# Start Qt Creator
export MAKEFLAGS="-j4 -k"
if [[ -z "$PROJ" ]]; then
    "$QTCREATOR"
else
    cd $(dirname "$PROJ")
    "$QTCREATOR" "$PROJ"
fi
