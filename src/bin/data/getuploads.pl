#!/usr/bin/perl -w

use strict;

use Net::SCP;
use File::Temp qw ( :POSIX );
use File::Basename;

open LOG, ">>", "/opt/nynex/logs/getuploads.log";

sub logdie {
    print LOG @_;
    print LOG "\n";
    die;
}

my ($user, $server, $path) = @ARGV;
logdie if !$path;
my $filename = basename(POSIX::tmpnam());
chdir("/opt/nynex/rawsamples");
system("ssh -2 $user\@$server tar -c -z -f $filename -s \"'/.*\\\///'\" $path > /dev/null 2>&1") and logdie "remote tar failed $!";
system("ssh -2 $user\@$server rm -rf '$path/*'") and logdie "remote clear dir failed $!";
my $scp = Net::SCP->new({host => $server, user => $user, interactive => 0 });
$scp->get("/home/mkb218/$filename") or logdie "scp failed $!";
system("ssh -2 $user\@$server rm $filename") and logdie "remote rmtar failed $!";
system("tar zxf $filename") and logdie "local untar failed $!";
if ( -f "manifest") {
    my @files = split(/\n/, `cat manifest`);
    foreach my $file (@files) {
        next if ($file =~ /^nynex-trax$/);
        if (!system("afconvert -f AIFF -d BEI16 -c 2 --src-complexity bats --src-quality 127 $file /opt/nynex/samples/$file.aiff > /dev/null 2>&1")) {
            print "/opt/nynex/samples/$file.aiff\n";
            print LOG "/opt/nynex/samples/$file.aiff\n";
            unlink $file;
        }
    }
    unlink("manifest");
}