<?php print '<?xml version="1.0" encoding="UTF-8"?>
<?xml-stylesheet type="text/css" href="default.css"?>'; ?>

<!DOCTYPE html PUBLIC
    "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">

<?php
// Provide the proper XML MIME type to browsers that can accept it
$contenttype = (stristr($_SERVER['HTTP_ACCEPT'], 'application/xhtml+xml') ?
    'application/xhtml+xml' : 'text/html') . '; charset=UTF-8';
header("Content-type: $contenttype");
header("Content-Script-Type: text/javascript");

$requested_url = (strstr($_SERVER['PHP_SELF'], '.php') ?
    basename($_SERVER['PHP_SELF']) : 'index.php');
	
$fs_path = strstr($_SERVER['PHP_SELF'], $requested_url, true);

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

$title = "News";

?>

  <head>
    <title>The FreeSynd Project :: <?php echo $title; ?></title>
    <meta http-equiv="Content-Type" content="<?php echo $contenttype; ?>" />
    <meta http-equiv="Content-Script-Type" content="text/javascript" />
    <link rel="stylesheet" href="default.css" title="FreeSynd" media="screen, projection, tv" />
	
	<link rel="alternate feed" type="application/rss+xml" title="Freesynd News" href="data/rss-news.xml" />
	<link rel="alternate feed" type="application/rss+xml" title="Freesynd SVN Activity" href="http://sourceforge.net/p/freesynd/code/feed/" />

    <script type="text/javascript" src="clock.js"></script>
  </head>

  <body onload="updateTime(); updateTimezone(); updateDate();">

    <div class="clock" id="time"><div class="clock_bdr"><?php printStartTime(); ?></div></div>
    <div class="clock" id="date"><div class="clock_bdr"><?php printStartDate(); ?></div></div>

    <div class="purp" id="logo"><div class="purp_bdr"><img src="images/freesynd.png" alt="FreeSynd" style="width:189px;height:23px;" /></div></div>

<div id="sidebar"><ul id="linklist">
<li class="teal"><a href="index.php" id="sb_news"><img src="images/bg.png" alt="News" style="width:0;height:0;" /></a></li>
<li class="teal"><a href="about.php" id="sb_about"><img src="images/bg.png" alt="About" style="width:0;height:0;" /></a></li>
<li class="teal"><a href="ss.php" id="sb_ss"><img src="images/bg.png" alt="Screenshots" style="width:0;height:0;" /></a></li>
<li class="teal"><a href="dl.php" id="sb_dl"><img src="images/bg.png" alt="Download" style="width:0;height:0;" /></a></li>
<li class="teal"><a href="dev.php" id="sb_dev"><img src="images/bg.png" alt="Development" style="width:0;height:0;" /></a></li>
<li class="teal"><a href="ff.php" id="sb_ff"><img src="images/bg.png" alt="File Formats" style="width:0;height:0;" /></a></li>
<li class="teal"><a href="links.php" id="sb_links"><img src="images/bg.png" alt="Links" style="width:0;height:0;" /></a></li>
</ul></div>

<div class="purp" id="titlebar"><div class="purp_bdr"><?php echo $title_img ?></div></div>

<div class="purp" id="content"><div class="purp_bdr">
<?php if (detectBrowser() == 'msie'): ?>
<div class="warning">WARNING: You are currently using an obsolete browser
that is unable to render this site correctly. It is suggested that you
upgrade to a newer and more standards-compliant browser. Possible
alternatives are Gecko variants such as
<a href="http://www.mozilla.org/products/firefox/">Firefox</a>,
<a href="http://www.mozilla.org/">Mozilla</a>,
<a href="http://caminobrowser.org/">Camino</a>,
<a href="http://galeon.sourceforge.net/">Galeon</a> and
<a href="https://www.google.com/intl/uk/chrome/browser/">Chrome</a>; KHTML
variants such as <a href="http://www.konqueror.org/">Konqueror</a> and
<a href="http://www.apple.com/safari/">Safari</a>;
<a href="http://www.opera.com/">Opera</a>; or even text-mode browsers such as
<a href="http://lynx.browser.org/">Lynx</a>,
<a href="http://links.sourceforge.net/">Links</a>, and its derivatives such as
<a href="http://elinks.or.cz/">ELinks</a>.</div>
<?php endif; ?>

