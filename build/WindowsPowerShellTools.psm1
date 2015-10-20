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
#  Windows PowerShell module with various tools.
# 
#  Sample usage when installed in the same directory as the calling script:
#
#    Import-Module -Name (Join-Path $PSScriptRoot WindowsPowerShellTools.psm1)
#
#  Provided functions:
#
#    ConvertFrom-ExcelWorkSheet
#    ConvertFrom-HtmlString
#    ConvertTo-HtmlString
#    ConvertTo-String
#    Get-DotNetVersion
#    Get-QtProjectFile
#    New-TempDirectory
#    New-ZipFile
#    Search-File
#    Set-QtPath
#    Show-PowerShellWindow
#    Split-DeepString
# 
#-----------------------------------------------------------------------------


<#
 .SYNOPSIS

  Convert an Excel worksheet into PowerShell hashtables.

 .DESCRIPTION

  Convert an Excel worksheet into PowerShell hashtables. Each row of the
  worksheet is emitted as a hash table on the output. Each cell of the row
  is an element of the hash table. All hashtables have the same number of
  elements, corresponding to the "width" of the used area of the worksheet.
  
  By default, the first line of the worksheet is considered as a header line.
  The value of each cell names the corresponding column and is used as the
  key in the hash table of all subsequent rows. The first line is consequently
  not output has a hashtable. If a cell is empty in the header line, the key
  of the column is an integer indicating the column number, starting at 1.
  The option -NoHeader cancels the interpretation of the first line as a list
  of keys.

 .PARAMETER Path

  Input Excel file.

 .PARAMETER Sheet

  Worksheet name or index (starting at 1) in the Excel file. Default to the
  first worksheet in the file.

 .PARAMETER NoHeader

  Do not consider the first row as a header line. It is considered as a
  normal data line and is emitted on output. In this case, the key of each
  column is an integer indicating the column number, starting at 1.

 .INPUTS

  One or more Excel file names. An alternative to specifying the -Path
  parameter.

 .OUTPUTS

  Each row of the worksheet is emitted as a hash table on the output. Each
  cell of the row is an element of the hash table. All hashtables have the
  same number of elements, corresponding to the "width" of the used area of
  the worksheet.
#>
function ConvertFrom-ExcelWorkSheet()
{
    [CmdletBinding()]
    param(
        [Parameter(ValueFromPipeline=$true,Mandatory=$true)] [String] $Path,
        [Parameter(Mandatory=$false)] $Sheet = 1,
        [switch] $NoHeader = $false
    )
    begin {
        $Excel = New-Object -ComObject Excel.Application
        $Excel.Visible = $false
    }
    process {
        $Headers = @()
        # Open the Excel file in Read-Only mode, return a Workbook object.
        $WorkBook = $Excel.Workbooks.Open($Path, 2, $true)
        # Open specified worksheet, return a Worksheet object.
        $WorkSheet = $WorkBook.Worksheets.Item($Sheet)
        # Loop on each row
        foreach ($Row in $WorkSheet.UsedRange.Rows) {
            if ($Row.Row -eq 1 -and -not $NoHeader) {
                # Read the header line.
                $Headers = @($Row.Cells | ForEach-Object {$_.Value()})
            }
            else {
                # Read a data line.
                $Line = @{}
                foreach ($Cell in $Row.Cells) {
                    $col = $Cell.Column
                    if ($col -lt 1 -or $col -gt $Headers.Length -or $Headers[$col-1] -eq $null) {
                        $key = $col
                    }
                    else {
                        $key = $Headers[$col-1]
                    }
                    if ($Cell.Value() -ne $null) {
                        $Line[$key] = $Cell.Value()
                    }
                }
                $Line
            }

        }
        # Close the workbook without saving changes (should not be any).
        $WorkBook.Close($false)
    }
    end {
        $Excel.Quit()
    }
}

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Decode an HTML string, resolving HTML escaped characters.

 .PARAMETER Html

  An HTML string.

 .OUTPUTS

  The string with all HTML escaped characters decoded.
