soundManager.flashVersion = 9;
soundManager.debugMode = false; // disable debug mode
soundManager.defaultOptions.multiShot = false;
soundManager.url = 'soundcloud-javascript-player'; // change to '.' to use local non-crossdomain soundmanager
$(function() {
  $("a.soundcloud-player#basic").scPlayer();
});
