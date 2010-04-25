<?php $file = @fopen("/home/mkb218/var/nynex/ratings.txt", "r");
	if (FALSE === $file) {
		$file = @fopen("defaultrate.txt", "r");
	}
	$generation = fgets($file);
	$tracks = array();
	while (!feof($file)) {
		$line = fgets($file);
		if ($line) {
			array_push($tracks,trim($line));
		}
	}
	
	$file = "rate";
	$titlebar = "rating generation $generation";
	$title = "Generation $generation";
	$subtitle = "Please listen to and rate the following compositions.";
	$i = 0;
	$content .= "<form method=\"GET\" action=\"submitrating.php\" >";
	foreach ($tracks as $id) {
		$content .= "Individual $i: <span name=\"rate$i\">";
		if ($_GET["ratings"][$id]) { 
			$content .= "Thanks for rating!";
		} else { 
			foreach (array(1,2,3,4,5) as $vote) { 
				$content .= "<img src=\"rate${vote}.png\" alt=\"Rate ${vote} points for individual ${i}\" height=\"16\"/><input type=\"radio\" name=\"rate[${id}]\" value=\"$vote\" />";
				}
			}
			$content .= "</span><a class=\"soundcloud-player\" id=\"basic\" href=\"http://soundcloud.com/tracks/$id\">Play</a><br/>";
			$content .= "<hr/>";
		++$i;
	}
	$content .= "<input type=\"submit\" value=\"submit\" /></form>";

$headers = <<<EOT
<link href="player.css" media="all" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="soundmanager2.js" />
<script type="text/javascript" src="jquery-1.3.2.min.js" />
<script type="text/javascript" src="player.js" />
<script type="text/javascript" src="rate.js" />
EOT;
	require("template.inc");

?>