#>
function ConvertFrom-HtmlString()
{
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true,Position=1)] [String] $Html
    )
    Add-Type -AssemblyName System.Web
    return [System.Web.HttpUtility]::HtmlDecode($Html)
}

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Convert a string to HTML, escaping HTML-reserved characters.

 .PARAMETER Plain

  An object to convert to an HTML string.

 .OUTPUTS

  The string with all HTML-reserved characters escaped.
#>
function ConvertTo-HtmlString()
{
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true,Position=1)] [AllowNull()] $Plain
    )
    Add-Type -AssemblyName System.Web
    return [System.Web.HttpUtility]::HtmlEncode((ConvertTo-String $Plain))
}

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Convert any object, including $null, to a string.

 .PARAMETER Object

  An object to convert to a string.

 .INPUTS

  One or more Objects. An alternative to specifying the -Object parameter.

 .OUTPUTS

  A formatted string for each input object.
#>
function ConvertTo-String()
{
    [CmdletBinding()]
    param(
        [Parameter(ValueFromPipeline=$true,Mandatory=$false)] [AllowNull()] $Object
    )
    process {
        if ($Object -eq $null) {
            return "";
        }
        elseif ($Object.GetType().IsArray) {
            $result = "";
            foreach ($element in $Object) {
                if ($result -ne "") {
                    $result += ", "
                }
                $result += ConvertTo-String $element
            }
            return "($result)"
        }
        elseif ($Object.GetType().Name -eq "hashtable") {
            $result = "";
            foreach ($enum in $Object.GetEnumerator() | Sort Name) {
                if ($result -ne "") {
                    $result += "; "
                }
                $result += (ConvertTo-String $enum.Name)
                $result += "="
                $result += (ConvertTo-String $enum.Value)
            }
            return "{$result}"
        }
        else {
            return $Object.ToString()
        }
    }
}

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Get the version of the .NET framework as an integer.

 .DESCRIPTION

  Get the version of the .NET framework as an integer in the form
  100 * Major + Minor.

.EXAMPLE

  PS> Get-DotNetVersion
  405

  Running .NET framework version 4.5.
#>
function Get-DotNetVersion()
{
    (@(([Environment]::Version.Major * 100) + [Environment]::Version.Minor) +
    (Get-ChildItem 'HKLM:\SOFTWARE\Microsoft\NET Framework Setup\NDP' -Recurse |
        Get-ItemProperty -Name Version -ErrorAction SilentlyContinue |
        Where { $_.PSChildName -match '^([Cc]lient|[Ff]ull|[Vv]\d)'} |
        ForEach-Object {
            $fields = ($_.Version -split '.',0,"SimpleMatch") + (0, 0)
            (($fields[0] -as [int]) * 100) + ($fields[1] -as [int])
        }) |
    Measure-Object -Maximum).Maximum
}

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Get the unique Qt project file in a specified directory.

 .DESCRIPTION

  If the specified directory contains one single .pro file, the
  path of this file is returned. Return $null is no project file or
  more than one is found in the directory.

 .PARAMETER Directories

  A list of directories to search for a project file. Can be a string or an array
  of strings. Each string may contain a semi-colon-separated list of directories.
  By default, use the current location.
#>
function Get-QtProjectFile
{
    param(
        $Directories = (Get-Location)
    )

    # Loop on all provided directories...
    foreach ($dir in $(Split-DeepString $Directories ";" -Trim -IgnoreEmpty)) {
        # If directory exists...
        if (Test-Path $dir -PathType Container) {
            # Get the list of Qt project files in directory.
            $files = @(Get-ChildItem -Path $dir -Name *.pro)
            # Return unique project file.
            if ($files.Count -eq 1) {
                return (Join-Path $dir $files[0])
            }
            # If more than one found, this is ambiguous, do not continue.
            if ($files.Count -gt 1) {
                return $null
            }
        }
    }
    # No suitable file found.
    return $null
}

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Create a temporary directory.

 .DESCRIPTION

  Create a temporary directory and return its name.
