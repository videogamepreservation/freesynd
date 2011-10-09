<?php

function generateRssFile($newsPath, $destPath)
{
	if (!file_exists($newsPath)) {
		return 'News file does not exists';
	}
	
	$xmlFile = simplexml_load_file($newsPath);
	
	// édition du début du fichier XML
	$xml = '<?xml version="1.0" encoding="UTF-8"?><rss version="2.0">';
	$xml .= '<channel>'; 
	$xml .= '<title>Freesynd News</title>';
	$xml .= '<link>http://freesynd.sourceforge.net/</link>';
	$xml .= '<description>Recent news from the Freesynd project.</description>';
	
	$lastBuildDate = date(DATE_RSS, filemtime($newsPath));
	$xml .= '<lastBuildDate>' . $lastBuildDate . '</lastBuildDate>';
	
	foreach ($xmlFile->item as $item) {
		$xml .= '<item>';
		// News title
		$xml .= '<title>' . htmlentities($item['title']) . '</title>';
		// link
		$xml .= '<link>http://freesynd.sourceforge.net/index.php#n' . $item['id'] . '</link>';
		// News content
		$xml .= '<description>' . $item['rssDesc'] . '</description>';

		$dtTime = DateTime::createFromFormat('Y-m-d', $item['date']);
		$xml .= '<pubDate>' . $dtTime->format("D, d M Y") .' 09:00:01 GMT</pubDate>';
		$xml .= '<guid>http://freesynd.sourceforge.net/index.php#n' . $item['id'] . '</guid>';
	$xml .= '</item>';
	}
	
	// édition de la fin du fichier XML
	$xml .= '</channel>';
	$xml .= '</rss>';

	// écriture dans le fichier
	$fp = fopen($destPath, 'w+');
	
	if (!$fp) {
		return "Cannot open news file";
	}
	
	if(fputs($fp, $xml) == false) {
		return "Cannot write to file " . $destPath;
	}
	
	fclose($fp);
	
	return FALSE;
}

?>