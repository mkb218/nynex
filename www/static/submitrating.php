<?php

define(STORE, "ratings.json");

function rate($id,$rating) {
	if (file_exists(STORE)) {
		$f = fopen(STORE, "r");
		if (flock($f, LOCK_EX)) {
			$json = "";
			while ($buf = fread($f, 8192)) {
				$json .= $buf;
			}
			$ratings = json_decode($json);
			fclose($f);
		} else {
			return false;
		}
	} else {
		$ratings = array();
	}
	$ratings[$id] += $rating;
	
	$f = fopen(STORE, "w");
	if (flock($f, LOCK_EX)) {
		$json = json_encode($ratings);
		fwrite($f, $json);
		fclose($f);
	} else {
		return false;
	}
	
	return true;
}

if (array_exists($_GET, "xml")) {
	$xml = (intval($_GET["xml"]) == 1);
} else {
	$xml = true;
}

if (array_exists($_GET, "id") && array_exists($_GET, "rating")) {
	$id = intval($_GET["id"]);
	$rating = intval($_GET["rating"]);
	if ($id == $_GET["id"] && $rating == $_GET["rating"]) {
		if ($xml) {
			xmlResponse(rate($id, $rating));
		} else {
			redirectResponse(rate($id, $rating));
		}
	}
}


?>