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
#  Unix script to clean up the project directory tree.
#  Back to a clean state of source files.
# 
#  With option --deep, delete absolutely all non-source files, including the
#  git repository and the content of the SourceForge mirror. This is typically
#  done on a COPY of the project directory tree to create an archive of the
#  source code.
#
#-----------------------------------------------------------------------------

DEEP=false
[ "$1" = "--deep" ] && DEEP=true

ROOT=$(cd $(dirname $0)/..; pwd)

# Delete generated binary and documentation files.
# Delete Qt Creator system-specific files (accept proposed defaults when restarting Qt Creator).
rm -rfv $ROOT/build-* $ROOT/src/*.pro.user*

# Delete spurious generated files in src tree. Could be left after incorrect build command.
# - Recursively delete directories named release, Release, debug, Debug.
# - Delete object files, libraries, Qt metafiles, etc.
find $ROOT \
    -type d -iname 'wintools*' -prune , \
    -type d -iname sourceforge -prune , \
    -type d \( -iname debug -o -iname release \) -exec rm -rfv {} \; -printf 'Deleted %p\n' -prune , \
    \( -iname 'Makefile*' -o \
       -iname 'moc_*' -o \
       -iname '*.obj' -o \
       -iname '*.o' -o \
       -iname '*.so' -o \
       -iname 'core' -o \
       -iname 'core.*' -o \
       -iname '*.lib' -o \
       -iname '*.dll' -o \
       -iname '*.exe' -o \
       -iname '*.qm' -o \
       -iname '*.log' -o \
       -iname '*.tmp' -o \
       -iname '*~' -o \
       -iname '*.autosave' -o \
       -iname '.directory' -o \
       -iname 'object_script.*.release' -o \
       -iname 'object_script.*.debug' \) \
    -exec rm {} \; -printf 'Deleted %p\n'

# Remove executable right from source files.
find $ROOT -iname 'build-*' -prune , ! -type d ! -iname '*.sh' -exec chmod 640 {} \;

# In deep mode, delete git repository and non-original files in SourceForge mirror.
if $DEEP; then
    rm -rf $ROOT/.git $ROOT/sourceforge/web/doc $ROOT/sourceforge/web/doxy
    find $ROOT \( -iname '*.exe' -o -iname '*.rpm' -o -iname '*.zip' \) -exec rm {} \; -printf 'Deleted %p\n'
fi

# Unixify the end of lines on text files.
find $ROOT \( \
    -iname '*.cpp' -o \
    -iname '*.h' -o \
    -iname '*.txt' -o \
    -iname '*.pro' -o \
    -iname '*.pri' -o \
    -iname '*.js' -o \
    -iname '*.sh' -o \
    -iname '*.ui' \
    \) -exec dos2unix -q {} \;
