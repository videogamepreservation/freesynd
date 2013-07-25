<?
// EPoll 3.2
function normal($string){
$string=strip_tags($string);
$string=str_replace("|::|","",$string);
return $string;
}
function check($datafile,$cfgfile,$passfile){
$txt_scr_fnfound = '<div align=center style="font-family:verdana;font-size:8pt;color:red;">Script was not installed properly, file not found: <b>';
$txt_scr_permfail = '<div align=center style="font-family:verdana;font-size:8pt;color:red;">Wrong permissiona are set for file: <b>';
if(phpversion()<4.3){
exit("<div align=center style=\"font-family:verdana;font-size:8pt;color:red;\">Script requires PHP 4.3 or above. You PHP version is: ".phpversion()."</div><br>");
}
if(!file_exists($datafile)){
exit($txt_scr_fnfound.$datafile."</b></div><br>");
}
if(!file_exists($passfile)){
exit($txt_scr_fnfound.$passfile."</b></div><br>");
}
if(!file_exists($cfgfile)){
exit($txt_scr_fnfound.$cfgfile."</b></div><br>");
}
if(!is_readable($datafile) || !is_writeable($datafile)){
exit($txt_scr_permfail.$datafile."</b></div><br>");
}
if(!is_readable($passfile) || !is_writeable($passfile)){
exit($txt_scr_permfail.$passfile."</b></div><br>");
}
if(!is_readable($cfgfile) || !is_writeable($cfgfile)){
exit($txt_scr_permfail.$cfgfile."</b></div><br>");
}
}
function color($i,$type){
$color[0]="deepskyblue";
$color[1]="orange";
$color[2]="gold";
$color[3]="lightgrey";
$color[4]="forestgreen";
$color[5]="lightblue";
$color[6]="lime";
$color[7]="lightgrey";
$color[8]="violet";
$color[9]="Bisque";
$color[10]="Burlywood";
$color[11]="Brown";
$color[12]="Darkorange";
$color[13]="deeppink";
$color[14]="blue";
$color[15]="Lightblue";
if ($type==1 || $type==0){
if (is_numeric($i)){
if($type==1){
if($i>15){
$color_="#DDCB82";
}else{
$color_=$color[$i];
}
return $color_;
}elseif($type==0){
$color_="#DDCB82";
return $color_;
}
}else{
$color_="#FFFFFF";
return $color_;
}
}else{
$color_="#FFFFFF";
return $color_;
}
}
function login($pass,$passfile){
$dat=file($passfile);
$data=explode("|::|",$dat[1]);
if (md5($pass)==$data[1]){
return true;
}else{
return false;
}
}
function tdate($raz){
$day=date('j');
$month=date('m');
$year=date('Y');
$week=date('w');
$hour=date('H');
$min=date('i');
$sec=date('s');
$string=$day.".".$month.".".$year." ".$raz." ".$hour.":".$min.":".$sec;
return $string;
}
?>