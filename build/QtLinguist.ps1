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
#  Windows PowerShell script to start Qt Linguist after updating translation
#  files.
#
#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Start Qt Linguist after updating translation files.

 .DESCRIPTION

  Start Qt Linguist after updating all translation files in the project.

  This script is typically run from the Windows Explorer.

 .PARAMETER ProjectFile

  Optional Qt project file (.pro). If no project file is provided and one single
  .pro file exists in the same directory as this script or in src or ..\src
  subdirectory, it is used as project file.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory=$false,Position=1)][string]$ProjectFile
)

# PowerShell execution policy.
Set-StrictMode -Version 3
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process -Force
Import-Module -Name (Join-Path $PSScriptRoot WindowsPowerShellTools.psm1)

# Build a safe environment for the latest Qt version.
Set-QtPath

# Determine project file.
if (-not $ProjectFile) {
    $ProjectFile = Get-QtProjectFile ($PSScriptRoot, (Join-Path $PSScriptRoot src), (Join-Path (Split-Path -Parent $PSScriptRoot) src))
}
if (-not $ProjectFile) {
    # No project file, simply run Qt Linguist
    linguist
}
else {
    Push-Location (Split-Path -Parent $ProjectFile)
    # The utility lupdate.exe is considered as admin command by Windows since it
    # contains tbe word "update". Really stupid. Copy it as lupd.exe to avoid that.
    $LUpdate = Search-File lupdate.exe
    if ($LUpdate) {
        $LUpdateDir = (Split-Path -Parent $LUpdate)
        Copy-Item $LUpdate (Join-Path $LUpdateDir lupd.exe)
        # Update all translation files.
        lupd -no-obsolete $ProjectFile
    }
    linguist (Get-ChildItem . -Recurse -Include *.ts -Name)
    Pop-Location
}
