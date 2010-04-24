<?php $file = @fopen("/home/mkb218/var/nynex/ratings.txt", "r");
	if (FALSE === $file) {
		$file = @fopen("defaultrate.txt", "r");
	}
	$generation = fgets($file);
	while (!feof($file)) {
		$line = fgets($file);
		array_push($tracks,trim($line));
	}
	
	$file = "rate";
	$titlebar = "rating generation $generation";
	$title = "Generation $generation";
	$subtitle = "Please listen to and rate the following compositions.";
	$i = 0;
	foreach ($tracks as $id) {
		$content .= "Individual $i: <span name=\"rate$i\">";
		if ($ratings[$i]) { 
			$content .= "Thanks for rating!";
		} else { 
			foreach (array(1,2,3,4,5) as $vote) { 
				$content .= "<a href=\"\" name=\"rate${i}-${vote}\"><img src=\"rate${vote}.png\" alt=\"Rate ${vote} points for individual ${i}\" height=\"16\"/></a>";
				}
			}
			$content .= "</span><a class=\"soundcloud-player\" id=\"basic\" href=\"http://soundcloud.com/tracks/$id\">Play</a><br/>";
			$content .= "<hr/>";
//		$content .= "</div>";
		++$i;
	}

	require("template.inc");

?>
