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
logdie "no path" if (!$path || $path =~ /^\s+$/);
my $filename = basename(POSIX::tmpnam());
chdir("/opt/nynex/rawsamples");
system("echo 'cd $path; ls | grep -v ratings.json | xargs tar -c -z -f ~/$filename' | ssh -2 $user\@$server > /dev/null 2>&1") and logdie "remote tar failed $!";
system("echo 'cd $path; ls | grep -v ratings.json | xargs rm -rf' | ssh -2 $user\@$server > /dev/null 2>&1") and logdie "remote clear dir failed $!";
my $scp = Net::SCP->new({host => $server, user => $user, interactive => 0 });
$scp->get("/home/mkb218/$filename") or logdie "scp failed $!";
system("ssh -2 $user\@$server rm $filename") and logdie "remote rmtar failed $!";
system("tar zxf $filename") and logdie "local untar failed $!";

if ( -f "manifest") {
    open (MANIFEST, "manifest") or logdie "manifest open failed $!";
    my $line;
    
    while ($line = <MANIFEST>) {
        chomp $line;
        $line =~ s/\s+$//;
        my ($path, $file, $author, $contact_info) = split(/\|/, $line);;
        $path = basename($path);
        if (!system("afconvert -f AIFF -d BEI16 -c 2 --src-complexity bats --src-quality 127 '$path' '/opt/nynex/samples/$file.aiff' > /dev/null 2>&1")) {
            print "/opt/nynex/samples/$file.aiff\n";
            print LOG "$line\n";
            unlink $file;
        } else {
            print LOG "$line failed audio conversion";
        }
    }
    close MANIFEST;
    unlink("manifest");
}
close LOG;