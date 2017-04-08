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
#  Windows PowerShell script to clean up the project directory tree.
#  Back to a clean state of source files.
# 
#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Clean up the project directory tree, back to a clean state of source files.

 .DESCRIPTION

  Delete generated binary and documentation files.
  
  Delete Qt Creator system-specific files (accept proposed defaults when
  restarting Qt Creator).

  Delete spurious generated files in src tree which could have been left after
  an incorrect build command:
  - Recursively delete directories named release, Release, debug, Debug.
  - Delete object files, libraries, Qt metafiles, etc.

 .PARAMETER NoPause

  Do not wait for the user to press <enter> at end of execution. By default,
  execute a "pause" instruction at the end of execution, which is useful
  when the script was run from Windows Explorer.

 .PARAMETER Silent

  Do not report deleted files.

 .PARAMETER Deep

  Delete absolutely all non-source files, including the git repository and
  the content of the SourceForge mirror. This is typically done on a COPY
  of the project directory tree to create an archive of the source code.
#>
[CmdletBinding(SupportsShouldProcess=$true)]
param(
    [switch]$NoPause = $false,
    [switch]$Silent = $false,
    [switch]$Deep = $false
)

$RootDir = Split-Path -Parent $PSScriptRoot

# Note that we cannot pipe Get-ChildItem directly into Remove-Item since Get-ChildItem -Recurse
# return directories first, followed by their content. So, we first get the list of files and
# directories to remove and then we delete them, if not yet deleted by a previous recursion.

$files = @(Get-ChildItem -Recurse $RootDir | Where-Object {
    ($_.FullName -notlike '*\mactools\*') -and
    ($_.FullName -notlike '*\wintools*\*') -and
    ($_.FullName -notlike '*\installers\*') -and
    ($_.FullName -notlike '*\sourceforge\*') -and
    (($_.Name -like "build-*" -and (Test-Path $_.FullName -PathType Container)) -or
    $_.Name -like "debug" -or
    $_.Name -like "release" -or
    $_.Name -like "*~" -or
    $_.Name -like "*.pro.*" -or
    $_.Name -like "*.exe" -or
    $_.Name -like "makefile*" -or
    $_.Name -like "moc_*" -or
    $_.Name -like "*.obj" -or
    $_.Name -like "*.o" -or
    $_.Name -like "*.so" -or
    $_.Name -like "core" -or
    $_.Name -like "core.*" -or
    $_.Name -like "*.lib" -or
    $_.Name -like "*.dll" -or
    $_.Name -like "*.exe" -or
    $_.Name -like "*.qm" -or
    $_.Name -like "*.autosave" -or
    $_.Name -like "object_script.*.release" -or
    $_.Name -like "object_script.*.debug")
}).FullName

function Delete($file) {
    if ((Test-Path $file) -and $PSCmdlet.ShouldProcess($file,"Delete")) {
        if (-not $Silent) {
            "Deleting $file"
        }
        Remove-Item $file -Recurse -Force
    }
}

foreach ($file in $files) {
    Delete $file
}

if ($Deep) {
    Delete "$RootDir\.git"
    Delete "$RootDir\sourceforge\web\doc"
    Delete "$RootDir\sourceforge\web\doxy"
    Get-ChildItem -Recurse $RootDir | Where-Object {
        ($_.Name -like "*.exe" -or $_.Name -like "*.rpm" -or $_.Name -like "*.deb" -or $_.Name -like "*.dmg" -or $_.Name -like "*.zip")
    } | ForEach-Object { Delete $_.FullName }
    Get-ChildItem -Recurse (Join-Path $RootDir "mactools") | Where-Object {
        ($_.Name -notlike "*.txt" -and $_.Name -notlike ".gitignore")
    } | ForEach-Object { Delete $_.FullName }
}

if (-not $NoPause) {
    pause
}