#>
function New-TempDirectory
{
    # The function GetTempFileName() creates a temporary file.
    $TempFile = [System.IO.Path]::GetTempFileName()
    try {
        # Derive a name for a temporary directory.
        $TempDir = [System.IO.Path]::ChangeExtension($TempFile, ".dir")
        # Create the temporary directory.
        [void] (New-Item -ItemType Directory -Force $TempDir)
        return $TempDir
    }
    finally {
        # Delete the temporary file, now useless.
        if (Test-Path $TempFile) {
            Remove-Item $TempFile -Force
        }
    }
}

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Create a zip file from any files piped in.

 .PARAMETER Path

  The name of the zip archive to create.

 .PARAMETER Root

  Store directory names in the zip entries. Use the same hierarchy as input
  files but strip the root from their full name. If unspecified, create a
  flat archive of files without hierarchy.

 .PARAMETER Force

  If specified, delete the zip archive if it already exists.

 .EXAMPLE

  PS > dir *.ps1 | New-ZipFile scripts.zip
  Copies all PS1 files in the current directory to scripts.zip

 .EXAMPLE

  PS > "readme.txt" | New-ZipFile docs.zip
  Copies readme.txt to docs.zip

 .NOTES

  Initial version from Windows PowerShell Cookbook (O'Reilly)
  by Lee Holmes (http://www.leeholmes.com/guide)
  with additional options.

  This function requires Microsoft .NET Framework version 4.5 or higher.
  See http://www.microsoft.com/en-us/download/details.aspx?id=30653 or, for
  a full offline package, http://go.microsoft.com/fwlink/?LinkId=225702
  (dotnetfx45_full_x86_x64.exe).
#>
function New-ZipFile
{
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true,Position=1)][String] $Path,
        [Parameter(Mandatory=$false,Position=2)][String] $Root = $null,
        [Parameter(ValueFromPipeline=$true)] $input,
        [Switch] $Force
    )

    Set-StrictMode -Version 3

    # Check if the file exists already.
    $ZipName = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Path)
    if (Test-Path $ZipName) {
        if ($Force) {
            Remove-Item $ZipName -Force
        }
        else {
            # Zip file exists and no -Force option, generate an error.
            throw "$ZipName already exists."
        }
    }

    # Build the root path, i.e. the path to strip from entries directory path.
    if ($Root) {
        $Root = (Get-Item $Root).FullName
    }

    # Add the DLL that helps with file compression.
    # This requires .NET 4.5 (FileNotFoundException on previous releases).
    Add-Type -Assembly System.IO.Compression.FileSystem

    try {
        # Open the Zip archive
        $archive = [System.IO.Compression.ZipFile]::Open($ZipName, "Create")

        # Go through each file in the input, adding it to the Zip file specified
        foreach ($file in $input) {
            $item = $file | Get-Item
            # Skip the current file if it is the zip file itself
            if ($item.FullName -eq $ZipName) {
                continue
            }
            # Skip directories
            if ($item.PSIsContainer) {
                continue
            }
            # Compute entry name in archive.
            if ($Root -and $item.FullName -like "$Root\*") {
                $name = $item.FullName.Substring($Root.Length + 1)
            }
            else {
                $name = $item.Name
            }
            # Add the file to the archive.
            $null = [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($archive, $item.FullName, $name)
        }
    }
    finally {
        # Close the file
        $archive.Dispose()
        $archive = $null
    }
}

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Search a file in a search path.

 .DESCRIPTION

  Search a file in a search path. Return the first file found in the path
  or $null if none found.

 .PARAMETER File

  Name of the file to search.

 .PARAMETER SearchPath

  A list of directories. Can be a string or an array of strings. Each string may
  contain a semi-colon-separated list of directories. By default, use the value
  of the environment variable Path ($env:Path).
