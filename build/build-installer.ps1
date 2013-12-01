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
#  Windows PowerShell script to build the binary installer for Windows.
#  Also build a source file zip archive.
# 
#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Build the binary installer for Windows.

 .DESCRIPTION

  Build the binary installer for Windows. The release version of the project is
  automatically rebuilt before building the installer.

 .PARAMETER ProductName

  Name of the product. The default is the base name of the unique project file
  in the same directory as this script or in src or ..\src subdirectory.

 .PARAMETER Version

  Version of the product. The default is extracted from the source file
  {ProductName}Version.h.

 .PARAMETER NoPause

  Do not wait for the user to press <enter> at end of execution. By default,
  execute a "pause" instruction at the end of execution, which is useful
  when the script was run from Windows Explorer.

 .PARAMETER NoBuild

  Do not rebuild the product, assumed to be still valid.

 .PARAMETER NoInstaller

  Do not build the binary installer. Still build the source archive.

 .PARAMETER NoSource

  Do not build the source archive. Still build the binary installer.
#>
[CmdletBinding()]
param(
    [Parameter(Position=1,Mandatory=$false)][string]$ProductName,
    [Parameter(Position=2,Mandatory=$false)][string]$Version,
    [switch]$NoPause = $false,
    [switch]$NoBuild = $false,
    [switch]$NoInstaller = $false,
    [switch]$NoSource = $false
)

# PowerShell execution policy.
Set-StrictMode -Version 3
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process -Force
Import-Module -Name (Join-Path $PSScriptRoot WindowsPowerShellTools.psm1)

# A function to exit this script.
function Exit-Script ([string]$Message = "")
{
    $Code = 0
    if ($Message -ne "") {
        Write-Output "ERROR: $Message"
        $Code = 1
    }
    if (-not $NoPause) {
        pause
    }
    exit $Code
}

# A function to search a file and exit if not found.
function Get-FileInPath ([string]$File, $SearchPath)
{
    $Path = Search-File $File $SearchPath
    if (-not $Path) {
        Exit-Script "$File not found"
    }
    return $Path
}

# Requires .NET 4.5 to build the source archive (need compression methods).
if (-not $NoSource) {
    $DotNetVersion = Get-DotNetVersion
    if ($DotNetVersion -lt 405) {
        $DotNetString = "v" + [int]($DotNetVersion / 100) + "." + ($DotNetVersion % 100)
        Exit-Script "Require .NET Framework v4.5, running $DotNetString"
    }
}

# Get the project directories.
$RootDir = (Split-Path -Parent $PSScriptRoot)
$SrcDir = (Join-Path $RootDir "src")
$ProjectFile = Get-QtProjectFile $SrcDir
$BuildDir = (Join-Path $RootDir "build-Win32-Release-Static")

# Get the product name.
if (-not $ProductName -and $ProjectFile) {
    $ProductName = (Get-Item $ProjectFile).BaseName
}
if (-not $ProductName) {
    Exit-Script "Product name not found"
}

# Get the product version.
if (-not $Version) {
    $VersionFile = Search-File "${ProductName}Version.h" ($SrcDir, (Join-Path $SrcDir $ProductName))
    if ($VersionFile) {
        $Version = (Get-Content $VersionFile | Select-String '^ *#define .*_VERSION  *"') -replace '^ *#define .*_VERSION  *"' -replace '".*$'
    }
}
if (-not $Version) {
    Exit-Script "Product version not found"
}

# Build the project in release mode.
if ($NoBuild) {
    Set-QtPath -Static
}
else {
    & (Join-Path $PSScriptRoot build.ps1) -Release -Static -NoPause
}

# Strip all executables.
Get-ChildItem -Recurse $BuildDir -Include *.exe | ForEach-Object { strip $_.FullName }

# Build the installer.
if (-not $NoInstaller) {

    # Locate Qt installation directory.
    $QtDir = Split-Path -Parent (Split-Path -Parent (Get-FileInPath qmake.exe $env:Path))

    # Locate NSIS, the Nullsoft Scriptable Installation System.
    $NsisExe = Get-FileInPath makensis.exe "$env:Path;C:\Program Files\NSIS;C:\Program Files (x86)\NSIS"
    $NsisScript = Get-FileInPath "${ProductName}.nsi" $PSScriptRoot

    # Build the 32-bit installer.
    & $NsisExe `
        "/DProductVersion=$Version" `
        "/DRootDir=$RootDir" `
        "/DBuildDir=$BuildDir" `
        "/DQtDir=$QtDir" `
        "$NsisScript"

    # Build the 64-bit installer.
    & $NsisExe `
        "/DWin64" `
        "/DProductVersion=$Version" `
        "/DRootDir=$RootDir" `
        "/DBuildDir=$BuildDir" `
        "/DQtDir=$QtDir" `
        "$NsisScript"
}

# Build the source archive.
if (-not $NoSource) {
    # Source archive name.
    $SrcArchive = (Join-Path $BuildDir "${ProductName}-${Version}-src.zip")
    $WintoolsArchive = (Join-Path $BuildDir "${ProductName}-${Version}-wintools.zip")

    # Create a temporary directory.
    $TempDir = New-TempDirectory

    Push-Location $TempDir
    try {
        # Copy project tree into temporary directory.
        $TempRoot = (Join-Path $TempDir "${ProductName}-${Version}")
        [void] (New-Item -ItemType Directory -Force $TempDir)
        Copy-Item $RootDir $TempRoot -Recurse

        # Create the wintools zip file.
        Get-ChildItem -Recurse (Join-Path $TempRoot wintools*) | New-ZipFile $WintoolsArchive -Force -Root $TempRoot

        # Cleanup the temporary tree.
        & (Join-Path (Join-Path $TempRoot build) cleanup.ps1) -Deep -NoPause -Silent

        # Create the source zip file.
        Get-ChildItem -Recurse (Split-Path $TempRoot) | New-ZipFile $SrcArchive -Force -Root $TempDir
    }
    finally {
        # Delete the temporary directory.
        Pop-Location
        if (Test-Path $TempDir) {
            Remove-Item $TempDir -Recurse -Force
        }
    }
}

Exit-Script
