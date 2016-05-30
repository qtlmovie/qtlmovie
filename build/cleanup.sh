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

[ "$1" = "--deep" ] && DEEP=true || DEEP=false

ROOT=$(cd $(dirname $0)/..; pwd)

# Delete spurious generated files.
find $ROOT \
    \( -iname .git -prune \) -o \
    \( -iname 'wintools*' -prune \) -o \
    \( -iname mactools -prune \) -o \
    \( -iname installers -prune \) -o \
    \( -iname sourceforge -prune \) -o \
    \( \( -iname debug -o -iname release -o -iname 'build-*' \) -type d -print -prune \) -o \
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
       -iname '*.pro.*' -o \
       -iname '*.autosave' -o \
       -iname '.directory' -o \
       -iname 'object_script.*.release' -o \
       -iname 'object_script.*.debug' \) -print \
    | xargs rm -rvf

# Remove executable right from source files.
find $ROOT \( -iname 'build-*' -prune \) -o \( -iname .git -prune \) -o ! -type d ! -iname '*.sh' | xargs chmod 640
find $ROOT \( -iname .git -prune \) -o -type d -o -iname '*.sh' -o -iname '*.postinst' | xargs chmod 750
for f in $(find $ROOT/mactools -type f ! -name '.*' ! -name '*.txt'); do chmod 755 $f; done

# In deep mode, delete git repository, installers and non-original files in SourceForge mirror.
if $DEEP; then
    rm -rf $ROOT/.git $ROOT/sourceforge/web/doc $ROOT/sourceforge/web/doxy
    find $ROOT \
         -iname '*.exe' -o \
         -iname '*.rpm' -o \
         -iname '*.deb' -o \
         -iname '*.dmg' -o \
         -iname '*.zip' -o \
         -iname .DS_Store \
        | xargs rm -vf
    (cd $ROOT/mactools; rm -vf $(cat .gitignore))
fi

# Windowsify or unixify the end of lines on text files depending on their type.
find $ROOT -iname '*.ps1' -o -iname '*.psm1' -o -iname '*.reg' | xargs unix2dos -q
find $ROOT \
    -iname '*.cpp' -o \
    -iname '*.h' -o \
    -iname '*.txt' -o \
    -iname '*.pro' -o \
    -iname '*.pri' -o \
    -iname '*.js' -o \
    -iname '*.sh' -o \
    -iname '*.html' -o \
    -iname '*.ui' -o \
    -iname '*.ts' \
    | xargs dos2unix -q
