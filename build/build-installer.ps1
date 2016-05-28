#-----------------------------------------------------------------------------
# 
#  Copyright (c) 2013-2015, Thierry Lelegard
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

  Build the binary installers for Windows.

 .DESCRIPTION

  Build the binary installers for Windows. The release version of the project
  is automatically rebuilt before building the installer.

  By default, installers are built for 32-bit and 64-bit systems, full
  executable binary installers, standalone binaries (without admin rights),
  source code and wintools archives.

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

  Do not build the binary installer.

 .PARAMETER NoStandalone

  Do not build the standalone binaries.

 .PARAMETER NoSource

  Do not build the source archive.
#>
[CmdletBinding()]
param(
    [Parameter(Position=1,Mandatory=$false)][string]$ProductName,
    [Parameter(Position=2,Mandatory=$false)][string]$Version,
    [switch]$NoPause = $false,
    [switch]$NoBuild = $false,
    [switch]$NoInstaller = $false,
    [switch]$NoStandalone = $false,
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
$InstallerDir = (Join-Path $RootDir "installers")
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

# Locate Qt installation directory.
$QtDir = Split-Path -Parent (Split-Path -Parent (Get-FileInPath qmake.exe $env:Path))

# Build the installers.
if (-not $NoInstaller) {
    # Locate NSIS, the Nullsoft Scriptable Installation System.
    $NsisExe = Get-FileInPath makensis.exe "$env:Path;C:\Program Files\NSIS;C:\Program Files (x86)\NSIS"
    $NsisScript = Get-FileInPath "${ProductName}.nsi" $PSScriptRoot

    # Build the 32-bit installer.
    & $NsisExe `
        "/DProductVersion=$Version" `
        "/DRootDir=$RootDir" `
        "/DBuildDir=$BuildDir" `
        "/DInstallerDir=$InstallerDir" `
        "/DQtDir=$QtDir" `
        "$NsisScript"

    # Build the 64-bit installer.
    & $NsisExe `
        "/DWin64" `
        "/DProductVersion=$Version" `
        "/DRootDir=$RootDir" `
        "/DBuildDir=$BuildDir" `
        "/DInstallerDir=$InstallerDir" `
        "/DQtDir=$QtDir" `
        "$NsisScript"
}

# Build the standalone binaries.
if (-not $NoStandalone) {
    # Create a temporary directory.
    $TempDir = New-TempDirectory

    Push-Location $TempDir
    try {
        # Copy product files in standalone tree.
        $TempRoot = New-Directory @($TempDir, $ProductName)
        Copy-Item (Join-MultiPath @($BuildDir, "QtlMovie", "QtlMovie.exe")) $TempRoot
        Copy-Item (Join-MultiPath @($RootDir, "build", "qt.conf")) $TempRoot
        Copy-Item (Join-MultiPath @($RootDir, "LICENSE.txt")) $TempRoot
        Copy-Item (Join-MultiPath @($RootDir, "CHANGELOG.txt")) $TempRoot

        $TempTranslations = New-Directory @($TempRoot, "translations")
        Get-ChildItem (Join-Path $QtDir "translations") -Filter "qt*_fr.qm" | ForEach-Object { Copy-Item $_.FullName $TempTranslations }
        Copy-Item (Join-MultiPath @($BuildDir, "Qtl", "locale", "qtl_fr.qm")) $TempTranslations
        Copy-Item (Join-MultiPath @($BuildDir, "Qts", "locale", "qts_fr.qm")) $TempTranslations
        Copy-Item (Join-MultiPath @($BuildDir, "QtlMovie", "locale", "qtlmovie_fr.qm")) $TempTranslations

        $TempFonts = New-Directory @($TempRoot, "fonts")
        Copy-Item (Join-MultiPath @($RootDir, "fonts", "fonts.conf.template")) $TempFonts
        Copy-Item (Join-MultiPath @($RootDir, "fonts", "subfont.ttf")) $TempFonts

        # Copy 32-bit Windows tools.
        $TempWintools = New-Directory @($TempRoot, "wintools")
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "ffprobe.exe")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "ffmpeg.exe")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "ffmpeg.txt")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "dvdauthor.exe")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "dvdauthor.txt")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "mkisofs.exe")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "mkisofs.txt")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "growisofs.exe")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "growisofs.txt")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "ccextractor.exe")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "ccextractor.txt")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "dvddecrypter.exe")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools", "dvddecrypter.txt")) $TempWintools

        # Create the 32-bit standalone zip file.
        Get-ChildItem -Recurse $TempDir | New-ZipFile (Join-Path $InstallerDir "${ProductName}-Win32-Standalone-${Version}.zip") -Force -Root $TempDir

        # Overwrite 64-bits tools.
        Copy-Item (Join-MultiPath @($RootDir, "wintools64", "ffprobe.exe")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools64", "ffmpeg.exe")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools64", "ffmpeg.txt")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools64", "ccextractor.exe")) $TempWintools
        Copy-Item (Join-MultiPath @($RootDir, "wintools64", "ccextractor.txt")) $TempWintools

        # Create the 64-bit standalone zip file.
        Get-ChildItem -Recurse $TempDir | New-ZipFile (Join-Path $InstallerDir "${ProductName}-Win64-Standalone-${Version}.zip") -Force -Root $TempDir
    }
    finally {
        # Delete the temporary directory.
        Pop-Location
        if (Test-Path $TempDir) {
            Remove-Item $TempDir -Recurse -Force
        }
    }
}

# Build the source archives.
if (-not $NoSource) {
    # Source archive name.
    $SrcArchive = (Join-Path $InstallerDir "${ProductName}-${Version}-src.zip")
    $WintoolsArchive = (Join-Path $InstallerDir "${ProductName}-${Version}-wintools.zip")

    # Create a temporary directory.
    $TempDir = New-TempDirectory

    Push-Location $TempDir
    try {
        # Copy project tree into temporary directory.
        $TempRoot = (Join-Path $TempDir "${ProductName}-${Version}")
        Copy-Item $RootDir $TempRoot -Recurse

        # Create the wintools zip file.
        Get-ChildItem -Recurse (Join-Path $TempRoot wintools*) | New-ZipFile $WintoolsArchive -Force -Root $TempRoot

        # Cleanup the temporary tree.
        & (Join-MultiPath @($TempRoot, "build", "cleanup.ps1")) -Deep -NoPause -Silent

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
