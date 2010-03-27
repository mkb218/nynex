<?php $file = @fopen("/home/mkb218/var/nynex/ratings.txt", "r");
	if (FALSE === $file) {
		$file = @fopen("defaultrate.txt", "r");
	}
	$generation = fgets($file);
	$timeleft = fgets($file);
	$timeleft = $timeleft - time();
	$seconds = $timeleft % 60;
	$minutes = intval($timeleft / 60);
	$timeleft = "$minutes minutes and $seconds seconds";
	while (!feof($file)) {
		$line = fgets($file);
		$kv = explode("=", $line, 2);
		$tracks[$kv[0]] = $kv[1];
	}
?>
<?xml version="1.0" encoding="UTF-8"?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
	<head>
		<title>The Republic of Nynex - rating generation <?php echo $generation; ?></title>
		<link rel="stylesheet" type="text/css" href="nynex.css" />
		<link href="player.css" media="all" rel="stylesheet" type="text/css" />
		<script type="text/javascript" src="soundmanager2.js" />
		<script type="text/javascript" src="jquery-1.3.2.min.js" />
		<script type="text/javascript" src="player.js" />
		<script type="text/javascript" src="rate.js" />
		<meta name="description" content="An interactive collaborative experiment in music-making using genetic algorithms, crowdsourced audio, and open source software" />
		<meta name="keywords" content="openFrameworks,GAlib,music,noise,audio art,genetic programming,genetic algorithms,open source,crowdsourcing" />
	</head>
	<body>
		<div id="pagetitle">The Republic of Nynex - rating generation <?php echo $generation; ?></div>
		<div id="pagesubtitle">Please listen to and rate the following compositions. This generation has <?php echo $timeleft; ?> remaining.</div>
<?php foreach ($tracks as $i => $url) { ?>
		<div id="content">Individual <?php echo $i ?>: <a class="soundcloud-player" id="basic" href="<?php echo $url; ?>">Play</a><?php foreach (array(1,2,3,4,5) as $vote) { ?><a href="" name="rate<?php echo $i; ?>-<?php echo $vote; ?>"><img src="rate<?php echo $vote; ?>.png" /></a> <?php } ?><br/></div>
<?php } ?>
		<div id="content">From here, you can <a href="http://github.com/mkb218/nynex/">follow the software's development</a>, <a href="blosxom.cgi">read the progress blog</a>, <a href="http://twitter.com/nynexrepublic">follow the project on Twitter</a>, <a href="live.html">find out where in-person demonstrations will be occurring</a>, <a href="http://soundcloud.com/republic-of-nynex">listen to the project's creations on SoundCloud</a>, <a href="http://mail231.csoft.net/mailman/listinfo/nynex-announce">join the announcement mailing list</a>, <a href="about.html">read more detailed information about the project</a>, <a href="http://www.facebook.com/pages/Republic-of-Nynex/268718961023">become a Facebook fan</a>, or <a href="contrib.html">see information on audio contributors</a>. Soon, you will be able contribute your own audio recordings, listen to previous generations of compositions, and read about the Republic of Nynex's creator.</div>
		<div id="cc"><p>This site looks best if you have the Futura typeface installed.</p><a href="http://www.twitter.com/nynexrepublic"><img src="http://twitter-badges.s3.amazonaws.com/twitter-c.png" alt="Follow nynexrepublic on Twitter"/></a><a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/us/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/3.0/us/88x31.png" /></a><br />Audio output of this project is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/us/">Creative Commons Attribution-Noncommercial-Share Alike 3.0 United States License</a>.<br/>&copy; 2010 <a href="http://hydrogenproject.com">Matthew Kane</a></div>
		<script type="text/javascript">
		var gaJsHost = (("https:" == document.location.protocol) ? "https://ssl." : "http://www.");
		document.write(unescape("%3Cscript src='" + gaJsHost + "google-analytics.com/ga.js' type='text/javascript'%3E%3C/script%3E"));
		</script>
		<script type="text/javascript">
		try {
		var pageTracker = _gat._getTracker("UA-1899245-5");
		pageTracker._trackPageview();
		} catch(err) {}</script>
		</body>
</html>