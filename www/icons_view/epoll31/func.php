<?
// EPoll 3.1
$txt_scr_fnfound = '<div align=center style="font-family:verdana;font-size:8pt;color:red;">Script was not installed properly, file not found: <b>';
$txt_scr_permfail = '<div align=center style="font-family:verdana;font-size:8pt;color:red;">Wrong permissiona are set for file: <b>';
function check($datafile,$cfgfile,$passfile){
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
$color_="#cccccc";
}else{
$color_=$color[$i];
}
return $color_;
}elseif($type==0){
$color_="#cccccc";
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
$data=explode("|::|",$dat[0]);
if ($pass==$data[0]){
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
if($month=="1"){$m="January";}
elseif($month=="2"){$m="February";}
elseif($month=="3"){$m="March";}
elseif($month=="4"){$m="April";}
elseif($month=="5"){$m="May";}
elseif($month=="6"){$m="June";}
elseif($month=="7"){$m="July";}
elseif($month=="8"){$m="August";}
elseif($month=="9"){$m="September";}
elseif($month=="10"){$m="October";}
elseif($month=="11"){$m="November";}
elseif($month=="12"){$m="December";}
if($week=="0"){$w="Sunday";}
elseif($week=="1"){$w="Monday";}
elseif($week=="2"){$w="Tuesday";}
elseif($week=="3"){$w="Wednesday";}
elseif($week=="4"){$w="Thursday";}
elseif($week=="5"){$w="Friday";}
elseif($week=="6"){$w="Saturday";}
$string=$w." ".$raz." ".$day." ".$m." ".$year." ".$raz." ".$hour.":".$min.":".$sec;
return $string;
}
?>
