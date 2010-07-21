<?php $title = 'News'; require_once 'include/common.php';

// The newsItem() signature is newsItem($title, $content, $submitter, $date);

newsItem(
    'New v0.3 release',
	"Freesynd is still alive! A new developpment release is available from <a href=\"https://sourceforge.net/project/showfiles.php?group_id=93282\">sourceforge</a>. This v0.3 release contains mainly bug fixes and 
	minor evolutions but it's still a step toward the v1.0. 
	
	Please try it and report any bug in the project bug tracker.",
    'Benoit Blancard',
    '2010-07-21');
	
newsItem(
    'Now with sound',
"After some <a href=\"http://quantumg.blogspot.com/2007/04/freesynd-version-02-released.html\">battles</a> FreeSynd now has almost all the sounds from the original game functional.  This is cause for release 0.2.  You can <a href=\"https://sourceforge.net/project/showfiles.php?group_id=93282\">download it here.</a>  If you would prefer to get the source from CVS, use the tag: release_0_2.

There's been a number of new people contributing code, and that's great.  If you would like to try your hand at some reverse engineering, implement some features or fix some bugs, we're here to help.",
    'Trent Waddington',
    '2007-04-24');

newsItem(
    'First level now playable',
"I hearby declare that the first level of FreeSynd is now playable.  To celebrate this milestone I have made release 0.1.  You can <a href=\"https://sourceforge.net/project/showfiles.php?group_id=93282\">download it here.</a>  This is an all-in-one package.  It includes: binaries for Linux, binaries for Windows, source code and all the required data files.  If you would prefer to get the source from CVS, use the tag: release_0_1.  

We still need lots of help.  If you find a bug, please report it in the <a href=\"http://sourceforge.net/tracker/?group_id=93282\">bug tracker</a>.  If you can run the original game in <a href=\"http://dosbox.sourceforge.net/\">dosbox</a>, have a go at figuring out what particular bytes in the GAME??.DAT files do (you'll need to <a href=\"http://rtfm.insomnia.org/~qg/dernc.exe\">decompress</a> it first).  If you can program in C++, check out the source code, fix some bugs or implement some features.",
    'Trent Waddington',
    '2006-12-21');

newsItem(
    'Progress',
"Believe it or not, development has started again.  Open Source is a funny
thing, sometimes people get so excited about what they can do that they don't
do anything.  Other times they find they are just overcomitted and can not seem
to find the time to do anything significant.  I like to think that every little
bit counts, so here's my little bit.  In the last couple of days I have
implemented all the menus which one could say make up half the game.  I've also
written a map renderer, so soon we can start on the other half of the game.
However, if I get distracted by work, family, or other projects, I expect
someone else to pick up where I left off.  Let's aim for a 2013 release date,
20 years to the day of the original game's release date!",
    'Trent Waddington',
    '2006-09-06');

include_once('../htdocs-include/testInclude.php');

printFooter(); ?>
