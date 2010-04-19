#!/usr/bin/perl -w

use strict;

use Net::SCP;
use File::Temp qw ( :POSIX );

my ($username, $server, $path) = @ARGV;
my $filename = basename(tmpnam());
system("ssh $server tar zcf $path");
system("ssh $server rm -rf '$path/*'");
$scp = Net::SCP->new($server, $username);
