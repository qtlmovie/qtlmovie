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
#  Windows PowerShell script to adjust HTML characters in non-English HTML
#  help files.
#
#  The idea it that these files are edited by hand using HTML syntax, except
#  for non-ASCII characters like letters with accents. Later, this script
#  updates these HTML files and transforms the non-ASCII letters with
#  appropriate HTML syntax.
#
#  The file html-adjust-table.txt is used to list the Unicode characters
#  which should be translated with the corresponding HTML symbols.
# 
#-----------------------------------------------------------------------------

param(
    [switch]$Reverse = $false,
    [switch]$NoPause = $false
)

# PowerShell execution policy.
Set-StrictMode -Version 3
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope Process -Force

# Directories.
$RootDir=(Split-Path -Parent $PSScriptRoot)
$HelpDir=(Join-Path $RootDir help)

# Character table file.
$Script = (Get-Item $PSCommandPath)
$CharTableFile = (Join-Path $Script.DirectoryName "$($Script.BaseName)-table.txt")

# Load the character table from a text file.
# Each line of the file has the format: unicode-value<tab>symbol<tab>....
# Create the hash table $CharTable indexed by unicode values (or reverse).
$CharTable = @{}
Get-Content -Encoding UTF8 $CharTableFile | ForEach-Object {
    $fields = $_.Split("`t")
    if ($fields.Length -ge 2) {
        $unicode = $fields[0] -as [int]
        if ($unicode -ne $null) {
            if ($Reverse) {
                $CharTable[$fields[1]] = [string][char]$unicode
            }
            else {
                $CharTable[$unicode] = $fields[1]
            }
        }
    }
}

# This function process all text lines from standard input to standard output.
# Transform all characters with unicode values in $CharTable in HTML equivalent.
function ProcessLines()
{
    foreach ($line in $input) {
        $result = ""
        if ($Reverse) {
            # Convert HTML sequences into unicode characters.
            $start = 0
            do {
                # Locate next "&...;" structure.
                $amp = $line.IndexOf("&", $start)
                if ($amp -ge 0) {
                    $semi = $line.IndexOf(";", $amp)
                }
                else {
                    $semi = -1
                }
                # Get corresponding unicode.
                if ($semi -gt 0) {
                    $unicode = $CharTable[$line.Substring($amp + 1, $semi - $amp - 1)]
                }
                else {
                    $unicode = $null
                }
                # Perform the replacement if found
                if ($unicode -ne $null) {
                    # Sequence and unicode replacement found.
                    $result += $line.Substring($start, $amp - $start)
                    $result += $unicode
                    $start = $semi + 1
                }
                elseif ($semi -gt 0) {
                    # Sequence found but no unicode replacement, copy sequence.
                    $result += $line.Substring($start, $semi - $start + 1)
                    $start = $semi + 1
                }
                else {
                    # Sequence not found, copy rest of line.
                    $result += $line.Substring($start)
                    $start = $line.Length
                }
            } while ($start -lt $line.Length)
        }
        else {
            # Convert unicode characters into HTML sequences.
            foreach ($c in $line.ToCharArray()) {
                $unicode = [int]$c
                if ($unicode -lt 128) {
                    $result += $c
                }
                else {
                    $sym = $CharTable[$unicode]
                    if ($sym -eq $null) {
                        $result += $c
                    }
                    else {
                        $result += "&" + $sym + ";"
                    }
                }
            }
        }
        $result
    }
}

# Process all non-English HTML files (with a "_locale" suffix in base name).
Get-ChildItem $HelpDir -Recurse -Include *_*.html | ForEach-Object {
    Write-Output "Updating '$_' ..."
    # Get transformed content.
    $Lines = (Get-Content -Encoding UTF8 $_ | ProcessLines)
    # Rewrite the transformed content in UTF-8 without BOM.
    [System.IO.File]::WriteAllLines($_.FullName, $Lines)
}

if (-not $NoPause) {
    pause
}
