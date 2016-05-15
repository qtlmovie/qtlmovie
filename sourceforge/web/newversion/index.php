<?php
// PHP script for http://qtlmovie.sourceforge.net/newversion/

//----------------------------------------------------------------------------
// Compare two version strings.
// Return -1, 0, 1 if $v1 is lower than, equal to, greater than $v2.
//----------------------------------------------------------------------------

function compareVersions($v1, $v2)
{
    // Split version strings in integer fields, all other characters are separators.
    $f1 = preg_split("/[^0-9]+/", $v1, -1, PREG_SPLIT_NO_EMPTY);
    $f2 = preg_split("/[^0-9]+/", $v2, -1, PREG_SPLIT_NO_EMPTY);

    // Compare the integer fields in sequence.
    for ($i = 0; $i < count($f1); $i++) {
        if ($i >= count($f2)) {
            // $v1 has more fields than $v2 and are all identical so far.
            return 1;
        }
        $i1 = (int)$f1[$i];
        $i2 = (int)$f2[$i];
        if ($i1 != $i2) {
            // Both versions differ at this point.
            return $i1 < $i2 ? -1 : 1;
        }
    }
    // $v2 has at least as many components as $v1 and are all identical so far.
    return count($f1) < count($f2) ? -1 : 0;
}


//----------------------------------------------------------------------------
// Get the latest version of an href link in a Web page.
// Return an array containing 'version' and 'url' indexes, empty if not found.
// The pattern "#" in $prefix or $suffix means any number.
//----------------------------------------------------------------------------

function getLatestVersionLink($url, $prefix, $suffix)
{
    // Get the HTML content.
    $page = file_get_contents($url);
    if ($page === FALSE) {
        return array();
    }

    // Build the pattern to look for in the HTML text.
    $pattern = '|<a +href="('
             . str_replace('http\\:', 'https*\\:', str_replace('#', '[0-9]+', preg_quote($prefix)))
             . ')([^"]*)('
             . str_replace('#', '[0-9]+', preg_quote($suffix))
             . ')"|';

    // Search all occurences of the patterns. Return $data with:
    // $data[0] = all matches
    // $data[1] = all prefixes
    // $data[2] = all versions
    // $data[3] = all suffixes
    $count = preg_match_all($pattern, $page, $data, PREG_PATTERN_ORDER);
    if ($count === FALSE || $count == 0) {
        return array();
    }

    // Search the index of the latest version.
    $latest = 0;
    for ($i = 1; $i < count($data[2]); $i++) {
        if (compareVersions($data[2][$i], $data[2][$latest]) > 0) {
            $latest = $i;
        }
    }

    // Return a description of the latest version.
    return array('version' => $data[2][$latest], 'url' => $data[1][$latest] . $data[2][$latest] . $data[3][$latest]);
}


//----------------------------------------------------------------------------
// Generate a JSON structure containing the latest version.
//----------------------------------------------------------------------------

function generateOutput($appVersion, $osName, $cpuArch)
{
    // By default, get source files.
    $url = 'http://sourceforge.net/projects/qtlmovie/files';
    $dir = 'src';
    $prefix = 'QtlMovie-';
    $suffix = '-src.zip';
    $option = '/download';

    // Depending on OS name and version, check if we can find a binary version.
    switch ($osName . '|' . $cpuArch) {

    case 'windows|x86_64':
        $dir = 'win64';
        $prefix = 'QtlMovie-Win64-';
        $suffix = '.exe';
        break;

    case 'windows|x86':
    case 'windows|i386':
        $dir = 'win32';
        $prefix = 'QtlMovie-Win32-';
        $suffix = '.exe';
        break;

    case 'osx|x86_64':
    case 'mac|x86_64':
    case 'macosx|x86_64':
        $dir = 'mac';
        $prefix = 'QtlMovie-';
        $suffix = '.dmg';
        break;

    case 'ubuntu|x86_64':
        $dir = 'ubuntu';
        $prefix = 'qtlmovie_';
        $suffix = '_amd64.deb';
        break;

    case 'fedora|x86_64':
        $dir = 'fedora';
        $prefix = 'qtlmovie-';
        $suffix = '-0.fc#.x86_64.rpm';
        break;
    }

    // Now get the lastest version.
    $result = getLatestVersionLink($url . '/' . $dir,
                                   $url . '/' . $dir . '/' . $prefix,
                                   $suffix . $option);

    // Instruct what the application should do.
    if (array_key_exists('version', $result)) {
        switch (compareVersions($appVersion, $result['version'])) {
        case -1:
            $result['status'] = 'new';
            break;
        case 0:
            $result['status'] = 'same';
            break;
        case 1:
            $result['status'] = 'old';
            break;
        }
    }
    
    // Result is always a JSON structure.
    header('Content-type: application/json; charset=utf-8');
    echo json_encode($result);
}


//----------------------------------------------------------------------------
// Main code
//----------------------------------------------------------------------------

// When invoked from QtlMovie, the cookie "target" contains a
// Base64-encoded JSON structure describing the target system.
if (!isset($_COOKIE) ||
    !array_key_exists('target', $_COOKIE) ||
    ($target = base64_decode($_COOKIE['target'])) === FALSE ||
    ($target = json_decode($target, TRUE)) === NULL ||
    !array_key_exists('appVersion', $target) ||
    !array_key_exists('osName', $target) ||
    !array_key_exists('cpuArch', $target)) {
    // The cookie is absent or incorrect, not a valid request, assume generic platform.
    generateOutput("0", "generic", "generic");
}
else {
    // The cookie has been correctly decoded, generate the reply.
    generateOutput($target['appVersion'], $target['osName'], $target['cpuArch']);
}
?>
