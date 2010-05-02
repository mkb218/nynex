<?php

define(STORE, "/home/mkb218/www/nynex-trax/ratings.json");
//define(STORE, "/Users/makane/ratings.json");

function rate($id,$rating) {
	if (file_exists(STORE)) {
		$f = fopen(STORE, "r");
		if (flock($f, LOCK_EX)) {
			$json = "";
			while ($buf = fread($f, 8192)) {
				$json .= $buf;
			}
			$ratings = json_decode($json, true);
			if ($ratings["generation"] != $_GET["generation"]) {
				$ratings = array();
			}
			fclose($f);
		} else {
			return false;
		}
	} else {
		$ratings = array();
	}
	
	if (!array_key_exists($id, $ratings)) {
		$ratings[$id] = array();
	} elseif (!is_array($ratings[$id])) {
		$ratings[$id] = array($ratings[$id]);
	}
	
	array_push($ratings[$id], $rating);
	$ratings["generation"] = $_GET["generation"];
	
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

$qratings = array();
foreach ( $_GET["rate"] as $id => $rating ) {
	$id = intval($id);
	if (!array_key_exists($id, $qratings)) {
		$qratings[$id] = array();
	}
	$rating = intval($rating);
	if (rate($id, $rating)) {
		array_push($qratings[$id], $rating);
	}
}

header("Location: rate.php?".http_build_query(array('ratings' => $qratings)));



?>