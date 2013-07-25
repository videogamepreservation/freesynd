<?
// EPoll 3.2
// Script for creating unlimited number of polls
//
// Script author: Egorix (http://duet.kiev.ua)
//
// BEGIN
$dat=file("pollcfg.dat");
for($i=0;$i<sizeof($dat);$i++){
$data=explode("|::|",$dat[$i]);
if($data[0]==$_GET['name']){
$pqqq=$data[1];
break;
}
}
$cname=md5("epoll-".$pqqq);
//
if($_GET['coltype']==1 || $_GET['coltype']==0){
$coltype=$_GET['coltype'];
}else{
$coltype=1;
}
if($_GET['w']!="" && is_numeric($_GET['w'])){
$w=$_GET['w'];
}else{
$w=500;
}
include("conf.php");
include("func.php");
check($polldata,$pollcfg,$passfile);
// Anti_hack
$_GET['name']=normal($_GET['name']);
$_GET['w']=normal($_GET['w']);
$_GET['days']=normal($_GET['days']);
$_GET['coltype']=normal($_GET['coltype']);
$_GET['vote']=normal($_GET['vote']);
// *
if($_GET['name']!=""){
if($_GET['vote']!=""){
$name=$_GET['name'];
if(!isset($_COOKIE[$cname])){
if(is_numeric($_GET['vote'])){
$dat=file($polldata);
$fh=fopen($polldata,"w");
for($i=0;$i<sizeof($dat);$i++){
$data=explode("|::|",$dat[$i]);
if($data[0]==$_GET['name']){
$str=$data[0]."|::|";
for($ii=1;$ii<sizeof($data)-2;$ii++){
if($ii==$_GET['vote']){
$data[$ii]=$data[$ii]+1;
}
$str=$str.$data[$ii]."|::|";
}
$str=$str.tdate($datechar)."|::|\n";
$dat[$i]=$str;
$anv=true;
}
fwrite($fh,$dat[$i],strlen($dat[$i]));
}
fclose($fh);
if($anv){
$error="Vote Successful";
if($_GET['days']<=0 || $_GET['days']=="" || !is_numeric($_GET['days'])){
$ctime=5;
}else{
$ctime=60*60*24*$_GET['days'];
}
$cook=@setcookie($cname,tdate("-"),time()+$ctime);
if(!$cook){
$error=$error."<br>Failed to set cookies";
}
}else{
$error="Vote Rejected!";
}
}else{
$error="Error during data sending!";
}
}else{
$error="You cannot vote in this poll";
}
}
?>
<html>
<head>
<title>Poll Results</title>
<style>
body{background-color:#FFFFFF;font-family:verdana;font-size:9pt;}
a:link{text-decoration:none;color:#000066;}
a:hover{text-decoration:none;color:#0000EE;}
.sm{font-family:verdana;font-size:8pt;color:#000033;}
</style>
</head>
<body>
<?
if(isset($error)){
?>
<table align=center width="<?print$w;?>" cellpadding=0 cellspacing=0 border=0 bgcolor="#000000" style="margin-top:5;margin-bottom:5;"><tr><td>
<table width="<?print$w;?>" align=center cellpadding=2 cellspacing=1 border=0><tr bgcolor="#F9EEC1" style="{font-size:8pt;color:#000044;font-family:verdana;}"><td>
<div align="center" style="color:red;"><?print$error;?></div>
</td></tr></table>
</td></tr></table>
<?
}
?>
<table align=center width="<?print$w;?>" cellpadding=0 cellspacing=0 border=0 bgcolor="#000000" style="margin-top:5;margin-bottom:5;"><tr><td>
<table width="<?print$w;?>" align=center cellpadding=2 cellspacing=1 border=0><tr bgcolor="#FDF6D7" style="{font-size:8pt;color:#000044;font-family:verdana;}"><td>
<h5 align="center" style="margin-bottom:5;">Poll Results</h5>
<div align="left" style="font-size:8pt;">
<?
$dat=file($pollcfg);
for($i=0;$i<sizeof($dat);$i++){
$data=explode("|::|",$dat[$i]);
if($data[0]==$_GET['name']){
$find=true;
$n=$i;
break;
}
}
if($find){
$dat1=file($polldata);
$dat2=file($pollcfg);
$data1=explode("|::|",$dat1[$n]);
$num=0;
for($i=1;$i<sizeof($data1)-2;$i++){
$num=$num+1;
$a1[$i-1]=$data1[$i];
}
$all=0;
for($i=0;$i<$num;$i++){
$all=$all+$a1[$i];
}
$last=$data1[$num+1];
$data2=explode("|::|",$dat2[$n]);
for($i=2;$i<sizeof($data2)-1;$i++){
$a2[$i-2]=$data2[$i];
}
$question=$data2[1];
$question=stripslashes($question);
print "<div align=center><b>".$question."</b></div><br>";
if($all!=0){
for($i=0;$i<$num;$i++){
$apr[$i]=round(($a1[$i]/$all)*100)."%";
if(round(($a1[$i]/$all)*100)==0){
$apr[$i]=round(($a1[$i]/$all)*100,2)."%";
}
}
?>
<table width="98%" cellpadding="1" cellspacing="2" border="0" align="center" class="sm">
<?
for($i=0;$i<$num;$i++){
$il=$i+1;
$tw=$apr[$i];
?>
<tr class="sm" bgcolor="#F7EEC7"><td width="3%" rowspan="2" align="center" valign="middle" bgcolor="#ECE1B1"><strong><?print$il;?></strong></td><td width="97%" colspan="2" align="left" valign="middle"><?print$a2[$i];?></td>
<tr class="sm" bgcolor="#F7EEC7"><td width="20%" align="left" valign="middle"><strong><?print$apr[$i];?></strong> ( <?print$a1[$i];?> )</td><td width="77%" align="left" valign="middle">
<?
print "<table align=left bgcolor=#000033 cellpadding=0 cellspacing=0 width=".$tw." height=\"15\"><tr><td><table width=\"100%\" cellpadding=0 cellspacing=1 align=left height=\"15\"><tr><td bgcolor=".color($i,$coltype)."><font style=\"font-size:6pt;color:".color($i,$coltype)."\">&nbsp;</font></td></tr></table></td></tr></table>";
?>
</td></tr>
<?
}
?>
</table>
<?
print "<br><div align=left><b>Total votes:</b> ".$all."</div>";
print "<div align=left><b>Last vote:</b> ".$last."</div><br>";
}else{
print "No one has voted yet!";
print "<div align=left>Poll was created on: ".$last."</div><br>";
}

}else{
print "Poll not found!";
}
?>
</div>
</td></tr></table>
</td></tr></table>
<?
}else{
?>
<html>
<head>
<title>Poll: <?print$_GET['p_q'];?></title>
<style>
body{background-color:#FFFFFF;font-family:verdana;font-size:8pt;}
a:link{text-decoration:none;color:#000066;}
a:hover{text-decoration:none;color:#0000EE;}
</style>
</head>
<body>
<table align=center width="<?print$w;?>" cellpadding=0 cellspacing=0 border=0 bgcolor="#000000" style="margin-top:5;margin-bottom:5;"><tr><td>
<table width="<?print$w;?>" align=center cellpadding=2 cellspacing=1 border=0><tr bgcolor="#F9EEC1" style="{font-size:8pt;color:#000044;font-family:verdana;}"><td>
<h3 align="center" style="margin-top:10;margin-bottom:10;">Poll not found</h3>
</td></tr></table>
</td></tr></table>
<?
}
?>
<table align=center width="<?print$w;?>" cellpadding=0 cellspacing=0 border=0 bgcolor="#000000" style="margin-top:5;margin-bottom:5;"><tr><td>
<table width="<?print$w;?>" align=center cellpadding=1 cellspacing=1 border=0><tr bgcolor="#F9EEC1" style="{font-size:8pt;color:#000044;font-family:verdana;}"><td align="center">
<a href="http://duet.kiev.ua" target="_blank" title="To website of script EPoll 3.2 author (Egorix)">Powered by EPoll 3.2</a>
</td></tr></table>
</td></tr></table>
<br>
<div align="center"><span style="font-size:9pt;font-weight:600;">[ <a href="javascript:window.close()">Close Window</a> ]</span></div>
</body>
</html>
<?
// END
?>
