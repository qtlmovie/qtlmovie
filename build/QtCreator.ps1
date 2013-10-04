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
#  Windows PowerShell script to start Qt Creator with a clean Path.
#  The default Path may include UnxUtils and interferes with qmake. Use this
#  script if build commands fail when launching Qt Creator from the system menu.
# 
#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Start Qt Creator with a clean Path.

 .DESCRIPTION

  Start Qt Creator with a clean Path. The default Path may include UnxUtils and
  interferes with qmake. Use this script if build commands fail when launching
  Qt Creator from the system menu.

  This script is typically run from the Windows Explorer.

 .PARAMETER ProjectFile

  Optional Qt project file (.pro). If no project file is provided and one single
  .pro file exists in the same directory as this script or in src or ..\src
  subdirectory, it is used as project file.

 .PARAMETER NoHide

  Do not hide the PowerShell command Window. By default, the command window is
  hidden, which is useful when the script was run from Windows Explorer.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory=$false,Position=1)][string]$ProjectFile,
    [switch]$NoHide = $false
)

# PowerShell execution policy.
Set-StrictMode -Version 3
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process -Force
Import-Module -Name (Join-Path $PSScriptRoot WindowsPowerShellTools.psm1)

# Immediately hide the PowerShell window
if (-not $NoHide) {
    Show-PowerShellWindow $SW_HIDE
}

# Build a safe environment for the latest Qt version.
Set-QtPath

# Determine project file.
if (-not $ProjectFile) {
    $ProjectFile = Get-QtProjectFile ($PSScriptRoot, (Join-Path $PSScriptRoot src), (Join-Path (Split-Path -Parent $PSScriptRoot) src))
}

# Start Qt Creator.
$env:MAKEFLAGS = "-j4 -k"
if (-not $ProjectFile) {
    QtCreator
}
else {
    Push-Location (Split-Path -Parent $ProjectFile)
    QtCreator $ProjectFile
    Pop-Location
}
