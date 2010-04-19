#!/usr/bin/perl -w

use strict;

use Net::SCP;
use File::Temp qw ( :POSIX );

my ($username, $server, $path) = @ARGV;
my $filename = basename(tmpnam());
system("ssh $server tar zcf $filename $path");
system("ssh $server rm -rf '$path/*'");
$scp = Net::SCP->new($server, $username);
$scp->scp($filename, "/opt/nynex/rawsamples");
system("ssh $server rm -rf $filename");
chdir("/opt/nynex/rawsamples");
my $files = `tar zxvf $filename`;
my @files = split(/\n/, $files);
foreach my $file (@files) {
    system("afconvert -f AIFF -d BEI16 -c 2 --src_complexity bats $file /opt/nynex/samples/$file.aiff");
    print "/opt/nynex/samples/$file.aiff\n";
}