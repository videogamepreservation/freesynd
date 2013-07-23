<?
// EPoll 3.1
// Скрипт для создания неограниченого количества опросов посетителей
//
// Автор скрипта: Egorix (http://egorix.net.ru)
//
// Ничего не меняйте в коде скрипта иначе он не будет работать нормально!
// BEGIN
//
$inc="";
if(!file_exists($p_path."func.php") || !file_exists($p_path."conf.php")){
print "<div align=center style=\"font-family:verdana;font-size:8pt;color:red;\">Неверно установлен путь к папке скрипта! Помощь в файле <b>README.html</b></div><br>";
}else{
if(!$inc){
include($p_path."conf.php");
$inc=true;
}
if(isset($pollname)){
$dat=file($p_path.$pollcfg);
for($i=0;$i<sizeof($dat);$i++){
$data=explode("|::|",$dat[$i]);
if($pollname==$data[0]){
$ex=true;
$p_q=$data[1];
for($i=2;$i<sizeof($data)-1;$i++){
$q[$i-2]=$data[$i];
}
break;
}
}
if($ex){
?>
<table align=<?print$p_align;?> width="<?print$p_width;?>" cellpadding=0 cellspacing=0 border=0 bgcolor="<?print$p_bcolor;?>"><tr><td>
<table width="<?print$p_width;?>" align=center cellpadding=2 cellspacing=<?print$p_bwidth;?> border=0><tr bgcolor="<?print$p_bgcolor;?>" style="{font-size:<?print$p_txtsize;?>;color:<?print$p_txtcolor;?>;font-family:<?print$p_font;?>;}"><td>
<form name="<?print$pollname;?>" action="<?print$p_path;?>add.php" method="get" target="_blank" style="{font-size:<?print$p_txtsize;?>;color:<?print$p_txtcolor;?>;font-family:<?print$p_font;?>;margin:0,0,0,0;}">
<div align="center"><b><?print$p_q;?></b></div><br>
<?
$cname=md5("epoll-".$p_q);
for($i=0;$i<sizeof($q);$i++){
$il=$i+1;
if(!isset($_COOKIE[$cname])){
print "<div align=left><input type=radio name=\"vote\" value=\"".$il."\"> ".$q[$i]."</div>";
}else{
print "<div align=left><input type=radio name=\"vote\" disabled> ".$q[$i]."</div>";
}
}
for($i=0;$i<sizeof($q);$i++){
$q[$i]="";
}
$q="";
?>
<input type="hidden" name="name" value="<?print$pollname;?>">
<input type="hidden" name="days" value="<?print$p_days;?>">
<input type="hidden" name="w" value="<?print$p_rwidth;?>">
<input type="hidden" name="coltype" value="<?print$p_coltype;?>">
<div align="center"><input type="submit" <?if(isset($_COOKIE[$cname])){print"disabled";}?> value="<?print$p_submit;?>" style="font-size:8pt;font-family:verdana;font-weight:600;"></div>
<div align="center"><a href="<?print$p_path;?>add.php?name=<?print$pollname;?>&w=<?print$p_rwidth;?>&coltype=<?print$p_coltype;?>" target="_blank" title="View poll results">Results</a>
</form>
</td></tr></table>
</td></tr></table>
<?
}else{
?>
<table align=<?print$p_align;?> width="<?print$p_width;?>" cellpadding=0 cellspacing=0 border=0 bgcolor="<?print$p_bcolor;?>"><tr><td>
<table width="<?print$p_width;?>" align=center cellpadding=2 cellspacing=<?print$p_bwidth;?> border=0><tr bgcolor="<?print$p_bgcolor;?>" style="{font-size:<?print$p_txtsize;?>;color:<?print$p_txtcolor;?>;font-family:<?print$p_font;?>;}"><td>
<div align="center">Опрос <b><?print$pollname;?></b> не найден!</b></div>
</td></tr></table>
</td></tr></table>
<?
}
}else{
?>
<table align=<?print$p_align;?> width="<?print$p_width;?>" cellpadding=0 cellspacing=0 border=0 bgcolor="<?print$p_bcolor;?>"><tr><td>
<table width="<?print$p_width;?>" align=center cellpadding=2 cellspacing=<?print$p_bwidth;?> border=0><tr bgcolor="<?print$p_bgcolor;?>" style="{font-size:<?print$p_txtsize;?>;color:<?print$p_txtcolor;?>;font-family:<?print$p_font;?>;}"><td>
<div align="center">Вы не задали имя опроса</b></div>
</td></tr></table>
</td></tr></table>
<?
}
}
// END
?>