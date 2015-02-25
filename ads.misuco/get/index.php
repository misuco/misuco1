<?
file_put_contents("./log",date(DATE_ATOM)."\t", FILE_APPEND);
foreach($_SERVER as $key=>$val) {
	file_put_contents("./log",$key."\t".$val."\t", FILE_APPEND);
}
file_put_contents("./log","\n", FILE_APPEND);
$num=100;
$width=640;
$height=480;
if(array_key_exists("w",$_GET)) {
	$width=$_GET["w"];
}
if(array_key_exists("h",$_GET)) {
	$height=$_GET["h"];
}
$filename="../cache/{$num}_{$width}_$height.jpg";
if(!file_exists($filename)) {
	$rsr_org = imagecreatefromjpeg("../ads/$num.jpg");
	$xori=imagesx($rsr_org);
	$yori=imagesy($rsr_org);
	$ar=$xori/$yori;
	$hnew=$width/$ar;
	$ynew=($height-$hnew)/2;
	$rsr_scl = imagecreatetruecolor($width, $height); 
	imagecopyresampled($rsr_scl, $rsr_org, 0, $ynew, 0, 0, $width, $hnew, imagesx($rsr_org), imagesy($rsr_org));
	imagejpeg($rsr_scl, $filename, 95 );
	imagedestroy($rsr_org);
	imagedestroy($rsr_scl);
}
header('Content-type: image/jpeg');
header('Adid: '.$num);
readfile($filename);
?>

