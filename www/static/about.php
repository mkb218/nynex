<?php
	$file = "about";
	$titlebar = "about";
	$title = "About the Republic of Nynex.";
	$subtitle = "The Republic of Nynex is an audio art installation that uses genetic programming to create audio compositions using audience-submitted raw audio.";
	$content = <<<EOT
	<p>The Republic of Nynex was created in response to a call for submissions to the 2010 <a href="http://megapolisfestival.org/">Megapolis Festival</a> in Baltimore. The name derives from NYNEX, the company formed by the merger of Baby Bells New England Telephone and New York Telephone. The Republic of Nynex as a fictional state could be thought of as the northern segment of the independent BosWash in some dystopian post-US future. Or, it could be something else.</p>
	<p><a href="http://hydrogenproject.com">Matthew Kane</a>, the creator, is a software engineer, amateur musician, and occasional college radio DJ living in Salem, Massachusetts.</p>
        <hr />
	<p>The Republic of Nynex is written primarily in C++, taking advantage of numerous open source projects and web services.</p>
	<p><a href="http://lancet.mit.edu/ga/">GALib</a> is an MIT-funded project implementing genetic algorithms, allowing developers to take advantage of genetic programming research without starting from scratch.</p>
	<p><a href="http://hydrogenproject.com/pages/fs1rgen.html">FS1rgen</a> is another GAlib-based project used as the starting point for the Republic of Nynex.</p>
	<p><a href="http://openframeworks.cc/">OpenFrameworks</a> is a set of cross-platform libraries used to make the creation audiovisual interfaces extremely easy and straightforward.</p>
	<p><a href="http://code.google.com/p/pygooglevoice/">PyGoogleVoice</a> allows programmatic access to Google Voice.</p>
	<p><a href="http://rapidxml.sourceforge.net">RapidXML</a> is a very lightweight DOM XML parser used by fs1rgen to parse the XML file describing the data layout of the Yamaha FS1r.</p>
	<p><a href="http://www.boost.org">Boost</a> provides free peer-reviewed portable C++ source libraries. They emphasize libraries that work well with the C++ Standard Library. Boost libraries are intended to be widely useful, and usable across a broad spectrum of applications. The Republic of Nynex uses Boost's data structures and JSON parser throughout.</p>
	<p><a href="http://kobesearch.cpan.org/htdocs/Net-SCP/Net/SCP.html">Net::SCP</a> is a Perl module supporting SCP file transfers.</p>
	<p><a href="http://www.openssh.org/">OpenSSH</a> provides the SSH client which is required for the secure file transfers.</p>
	<p><a href="http://csoft.net/">CSoft</a> is our webhost.</p>
	<p><a href="http://sox.sourceforge.net/libsox.html">libsox</a> is a library of sound sample file format readers/writers and sound effects processors. It is mainly developed for use by SoX but is useful for any sound application.</p>
	<p><a href="http://blosxom.com/">Blosxom</a> is a lightweight, extremely customizable blog engine.</p>
	<p><a href="http://soundcloud.com/developers">SoundCloud</a> is a free audio hosting service with an API allowing integration with external applications. Dave Gamble has written a <a href="http://github.com/DaveGamble/soundcloud-c-api-wrapper">C wrapper for this API</a>, using liboauth. The web voting page uses Eric Walhforss's <A href="http://wiki.github.com/ericw/javascript-player/">SoundCloud Javascript player</a>.</p>
	<p><a href="http://twitter.com">Twitter</a>... well, you probably all know what Twitter is. Republic of Nynex uses Twitter to announce new generations.</p>
	<p><a href="http://liboauth.sourceforge.net">liboauth</a> is a software package implementing Twitter's OAuth API, used to connect to both SoundCloud.</a></p>
	<p><a href="http://gnu.ethz.ch/linuks.mine.nu/beteckna/">Beteckna</a> is a GPL-licensed font inspired by Futura.</p>
EOT;
	require("template.inc");
?>