#>
function Search-File
{
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true,Position=1)] [String] $File,
        [Parameter(Mandatory=$false,Position=2)] $SearchPath=$env:Path
    )

    # Loop on all provided directories...
    foreach ($dir in $(Split-DeepString $SearchPath ";" -Trim -IgnoreEmpty)) {
        $path = (Join-Path $dir $File)
        if (Test-Path $path) {
            return $path
        }
    }
    # No suitable file found.
    return $null
}

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Define a clean environment Path for Qt applications and tools.

 .DESCRIPTION

  The environment variable Path is completely redefined with Qt binary
  directories and Windows system directories only. Previous directories in
  Path which could interfere with the Qt build process (UnxUtils is known
  for that) are removed.

  The environment variable QtCreator is defined with the path to Qt Creator
  executable, if one is found.

  Note that QtCreator is not in the path. With the Qt / MinGW environment,
  QtCreator is built with MSVC, not MinGW, and brings some DLL's which are
  also built with MSVC. Having them in the path can interfere with the MinGW
  applications. This is especially the case with OpenSSL libraries, starting
  with Qt 5.3.0.

 .PARAMETER QtRoot

  Root directory where Qt installations are dropped (default: search all Qt
  directories at the root of all local drives).

 .PARAMETER Static

  Use the static version of Qt which is under the $QtRoot\Static.
#>
function Set-QtPath
{
    param(
        [String] $QtRoot = "",
        [switch] $Static = $false
    )

    # Hash table of Qt bin directories. Indexed by names of a few standard commands.
    # Initially, the directories for these commands are unknown (empty strings).

    $QtBinMap = @{qmake=""; qtcreator=""; gcc=""; openssl=""}

    # In exploring the Qt installation, skip a few subdirectories which are known
    # to be useless. Since these directories contain hundredths of files, ignoring
    # them speeds up the exploration process. The initial implementation used
    # one single "Get-ChildItem -Recurse" instead of the manual exploration in
    # function Get-QtBinDir. This was much simpler but also 10 times slower.

    $QtSkipDirectories = @("doc", "examples", "imports", "include", "lib", "libexec", "opt", "plugins", "qml", "share", "src", "static")

    # This function explores a tree of directories and updates $QtBinMap.
    # Always sort subdirectories exploration in order to use the latest Qt installation
    # (we assume that the latest is the last one in alphabetical order, eg. 4.8.6, 5.0.2, 5.1.0).

    function Get-QtBinDir ($root, $refQtBinMap)
    {
        # If root directory exists...
        if (Test-Path $root -PathType Container) {

            # Get the list of executable files in root directory.
            $rootexe = @(Get-ChildItem -Path $root -Name *.exe) -replace "`.exe",""

            # Check existence of each reference executable in root directory.
            foreach ($exe in @($refQtBinMap.Value.Keys)) {
                if ($exe -in $rootexe) {
                    $refQtBinMap.Value[$exe] = $root
                }
            }

            # Process each subdirectory in alphabetical order.
            foreach ($subdir in @(Get-ChildItem -Path $root -Directory | Sort-Object)) {
                # Recurse if subdirectory name not in skip list.
                if ($subdir -notin $QtSkipDirectories) {
                    Get-QtBinDir (Join-Path $root $subdir) $refQtBinMap
                }
            }
        }
    }

    # Get the array of root directories to search.
    if ($QtRoot) {
        $QtRootList = @($QtRoot)
    }
    else {
        # Get all local drives, with Qt root directory
        $QtRootList = @((Get-PSDrive -PSProvider FileSystem | Where DisplayRoot -NotMatch "\\\\.*").Root |
            ForEach-Object { Join-Path $_ "Qt" } | Where { Test-Path $_ -PathType Container })
    }

    # Get Qt bin directories.

    foreach ($root in $QtRootList) {
        Get-QtBinDir $root ([REF]$QtBinMap)
    }

    # Get OpenSSL bin directories (assume installation of OpenSSL binaries from
    # http://slproweb.com/products/Win32OpenSSL.html)

    Get-QtBinDir C:\OpenSSL-Win32 ([REF]$QtBinMap)

    # When using static Qt, only keep gcc, then search again in $QtRoot\Static

    if ($Static) {
        $QtBinMap["qmake"] = ""
        Get-QtBinDir $QtRoot\Static ([REF]$QtBinMap)
    }

    # Initial system path, without interfering commands.

    $env:Path = (Join-Path $env:SystemRoot System32) + ";" + $env:SystemRoot

    # Prepend the various Qt binary directories in the system path.
    # The order is significant.

    function Prepend-QtBinDir ($exe)
    {
        $bindir = $QtBinMap[$exe]
        if ($bindir) {
            $env:Path = $bindir + ";" + $env:Path
        }
    }

    Prepend-QtBinDir "openssl"
    Prepend-QtBinDir "gcc"
    Prepend-QtBinDir "qmake"

    # Specific environment variable for Qt Creator.
    $bindir = $QtBinMap["qtcreator"]
    if ($bindir) {
        $env:QtCreator = (Join-Path $QtBinMap["qtcreator"] QtCreator.exe)
    }
}

