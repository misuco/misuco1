
<?php 

echo "
CREATE TABLE `wp_posts` (
  `ID` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `post_author` bigint(20) unsigned NOT NULL DEFAULT '0',
  `post_date` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `post_date_gmt` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `post_content` longtext NOT NULL,
  `post_title` text NOT NULL,
  `post_excerpt` text NOT NULL,
  `post_status` varchar(20) NOT NULL DEFAULT 'publish',
  `comment_status` varchar(20) NOT NULL DEFAULT 'open',
  `ping_status` varchar(20) NOT NULL DEFAULT 'open',
  `post_password` varchar(20) NOT NULL DEFAULT '',
  `post_name` varchar(200) NOT NULL DEFAULT '',
  `to_ping` text NOT NULL,
  `pinged` text NOT NULL,
  `post_modified` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `post_modified_gmt` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `post_content_filtered` longtext NOT NULL,
  `post_parent` bigint(20) unsigned NOT NULL DEFAULT '0',
  `guid` varchar(255) NOT NULL DEFAULT '',
  `menu_order` int(11) NOT NULL DEFAULT '0',
  `post_type` varchar(20) NOT NULL DEFAULT 'post',
  `post_mime_type` varchar(100) NOT NULL DEFAULT '',
  `comment_count` bigint(20) NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`),
  KEY `post_name` (`post_name`),
  KEY `type_status_date` (`post_type`,`post_status`,`post_date`,`ID`),
  KEY `post_parent` (`post_parent`),
  KEY `post_author` (`post_author`)
) ENGINE=MyISAM AUTO_INCREMENT=0 DEFAULT CHARSET=utf8;";


$n=1;
for($i=0;$i<12;$i++) {
	for($j=0;$j<2048;$j++) {	
		$ntxt=notext($i);
		$ntxturl=notext($i);
		$bitval=1;
        $notecount=1;
		for($k=1;$k<12;$k++) {
			if($j&$bitval) {
				$note=$i+$k;
				$ntxt.=" ".notext($note);
				$ntxturl.="-".notext($note);
                $notecount++;
			}
			$bitval*=2;
		}
//		 echo "INSERT INTO `wp_posts` VALUES ($n, 1, '2014-01-01 00:00:00', '2014-01-01 00:00:00', '$ntxt', '$ntxt', '', 'publish', 'open', 'open', '', '$ntxturl', '', '', '2014-01-01 00:00:00', '2014-01-01 00:00:00', '', 0, 'http://scales.misuco.org/?post_id=$n', 0, 'post', '', 0); \n";

        
        //|       1 | einklang      | einklang      |          0 |
        if($notecount==1) {
            echo "INSERT INTO wp_term_relationships VALUES ($n, 1, 0); \n";
        }
        
        //
        //|       7 | chord         | chord         |          0 |
        //|       4 | interval      | interval      |          0 |
        //
        if($notecount==2) {
            echo "INSERT INTO wp_term_relationships VALUES ($n, 4, 0); \n";
            echo "INSERT INTO wp_term_relationships VALUES ($n, 7, 0); \n";
        }
        //|       5 | triad         | triad         |          0 |
        if($notecount==3) {
            echo "INSERT INTO wp_term_relationships VALUES ($n, 5, 0); \n";
            echo "INSERT INTO wp_term_relationships VALUES ($n, 7, 0); \n";
        }
        //|       6 | seventh chord | seventh-chord |          0 |
        //|      12 | tetratonic    | tetratonic    |          0 |
        if($notecount==4) {
            echo "INSERT INTO wp_term_relationships VALUES ($n, 6, 0); \n";
            echo "INSERT INTO wp_term_relationships VALUES ($n, 7, 0); \n";
            echo "INSERT INTO wp_term_relationships VALUES ($n, 8, 0); \n";
            echo "INSERT INTO wp_term_relationships VALUES ($n, 12, 0); \n";
        }

        //|       8 | scale         | scale         |          0 |
        //|       9 | diatonic      | diatonic      |          0 |
        if($notecount==7) {
            echo "INSERT INTO wp_term_relationships VALUES ($n, 8, 0); \n";
            echo "INSERT INTO wp_term_relationships VALUES ($n, 9, 0); \n";
            /*         10101011010
                     01101010110
                   01011010101
                  10101101010
       V        01101011010
              01011010110
            01010110101         */
            //|      14 | mode          | mode          |          0 |
            //|      15 | ionian        | ionian        |          0 |
            if($j==bindec("10101011010")) {
                echo "INSERT INTO wp_term_relationships VALUES ($n, 14, 0); \n";
                echo "INSERT INTO wp_term_relationships VALUES ($n, 15, 0); \n";
            }
            //|      16 | dorian        | dorian        |          0 |
            if($j==bindec("01101010110")) {
                echo "INSERT INTO wp_term_relationships VALUES ($n, 14, 0); \n";
                echo "INSERT INTO wp_term_relationships VALUES ($n, 16, 0); \n";
            }
            //|      17 | phrygian      | phrygian      |          0 |
            if($j==bindec("01011010101")) {
                echo "INSERT INTO wp_term_relationships VALUES ($n, 14, 0); \n";
                echo "INSERT INTO wp_term_relationships VALUES ($n, 17, 0); \n";
            }
            //|      18 | lydian        | lydian        |          0 |
            if($j==bindec("10101101010")) {
                echo "INSERT INTO wp_term_relationships VALUES ($n, 14, 0); \n";
                echo "INSERT INTO wp_term_relationships VALUES ($n, 18, 0); \n";
            }
            //|      19 | mixolydian    | mixolydian    |          0 |
            if($j==bindec("01101011010")) {
                echo "INSERT INTO wp_term_relationships VALUES ($n, 14, 0); \n";
                echo "INSERT INTO wp_term_relationships VALUES ($n, 19, 0); \n";
            }
            //|      20 | aeolian       | aeolian       |          0 |
            if($j==bindec("01011010110")) {
                echo "INSERT INTO wp_term_relationships VALUES ($n, 14, 0); \n";
                echo "INSERT INTO wp_term_relationships VALUES ($n, 20, 0); \n";
            }
            //|      21 | locrian       | locrian       |          0 |
            if($j==bindec("01010110101")) {
                echo "INSERT INTO wp_term_relationships VALUES ($n, 14, 0); \n";
                echo "INSERT INTO wp_term_relationships VALUES ($n, 20, 0); \n";
            }
            
        }
        //|      10 | hexatonic     | hexatonic     |          0 |
        if($notecount==6) {
            echo "INSERT INTO wp_term_relationships VALUES ($n,  8, 0); \n";
            echo "INSERT INTO wp_term_relationships VALUES ($n, 10, 0); \n";
        }
        //|      11 | pentatonic    | pentatonic    |          0 |
        if($notecount==5) {
            echo "INSERT INTO wp_term_relationships VALUES ($n,  8, 0); \n";
            echo "INSERT INTO wp_term_relationships VALUES ($n, 11, 0); \n";
        }
        
		 /*
		  * test for uniqueness 
		  * 
			$store[$ntxt]=$n;
			
		  */
		$n++;
	}
}

function notext($num) {
	switch($num%12) {
		case 0:
			return "c";
		break;
		case 1:
			return "cs";
		break;
		case 2:
			return "d";
		break;
		case 3:
			return "ds";
		break;
		case 4:
			return "e";
		break;
		case 5:
			return "f";
		break;
		case 6:
			return "fs";
		break;
		case 7:
			return "g";
		break;
		case 8:
			return "gs";
		break;
		case 9:
			return "a";
		break;
		case 10:
			return "as";
		break;
		case 11:
			return "b";
		break;
	}
}
?>
