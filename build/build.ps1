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
#  Windows PowerShell script to build the project.
# 
#  Documentation is not generated here. See build-doc.ps1.
# 
#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Build the project (compile and link the executable).

 .DESCRIPTION

  Build the project (compile and link the executable). The sources of the
  project shall be in directory ..\src from this script. There must be one
  single Qt project file (.pro ) in ..\src.

  This script is for general purpose and can be used in any project provided
  that the above requirements are met.

  The release and debug versions are built in ..\build-Win32-Release and
  ..\build-Win32-Debug respectively.

 .PARAMETER Debug

  Build the debug version. If neither -Release nor -Debug is provided,
  build both versions.

 .PARAMETER Release

  Build the release version. If neither -Release nor -Debug is provided,
  build both versions.

 .PARAMETER NoPause

  Do not wait for the user to press <enter> at end of execution. By default,
  execute a "pause" instruction at the end of execution, which is useful
  when the script was run from Windows Explorer.
#>
param(
    [switch]$Release = $false,
    [switch]$Debug = $false,
    [switch]$NoPause = $false
)

# PowerShell execution policy.
Set-StrictMode -Version 3
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process -Force
Import-Module -Name (Join-Path $PSScriptRoot WindowsPowerShellTools.psm1)

# Build a safe environment for the latest Qt version.
Set-QtPath

# Default arguments: build all.
if (-not $Release -and -not $Debug) {
    $Release = $true
    $Debug = $true
}

# Project file
$RootDir=(Split-Path -Parent $PSScriptRoot)
$ProjectFile = Get-QtProjectFile (Join-Path $RootDir src)
if ($ProjectFile -eq $null) {
    Write-Error "No project file found"
}
else {
    # Build directories for release and debug.
    $BuildDirBase = (Join-Path $RootDir "build-Win32")
    $BuildDirRelease = $BuildDirBase + "-Release"
    $BuildDirDebug = $BuildDirBase + "-Debug"

    # Build project in release mode.
    if ($Release) {
        Write-Output "Building Release version..."
        [void] (New-Item -ItemType Directory -Force $BuildDirRelease)
        Push-Location $BuildDirRelease
        qmake $ProjectFile -r -spec win32-g++
        mingw32-make -j4 --no-print-directory -k
        Pop-Location
    }

    # Build project in debug mode.
    if ($Debug) {
        Write-Output "Building Debug version..."
        [void] (New-Item -ItemType Directory -Force $BuildDirDebug)
        Push-Location $BuildDirDebug
        qmake $ProjectFile -r -spec win32-g++ CONFIG+=debug CONFIG+=declarative_debug
        mingw32-make -j4 --no-print-directory -k
        Pop-Location
    }
}

if (-not $NoPause) {
    pause
}
