<?php $title = 'Development'; require_once 'include/common.php'; ?>

<h3>V0.5 Roadmap</h3>
<p>Release v0.5 is now under development. There is no release date scheduled for now.</p>
<h4>Bugs :</h4>
<ul>
  <?php
  //NOTE: symbol <'> must be escaped <\'>
  //function roadmapItem($id, $content) - id of bug/feature + short description
  //function roadmapItemNoid($content) - description only
  roadmapItem('3005294','Debriefing information is not displayed after ending the mission');
  roadmapItem('3031032','Rewrite animation generation procedures');
  roadmapItem('3036610','Rewrite map drawing');
  roadmapItem('3005295','The tax rate for the region cannot altered by the player');

  ?>
</ul>

<h4>Features:</h4>
<ul>
  <?php

  roadmapItem('3043686','Game time');
  roadmapItem('3072653','Use libsyndicate data info');

  ?>
</ul>

<?php printFooter(); ?>
