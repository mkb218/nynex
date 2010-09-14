#!/usr/bin/env perl -w

use strict;

use Net::Twitter::Lite;

my ($key, $secret, $tweet) = @ARGV;

my $twitter = Net::Twitter::Lite->new( consumer_key => $key, consumer_secret => $secret, ssl => 1);

get_tokens($twitter);

while (! $twitter->authorized ) {
    print "Authorize this app at ", $twitter->get_authorization_url, " and enter the PIN#\n";
    
    my $pin = <STDIN>;
    chomp $pin;
    
    my($access_token, $access_token_secret, $user_id, $screen_name) =
      $nt->request_access_token(verifier => $pin);
      
    save_tokens($twitter);
}
    
$twitter->update($tweet);