#-----------------------------------------------------------------------------
#
# PowerShell interface to Win32 function ShowWindowAsync.
#
Add-Type -NameSpace Win32 -Name Win32ShowWindowAsync –PassThru –MemberDefinition @”
    [DllImport("user32.dll")]
    public static extern bool ShowWindowAsync(IntPtr hWnd, int nCmdShow);
“@

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Manipulate the command window of the current PowerShell session.

 .DESCRIPTION

  Manipulate the current PowerShell command window (minimize, maximize,
  restore, hide, etc.) More precisely, this function sends the specified
  command to the command window of the current PowerShell session using
  the Win32 function ShowWindowAsync.

 .PARAMETER Command

  The integer command to send to the window. The supported values are:

  $SW_FORCEMINIMIZE
    Minimizes a window, even if the thread that owns the window is not responding.
    This flag should only be used when minimizing windows from a different thread.

  $SW_HIDE
    Hides the window and activates another window.

  $SW_MAXIMIZE
    Maximizes the specified window.

  $SW_MINIMIZE
    Minimizes the specified window and activates the next top-level window in
    the Z order.

  $SW_RESTORE
    Activates and displays the window. If the window is minimized or maximized,
    the system restores it to its original size and position. An application
    should specify this flag when restoring a minimized window.

  $SW_SHOW
    Activates the window and displays it in its current size and position.

  $SW_SHOWDEFAULT
    Sets the show state based on the SW_ value specified in the STARTUPINFO
    structure passed to the CreateProcess function by the program that started
    the application.

  $SW_SHOWMAXIMIZED
    Activates the window and displays it as a maximized window.

  $SW_SHOWMINIMIZED
    Activates the window and displays it as a minimized window.

  $SW_SHOWMINNOACTIVE
    Displays the window as a minimized window. This value is similar to
    $SW_SHOWMINIMIZED, except the window is not activated.

  $SW_SHOWNA
    Displays the window in its current size and position. This value is similar
    to $SW_SHOW, except that the window is not activated.

  $SW_SHOWNOACTIVATE
    Displays a window in its most recent size and position. This value is similar
    to $SW_SHOWNORMAL, except that the window is not activated.

  $SW_SHOWNORMAL
    Activates and displays a window. If the window is minimized or maximized, the
    system restores it to its original size and position. An application should
    specify this flag when displaying the window for the first time.

 .EXAMPLE

  PS> Show-PowerShellWindow $SW_SHOWMINIMIZED

  Minimize the current PowerShell window.

 .EXAMPLE

  PS> Show-PowerShellWindow $SW_HIDE

  Completely hide the current PowerShell window. Cannot be restored but the current
  PowerShell script continues its execution.

 .NOTES
 
  Based on the following external references:
  - http://msdn.microsoft.com/en-us/library/windows/desktop/ms633549%28v=vs.85%29.aspx
  - http://blogs.msdn.com/b/powershell/archive/2008/06/03/show-powershell-hide-powershell.aspx
#>
function Show-PowerShellWindow
{
    param(
        [Parameter(Mandatory = $true)][Int] $Command
    )
    [void][Win32.Win32ShowWindowAsync]::ShowWindowAsync((Get-Process –id $pid).MainWindowHandle, $Command)
}

