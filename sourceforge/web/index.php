<?php

// Browser preferred language:
$lang = "";
if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
    $lang = substr($_SERVER['HTTP_ACCEPT_LANGUAGE'], 0, 2);
}

// Filter non-English languages, fallback unsupported languages to English.
$suffix = "";
if ($lang == "fr") {
    $suffix = "_" . $lang;
}

// Redirect to localized page.
// Use status 307 ("temporary redirect") to avoid the browser caching the result.
header("location: doc/qtlmovie-intro" . $suffix . ".html", $true, 307);

?>
