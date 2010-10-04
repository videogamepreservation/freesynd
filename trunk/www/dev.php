<?php $title = 'Development'; require_once 'include/common.php'; ?>

<h2>Project Communication</h2>
<p>There are different forms of communication used by the FreeSynd team - mailing lists, trackers, forums. 
If you wish to get in contact with the team, join the
community, or volunteer to help out with the project then feel free to use any of those medias and make your voice heard.</p>

<p class="indent"><a href="http://sourceforge.net/projects/freesynd/">Project Page</a> -
FreeSynd's SourceForge Project Page.
<br /><br />

FreeSynd Bugs/Requests Trackers<br />
<small>
(<a href="http://sourceforge.net/tracker/?atid=603796&amp;group_id=93282&amp;func=browse">Bugs</a> |
<a href="http://sourceforge.net/tracker/?atid=603796&amp;group_id=93282&amp;func=browse">Feature Requests</a> |
<a href="http://sourceforge.net/tracker/?atid=603798&amp;group_id=93282&amp;func=browse">Patches</a>)
</small>
<br /><br />

FreeSynd User mailing list<br />
<small>
(<a href="http://lists.sourceforge.net/lists/listinfo/freesynd-user">Subscribe/Manage</a> |
<a href="http://sourceforge.net/mailarchive/forum.php?forum=freesynd-user">Archive</a>)
</small>
<br /><br />

FreeSynd Development mailing list<br />
<small>
(<a href="http://lists.sourceforge.net/lists/listinfo/freesynd-devel">Subscribe/Manage</a> |
<a href="http://sourceforge.net/mailarchive/forum.php?forum=freesynd-devel">Archive</a>)
</small>
<br /><br />

SVN commit notifications<br />
<small>
(<a href="http://lists.sourceforge.net/lists/listinfo/freesynd-cvs">Subscribe/Manage</a> |
<a href="http://sourceforge.net/mailarchive/forum.php?forum=freesynd-cvs">Archive</a>)
</small>
</p>

<hr />

<h2>V0.5 Roadmap</h2>
<p><b>2010-09-30</b> : Release v0.5 is now under development. There is no release date scheduled for now.</p>
<h4>Features:</h4>
<ul>
  <?php

  roadmapItem('3043686','Game time','');
  roadmapItem('3072653','Use libsyndicate data info','');

  ?>
</ul>

<h4>Bugs :</h4>
<ul>
  <?php
  //NOTE: symbol <'> must be escaped <\'>
  //function roadmapItem($id, $content, $done) - id of bug/feature + short description + done('yes' or '')
  //function roadmapItemNoid($content, $) - description + done('yes' or '')
  roadmapItem('3005294','Debriefing information is not displayed after ending the mission','');
  roadmapItem('3031032','Rewrite animation generation procedures','');
  roadmapItem('3036610','Rewrite map drawing','');
  roadmapItem('3005295','The tax rate for the region cannot altered by the player','yes');

  ?>
</ul>

<hr />

<h2>The FreeSynd Team</h2>

<p>(listed in alphabetical order)</p>

<a href="mailto:skbinge@gmail.com">Stuart Bing&euml;</a>
(Developer)<br />
<a href="mailto:xxxx@users.sourceforge.net">Benoit Blancard</a>
(Developer)<br />
<a href="mailto:j-Please.Dont.Spam@7fc1.org">Joost Peters</a>
(Developer)<br />
<a href="mailto:chamel-Please.Dont.Spam@users.sourceforge.net">Bohdan Stelmakh</a>
(Developer)<br />
<a href="mailto:qg@biodome.org">Trent Waddington</a>
(Developer)<br />

<?php printFooter(); ?>