#-----------------------------------------------------------------------------
#
# Possible values for Show-PowerShellWindow command parameter:
#
Set-Variable -Option ReadOnly -Name SW_FORCEMINIMIZE   -Value 11
Set-Variable -Option ReadOnly -Name SW_HIDE            -Value  0
Set-Variable -Option ReadOnly -Name SW_MAXIMIZE        -Value  3
Set-Variable -Option ReadOnly -Name SW_MINIMIZE        -Value  6
Set-Variable -Option ReadOnly -Name SW_RESTORE         -Value  9
Set-Variable -Option ReadOnly -Name SW_SHOW            -Value  5
Set-Variable -Option ReadOnly -Name SW_SHOWDEFAULT     -Value 10
Set-Variable -Option ReadOnly -Name SW_SHOWMAXIMIZED   -Value  3
Set-Variable -Option ReadOnly -Name SW_SHOWMINIMIZED   -Value  2
Set-Variable -Option ReadOnly -Name SW_SHOWMINNOACTIVE -Value  7
Set-Variable -Option ReadOnly -Name SW_SHOWNA          -Value  8
Set-Variable -Option ReadOnly -Name SW_SHOWNOACTIVATE  -Value  4
Set-Variable -Option ReadOnly -Name SW_SHOWNORMAL      -Value  1

#-----------------------------------------------------------------------------

<#
 .SYNOPSIS

  Split an arbitrary object in multiple strings.

 .DESCRIPTION

  Split an arbitrary object in multiple strings. If the Data parameter is an
  array, it is recursively flattened (i.e. the elements of all nested arrays
  are grouped into one single array). All elements are converted to strings
  which are split according to the specified separator string. All resulting
  strings are returned into one single array of strings.

 .PARAMETER Data

  The data to split. Can be a string, an array or anything.

 .PARAMETER Separator

  The separator string to use for the split.

 .PARAMETER Trim

  If specified, all returned strings are trimmed.

 .PARAMETER IgnoreEmpty

  If specified, empty strings are omitted in the result array.

 .OUTPUTS

  An array of strings.
#>
function Split-DeepString
{
    [CmdletBinding()]
    param(
        [Parameter(Mandatory=$true,Position=1)] $Data,
        [Parameter(Mandatory=$true,Position=2)] [String] $Separator,
        [switch] $Trim,
        [switch] $IgnoreEmpty
    )

    # Create an array of strings with zero elements.
    $result = New-Object String[] 0

    # Add elements in the array, based on the parameter type.
    if ($Data.GetType().IsArray) {
        # Recursively split the array elements.
        foreach ($element in $Data) {
            $parameters = @{Data = $element; Separator = $Separator; Trim = $Trim; IgnoreEmpty = $IgnoreEmpty}
            $result += Split-DeepString @parameters
        }
    }
    else {
        # Convert parameter into a string and split.
        foreach ($element in $($Data.toString() -split $Separator)) {
            if ($Trim) {
                $element = $element.Trim()
            }
            if (-not $element -eq "" -or -not $IgnoreEmpty) {
                $result += $element
            }
        }
    }
    return $result
}

#-----------------------------------------------------------------------------
#
# Module interface.
#
Export-ModuleMember -Function ConvertFrom-ExcelWorkSheet
Export-ModuleMember -Function ConvertFrom-HtmlString
Export-ModuleMember -Function ConvertTo-HtmlString
Export-ModuleMember -Function ConvertTo-String
Export-ModuleMember -Function Get-DotNetVersion
Export-ModuleMember -Function Get-QtProjectFile
Export-ModuleMember -Function New-TempDirectory
Export-ModuleMember -Function New-ZipFile
Export-ModuleMember -Function Search-File
Export-ModuleMember -Function Set-QtPath
Export-ModuleMember -Function Show-PowerShellWindow
Export-ModuleMember -Function Split-DeepString
Export-ModuleMember -Variable SW_*
