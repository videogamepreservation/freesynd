<?php $title = 'News'; require_once 'include/common.php'; ?>
<?php printHeader(); 

require_once 'include/rss-inc.php';

foreach($_GET as $key=>$val)
{
   if($key == 'rss' && $val == 'gen') {
		$err = generateRssFile("data/news.xml", "data/rss-news.xml");
	
		if ($err) {
			echo 'Did not generate RSS Feed : ' . $err;
		} else {
			echo 'RSS Feed generated.';
		}
	}
}

?>



<h3>GREETINGS, OPERATIVE. WELCOME TO THE FREESYND PROJECT.</h3>

<p>FreeSynd is a cross-platform, <a href="http://www.gnu.org/licenses/gpl.html">
GPL</a>ed reimplementation of the classic Bullfrog game, Syndicate.</p>

<?php printNews() ?>

<?php printFooter(); ?>
