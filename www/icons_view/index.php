<html>
	<body style="background-color: gray">
		<H2>Icons in different sizes</H2>
		<br /><?php
 //Begin Epoll 3.1
 $pollname="icon";
 $p_path="epoll31/";
 $p_submit="Vote";
 $p_align="center";
 $p_width="250";
 $p_font="verdana";
 $p_txtsize="8pt";
 $p_bcolor="#000000";
 $p_bgcolor="#E5E5E5";
 $p_txtcolor="#000000";
 $p_bwidth="1";
 $p_days="";
 $p_coltype="0";
 $p_rwidth="500";
 include($p_path."form.php");
 //End Epoll 3.1
 ?>
		<br />
		<?php
		$sizes = array(16, 24, 32, 48, 64, 96, 128, 256, 512);
		$names = array('./FS_icon-rev3-512x512.svg' => '"FS icon" by Pawel Swoboda',
'./Freesynd_rev7.svg' => '"Syndilogo" - Oscar Natividad',
'./freesyndicon.svg' => '"Freesynd icon" - Nick Parker-Stevens');
		
		foreach ($names as $fname => $authname) {
			echo '<br /> <hr />';
			echo '<h4>'.$authname.'</h4>'."\n";
			foreach ($sizes as $sz) {
				echo '<img src="'.$fname.'" width="'.$sz.'px" height="'.$sz.'px">'.$sz.'x'.$sz.'</img>';
			}
		}
		?>
	</body>
</html>
