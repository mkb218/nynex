<?php echo '<?xml version="1.0" encoding="UTF-8"?>'; ?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
	<head>
		<title>The Republic of Nynex - <?php echo $titlebar; ?></title>
		<link rel="stylesheet" type="text/css" href="nynex.css" />
		<meta name="description" content="An interactive collaborative experiment in music-making using genetic algorithms, crowdsourced audio, and open source software" />
		<meta name="keywords" content="openFrameworks,GAlib,music,noise,audio art,genetic programming,genetic algorithms,open source,crowdsourcing" />
		<meta name="google-site-verification" content="6tbLZ0GEcTRgF-WS51my-UEuvP8JVdzk8EWEZaKTaT4" />
<?php echo $headers; ?>
	</head>
	<body>
		<div id="pagetitle"><?php echo $title; ?></div>
		<div id="pagesubtitle"><?php echo $subtitle; ?></div>
		<div id="content"><?php echo $content; ?><p>From here, you can <a href="http://github.com/mkb218/nynex/">follow the software's development</a>, <a href="blosxom.cgi">read the progress blog</a>, <?php if ($file !== "live") { ?><a href="live.php">find out where in-person demonstrations will be occurring</a>, <?php } ?><a href="http://soundcloud.com/republic-of-nynex">listen to the project's creations on SoundCloud</a>, <a href="http://mail231.csoft.net/mailman/listinfo/nynex-announce">join the announcement mailing list</a>, <?php if ($file !== "upload") { ?><a href="upload.php">contribute your own audio recordings</a>, <?php } if ($file !== "about") { ?><a href="about.php">read more detailed information about the project</a>, <?php } if ($file !== "contrib") { ?><a href="contrib.php">see information on audio contributors</a>, <?php } if ($file !== "index") { ?><a href="index.php">go back to the index</a>, <?php } ?><a href="http://www.facebook.com/pages/Republic-of-Nynex/268718961023">become a Facebook fan</a>, or <a href="http://twitter.com/nynexrepublic">follow the project on Twitter</a>. Soon, you will be able listen to new compositions and rate them, listen to previous generations of compositions, and read about the Republic of Nynex's creator.</div>
		<div id="cc"><p>This site looks best with CSS3 downloadable fonts enabled, or with the Futura typeface installed.</p><a href="http://www.twitter.com/nynexrepublic"><img src="http://twitter-badges.s3.amazonaws.com/twitter-c.png" alt="Follow nynexrepublic on Twitter"/></a><a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/us/"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/3.0/us/88x31.png" /></a><br />Audio output of this project is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/us/">Creative Commons Attribution-Noncommercial-Share Alike 3.0 United States License</a>. The software itself is licensed under the <a href="http://gnu.org/licenses/gpl-3.0.html">GNU GPL</a>.<br/>&copy; 2010 <a href="http://hydrogenproject.com">Matthew Kane</a></div>
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