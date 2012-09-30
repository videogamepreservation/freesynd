<?php

function printHeader()
{
    require 'include/header-inc.php';
}

function printFooter()
{
    require 'include/footer-inc.php';
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

function getLinkToBug($id, $label) {
	return '<a href="http://sourceforge.net/p/freesynd/bugs/' . $id . '/">'. $label . '</a>';
}

function getLinkToFeature($id, $label) {
	return '<a href="http://sourceforge.net/p/freesynd/feature-requests/' . $id . '/">'. $label . '</a>';
}

function replaceLink($str) {
	$res = $str;
	$start = 0;
	$end = 0;
	
	while (($start = stripos($res,"[[")) !== false) {
		if (($end = stripos($res, "]]", $start)) !== false) {
			$href = trim(substr($res, $start + 2, $end - $start - 2));
			$label = $href;
			
			$pos = strpos($href, ' ');
			if ($pos !== false) {
				$label = trim(substr($href, $pos));
				$href = trim(substr($href, 0, $pos));
			}
			$link = '<a href="' . $href . '">'. $label . '</a>';
			$res = substr($res, 0, $start) . $link . substr($res, $end + 2);
		} else {
			return $str;
		}
	}
	return $res;
}

function replaceHTMLTag($str) {
	$res = $str;
	$start = 0;
	$end = 0;
	
	while (($start = stripos($res,'{{')) !== false) {
		if (($end = stripos($res, '}}', $start)) !== false) {
			$tag = ' <' . trim(substr($res, $start + 2, $end - $start - 2)) . '> ';
			$res = substr($res, 0, $start) . $tag . substr($res, $end + 2);
		} else {
			return $str;
		}
	}
	return $res;
}


function newsItem($title, $content, $submitter, $date, $id)
{
	// News title
	echo '<a name="n' . $id . '">&nbsp;</a>';
    echo '<h3>' . htmlentities($title) . "</h3>\n";
	// News content
	$arr = explode("\n", $content);
	
	$openedPar = false;
	foreach ($arr as &$line) {
		$trimLine = trim($line);
		$trimLine = replaceLink($trimLine);
		$trimLine = replaceHTMLTag($trimLine);
		if (strlen($trimLine) == 0 && $openedPar == true) {
			echo "</p>\n";
			$openedPar = false;
		} else {
			if ($openedPar == false) {
				$openedPar = true;
				echo "<p>";
			}
			echo $trimLine . "\n";
		}
	}
	
	if ($openedPar == true) {
		echo "</p>\n";
	}

	// News submitter and date
    echo '<div class="rght_algn"><b> -- ' . htmlentities($submitter)
        . '</b><br />' . htmlentities($date) . "</div>\n\n";
}

function roadmapItem($id, $content, $status, $isBug)
{
	$statusLabel = '';
	$style = '';
	
	switch( (String) $status) {
    case 'D': // Item completed
		$statusLabel = '(Done)';
        $style = ' style = "text-decoration:line-through"';
        break;
	case 'P': // Item postponed
		$statusLabel = '(Postponed)';
		$style = ' style = "text-decoration:line-through"';
		break;
	case 'C': // item canceled
		$statusLabel = '(Canceled)';
		$style = ' style = "text-decoration:line-through"';
		break;
	case 'T': // item is being tested
		$statusLabel = '(Testing)';
		$style = ' style = "text-decoration:underline"';
		break;
	case 'I': // item is being coded
		$statusLabel = '(Coding)';
		$style = ' style = "text-decoration:underline"';
		break;
  }
	if ($isBug) {
		echo '<li><span' . $style . '>' . getLinkToBug($id, $id) . '&nbsp;' . $content . '</span>&nbsp;' . $statusLabel . "</li>\n";
	} else {
		echo '<li><span' . $style . '>' . getLinkToFeature($id, $id) . '&nbsp;' . $content . '</span>&nbsp;' . $statusLabel . "</li>\n";
	}
}

function printRoadMap() {
	if (file_exists('data/roadmap.xml')) {
		$xml = simplexml_load_file('data/roadmap.xml');
		
		echo "<p><b>" . date("Y-m-d", filemtime('data/roadmap.xml')) . "</b> : " . $xml->status . "</p>\n";
		
		echo "<h4>Features :</h4>\n";
		echo " <ul>\n";
		
		foreach ($xml->features->feature as $feat) {
			roadmapItem($feat['id'], $feat, $feat['status'], false);
		}

		echo " </ul>\n";

		echo "<h4>Bugs :</h4>\n";
		echo " <ul>\n";
		
		foreach ($xml->bugs->bug as $bug) {
			roadmapItem($bug['id'], $bug, $bug['status'], true);
		}

		echo " </ul>\n";
	} else {
		echo "No roadmap defined.\n";
	}
}

function printNews() {
	if (file_exists('data/news.xml')) {
		$xml = simplexml_load_file('data/news.xml');
		
		foreach ($xml->item as $item) {
			newsItem($item['title'], $item, $item['author'], $item['date'], $item['id']);
		}

	} else {
		echo "No news defined.\n";
	}
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

?>
