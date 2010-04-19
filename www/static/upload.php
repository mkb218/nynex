<?php
	$file = "upload";
	$titlebar = "The Republic of Nynex - contribute";
	$title = "Contribute.";
	$subtitle = "Help make things interesting.";
	$maxfilesize = ini_get("upload_max_filesize");
	$content = <<<EOT
<p>You can add your tracks to the project by uploading an audio file below. Preferred formats are MP3, WAV, and AIFF. Maximum size is $maxfilesize. We will only be able to notify you of a failure manually, and only if you provide contact information.</p>
<p>By submitting this audio, you agree that this work may be remixed and redistributed under the <a href="http://creativecommons.org/licenses/by-nc-sa/3.0/us/">Creative Commons Attribution-Noncommercial-Share Alike 3.0 United States</a> license as part of the Republic of Nynex project.</p>
<form action="upload-handler.php" method="post" enctype="multipart/form-data">
<p>Your name: <input type="text" name="contributor_name" /></p>
<p>Contact information (e-mail, phone): <input type="text" name="contact" /></p>
<p>Track name: <input type="text" name="filename" /></p>
<p>File: <input type="file" name="file" /></p>
<p><input type="submit" value="Upload" /> <input type="reset" /></p>
</form>
EOT;
	require("template.inc");
?>