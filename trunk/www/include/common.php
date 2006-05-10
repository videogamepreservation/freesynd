<?php

function printFooter()
{
    require 'include/footer';
}

function detectBrowser()
{
    $user_agent = strtolower($_SERVER['HTTP_USER_AGENT']);

    if (strstr($user_agent, 'opera')) {
        return 'opera';
    } elseif (strstr($user_agent, 'msie')) {
        return 'msie';
    } elseif ((strstr($user_agent, 'konqueror')) || (strstr($user_agent, 'safari'))) {
        return 'khtml';
    } elseif (strstr($user_agent, 'gecko')) {
        return 'mozilla';
    } elseif (strstr($user_agent, 'mozilla/4')) {
        return 'ns4';
    } elseif (strstr($user_agent, 'lynx')) {
        return 'lynx';
    } elseif (strstr($user_agent, 'links')) {
        return 'links';
    } else {
        return false;
    }
}

function img($url, $alt, $wid, $hei, $id = '')
{
    if ($wid != 0) $wid .= 'px'; if ($hei != 0) $hei .= 'px';
    return "<img src=\"images/$url\" alt=\"$alt\" style=\"width:$wid;height:$hei;\" " . (empty($id) ? '' : "id=\"$id\" ") . "/>";
}

function printBlock($text, $block_type = 'p')
{
    echo "<$block_type>" . htmlentities($text) . "</$block_type>";
}

function getFileSize($file)
{
   $size = filesize($file);
   for ($si = 0; $size >= 1024; $size /= 1024, $si++);
   return round($size, 1) . ' ' . trim(substr(' KMGT', $si, 1)) . 'B';
}

function newsItem($title, $content, $submitter, $date)
{
    echo '<h3>' . htmlentities($title) . "</h3>\n";
    echo '<p>' . implode("</p>\n<p>", explode("\n\n", htmlentities($content)))
        . "</p>\n";
    echo '<div class="rght_algn"><b> -- ' . htmlentities($submitter)
        . '</b><br />' . htmlentities($date) . "</div>\n\n";
}

function printStartTime()
{
    $now = getdate();
    $tz = gettimeofday();
    $tz = -$tz['minuteswest'];

    $p = 'a';
    if ($now['hours'] >= 12) {
        $p = 'p';
        $now['hours'] -= 12;
    }
    if ($now['hours'] == 0) {
        $now['hours'] = 12;
    }

    $d = floor($now['hours'] / 10);
    $r = $now['hours'] % 10;
    echo img("clock/$d.png", $d, 8, 7, 'ho1');
    echo img("clock/$r.png", $r, 8, 7, 'ho0');

    echo img('clock/colon.png', ':', 5, 7);

    $d = floor($now['minutes'] / 10);
    $r = $now['minutes'] % 10;
    echo img("clock/$d.png", $d, 8, 7, 'mi1');
    echo img("clock/$r.png", $r, 8, 7, 'mi0');

    echo img('clock/colon.png', ':', 5, 7);

    $d = floor($now['seconds'] / 10);
    $r = $now['seconds'] % 10;
    echo img("clock/$d.png", $d, 8, 7, 'se1');
    echo img("clock/$r.png", $r, 8, 7, 'se0');

    echo img("clock/$p.png", $p, 6, 5, 'per');
    echo img('clock/m.png', 'm', 7, 5);

    if ($tz < 0) {
        echo img('clock/minus.png', '-', 7, 7, 'tzs');
        $tz = -$tz;
    } else {
        echo img('clock/plus.png', '+', 7, 7, 'tzs');
    }

    $t = floor($tz / 60);
    $d = floor($t / 10);
    $r = $t % 10;
    echo img("clock/$d.png", $d, 8, 7, 'tz3');
    echo img("clock/$r.png", $r, 8, 7, 'tz2');

    $t = $tz % 60;
    $d = floor($t / 10);
    $r = $t % 10;
    echo img("clock/$d.png", $d, 8, 7, 'tz1');
    echo img("clock/$r.png", $r, 8, 7, 'tz0');
}

function printStartDate()
{
    $today = getdate();

    $t = floor($today['year'] / 100);
    $d = floor($t / 10);
    $r = $t % 10;
    echo img("clock/$d.png", $d, 8, 7, 'ye3');
    echo img("clock/$r.png", $r, 8, 7, 'ye2');

    $t = $today['year'] % 100;
    $d = floor($t / 10);
    $r = $t % 10;
    echo img("clock/$d.png", $d, 8, 7, 'ye1');
    echo img("clock/$r.png", $r, 8, 7, 'ye0');

    echo img('clock/colon.png', ':', 5, 7);

    $d = floor($today['mon'] / 10);
    $r = $today['mon'] % 10;
    echo img("clock/$d.png", $d, 8, 7, 'mo1');
    echo img("clock/$r.png", $r, 8, 7, 'mo0');

    echo img('clock/colon.png', ':', 5, 7);

    $d = floor($today['mday'] / 10);
    $r = $today['mday'] % 10;
    echo img("clock/$d.png", $d, 8, 7, 'da1');
    echo img("clock/$r.png", $r, 8, 7, 'da0');

    echo img('clock/nc.png', 'nc', 11, 5);
}

// Provide the proper XML MIME type to browsers that can accept it
$contenttype = (stristr($_SERVER['HTTP_ACCEPT'], 'application/xhtml+xml') ?
    'application/xhtml+xml' : 'text/html') . '; charset=UTF-8';
header("Content-type: $contenttype");
header("Content-Script-Type: text/javascript");

$requested_url = (strstr($_SERVER['PHP_SELF'], '.php') ?
    basename($_SERVER['PHP_SELF']) : 'index.php');

switch ($requested_url) {
case 'index.php':
    $title_img = img('titlebar/news.png', 'News', 98, 23);
    break;

case 'about.php':
    $title_img = img('titlebar/about.png', 'About', 117, 23);
    break;

case 'ss.php':
    $title_img = img('titlebar/ss.png', 'Screenshots', 261, 23);
    break;

case 'dl.php':
    $title_img = img('titlebar/dl.png', 'Download', 195, 23);
    break;

case 'dev.php':
    $title_img = img('titlebar/dev.png', 'Development', 267, 23);
    break;

case 'ff.php':
    $title_img = img('titlebar/ff.png', 'File Formats', 263, 23);
    break;

case 'links.php':
    $title_img = img('titlebar/links.png', 'Links', 103, 23);
    break;

default:
    $title_img = '';
}

require 'include/header';

?>
