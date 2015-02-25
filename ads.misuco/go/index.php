<?
file_put_contents("./log",date(DATE_ATOM)."\t", FILE_APPEND);
foreach($_SERVER as $key=>$val) {
	file_put_contents("./log",$key."\t".$val."\t", FILE_APPEND);
}
file_put_contents("./log","\n", FILE_APPEND);

if(array_key_exists("lt",$_GET)) {
	echo "misuco performance report<br/><br/>";
	echo "you have been directed to this page, since you have clicked on the ad page. Thank you for providing us with your performance statistic. This will help to figure out, on which devices misuco performs the best. To start the app, on the ad screen, click on the skip button in the upper right corner.<br/><br/>";
	echo "We well provide this page in a nicer layout soon. Statistics will soon be published at www.misuco.org. If you have any questions, please contact c(at)misuco.org.<br/><br/>";
	$lt=hexdec($_GET["lt"]);
	echo "so far you've been running misuco for ";
    	$dtF = new DateTime("@0");
    	$dtT = new DateTime("@$lt");
    	echo $dtF->diff($dtT)->format('%a days, %h hours, %i minutes and %s seconds');
	echo "<br/>";

	$ev=hexdec($_GET["ev"]);

	//echo "during this period misuco has captured $ev touch events<br/>";
	//echo "the touch events had the following delay times. the events with delay >255ms were excluded.<br/>";

	$fpsmax=0;
	$latmax=0;
	$fpsmaxid=0;
	$latmaxid=0;
	$detaillistf="";
	$detaillistl="";

	for($i=0;$i<255;$i++) {
		$key=sprintf("f%02x",$i);
		if(array_key_exists($key,$_GET)) {
			$fps[$i]=hexdec($_GET[$key]);
			if($fps[$i]>$fpsmax) {
				$fpsmax=$fps[$i];
			}
			$fpsmaxid=$i;
			$detaillistf.=sprintf("<br/>%d fps: %d events",$i,$fps[$i]);
		} else {
			$fps[$i]=0;
		}
		$key=sprintf("l%02x",$i);
		if(array_key_exists($key,$_GET)) {
			$lat[$i]=hexdec($_GET[$key]);
			if($lat[$i]>$latmax) {
				$latmax=$lat[$i];
			}
			$latmaxid=$i;
			$detaillistl.=sprintf("<br/>%d ms: %d events",$i,$lat[$i]);
		} else {
			$lat[$i]=0;
		}
	}

	echo "<br/><pre>";
	$scale=25/$latmax;
	$scalefps=25/$fpsmax;
	$latmaxid/=10;
	$fpsmaxid/=10;
	for($i=0;$i<=$fpsmaxid;$i++) {
		$seriemax=0;
		$st=$i*10;
		for($j=0;$j<10;$j++) {
			if($fps[$st+$j]>$seriemax) {
				$seriemax=$fps[$st+$j];
			}
		} 
		printf("%4d - %4d fps: ",$i*10,$i*10+9);
		$vscaled=$seriemax*$scalefps;
		for($j=0;$j<$vscaled;$j++) {
			echo "*";
		}	
		echo "<br/>";
	}
	echo "<br/>";
	for($i=0;$i<=$latmaxid;$i++) {
		$seriemax=0;
		$st=$i*10;
		for($j=0;$j<10;$j++) {
			if($lat[$st+$j]>$seriemax) {
				$seriemax=$lat[$st+$j];
			}
		} 
		printf("%4d - %4d ms: ",$i*10,$i*10+9);
		$vscaled=$seriemax*$scale;
		for($j=0;$j<$vscaled;$j++) {
			echo "*";
		}	
		echo "<br/>";
	}
	echo "</pre>";

	echo "<br/>detailed data:<br/>";
	echo $detaillistf;
	echo "<br/>";
	echo $detaillistl;

	echo "<br/><br/>your system is: <br/>- type:";
	echo $_GET["type"];
	echo "<br/>- ver:";
	echo $_GET["ver"];
	echo "<br/>- arch:";
	echo $_GET["arch"];
	echo "<br/>- browser:";
	echo $_SERVER["HTTP_USER_AGENT"];
	echo "<br/>- ip:";
	echo $_SERVER["REMOTE_ADDR"];
	
} else {
	if($_GET["id"]=="1") {
		header("Location: http://www.misuco.org/etc/ad");
	} else if($_GET["id"]=="3") {
		header("Location: http://www.misuco.org/etc/ad");
	} else {
		header("Location: http://www.misuco.org/etc/ad");
	}
}
?>

