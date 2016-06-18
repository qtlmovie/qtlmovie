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
#  Use the SourceForge download statistics API to collect download statistics
#  for QtlMovie per day and per version. Build a text file with tab-separated
#  fields, suitable for Excel.
#
#  The output file has same directory and name as this script with ".txt".
#
#-----------------------------------------------------------------------------
#
#  Sample usages of SourceForge download statistics API:
#
#  http://sourceforge.net/projects/qtlmovie/files/stats/json?start_date=2013-10-04&end_date=2013-12-31
#  http://sourceforge.net/projects/qtlmovie/files/win64/QtlMovie-Win64-1.2.10.exe/stats/json?start_date=2013-11-20&end_date=2013-12-06
#
#  Returned JSON format (reformatted for readability):
#
#  {
#    "oses": [["Windows", 42], ["Unknown", 3]],
#    "end_date": "2013-12-06 00:00:00",
#    "countries": [["Turkey", 18], ..., ["Hong Kong", 1]],
#    "oses_with_downloads": ["Unknown", "Windows"],
#    "downloads": [["2013-11-20 00:00:00", 0], ["2013-11-21 00:00:00", 0], ..., ["2013-12-06 00:00:00", 1]],
#    "messages": [],
#    "period": "daily",
#    "start_date": "2013-11-20 00:00:00",
#    "oses_by_country": {"Ukraine": {"Windows": 1}, "Turkey": {"Windows": 18}, ..., "Hong Kong": {"Windows": 1}},
#    "total": 45,
#    "stats_updated": "2013-12-06 06:01:14",
#    "summaries": {
#      "os": {"top": "Windows", "percent": 93, "modifier_text": ""},
#      "geo": {"top": "Turkey", "percent": 40, "modifier_text": ""},
#      "time": {"downloads": 45}
#    }
#  }
#
#  Date granularities:
#
#  If the stat and end dates differ by less than one month (31 days), the
#  "downloads" field contains one entry per day. Otherwise, there is only
#  one entry per week (7 days). To collect daily statistics, we need to
#  issue one request per month (or per range of 31 days, regardless of
#  month alignment).
# 
#-----------------------------------------------------------------------------

# PowerShell execution policy.
Set-StrictMode -Version 3

# Build output file name.
$Script = Get-Item $PSCommandPath
$OutFile = Join-Path $Script.DirectoryName "$($Script.BaseName).txt"

# Project files root URL
$Url = "http://sourceforge.net/projects/qtlmovie/files"

# Initial and last statistics date.
$StartDate = [DateTime]::Parse("2013-10-04")
$EndDate = [DateTime]::Today

# Number of days per request.
$ReqDays = 30

# Display debug messages (can be disabled)
$DebugPreference = "Continue"

# Get the content of the src directory (contains a list of source archives).
Write-Debug "Getting $Url/src/"
$WebClient = New-Object System.Net.WebClient
$SrcHtmlPage = $WebClient.DownloadString("$Url/src/")

# Get the list of versions from the list of source archives.
$Versions = @{}
$SrcHtmlPage -split "`n" | 
    Select-String "/src/QtlMovie-\d+\.\d+\.\d+-src.zip/download" |
    ForEach-Object {
        $vs = ($_ -replace ".*/src/QtlMovie-(\d+\.\d+\.\d+)-src.zip/download.*",'$1')
        Write-Debug "Got version $vs"
        $vi = ($vs -split '.',0,"SimpleMatch")
        $Versions[(($vi[0] -as [int]) * 10000) + (($vi[1] -as [int]) * 100) + ($vi[2] -as [int])] = $vs
    }

# Sort the versions in ascending order.
$Versions = ($Versions.GetEnumerator() | Sort-Object Name -Unique | ForEach-Object Value)
Write-Debug "Got $($Versions.Length) versions"

# The hashtable $Stats is indexed by date, in order of dates.
# Each value is a hashtable, indexed by version (ALL means all files), in display order.
$Stats = [ordered] @{}
for ($Date = $StartDate; $Date.CompareTo($EndDate) -le 0; $Date = $Date.AddDays(1)) {
    $D = $Date.ToString("yyyy-MM-dd")
    $Stats[$D] = [ordered] @{}
    $Stats[$D] = [ordered] @{}
    $Stats[$D]["ALL"] = 0
    foreach ($v in $versions) {
        $Stats[$D][$v] = 0
    }
}

# Get the download statistics for a file in a given date range.
function Download-Stats ([String]$FileName, [String]$Version, [DateTime]$StartDate, [DateTime]$EndDate)
{
    # Invoke the SourceForge download stats API
    $D1 = $StartDate.ToString("yyyy-MM-dd")
    $D2 = $EndDate.ToString("yyyy-MM-dd")
    Write-Debug "Getting stat for $D1 to $D2 : $FileName"
    $json = (Invoke-RestMethod "$Url/$FileName/stats/json?start_date=$D1&end_date=$D2")

    # Extract stats for each day.
    foreach ($StatLine in $json.downloads) {
        $Stats[$StatLine[0] -replace ' .*',''][$Version] += $StatLine[1] -as [int]
    }
}

# Loop on dates by chunks of 30 ($ReqDays) days.
for ($Date1 = $StartDate; $Date1.CompareTo($EndDate) -le 0; $Date1 = $Date1.AddDays($ReqDays)) {

    # Compute end of this date chunk
    $Date2 = $Date1.AddDays($ReqDays - 1)
    if ($Date2.CompareTo($EndDate) -gt 0) {
        $Date2 = $EndDate
    }

    # Get total downloads for this day.
    Download-Stats "" "ALL" $Date1 $Date2

    # Get downloads per version.
    foreach ($V in $Versions) {
        Download-Stats "src/QtlMovie-$V-src.zip" $V $Date1 $Date2
        Download-Stats "win32/QtlMovie-Win32-$V.exe" $V $Date1 $Date2
        Download-Stats "win64/QtlMovie-Win64-$V.exe" $V $Date1 $Date2
    }
}

# Format the statistics in tab format.
function Generate-Output
{
    $VersionList = $Versions -join "`t"
    Write-Output "Date`tAll`t$VersionList"
    $Stats.GetEnumerator() | ForEach-Object { 
        $Date = [DateTime]::Parse($_.Name).ToString("dd/MM/yyyy")
        $ValueList = $_.Value.Values -join "`t"
        Write-Output "$Date`t$ValueList"
    }
}
Generate-Output | Out-File $OutFile
pause
