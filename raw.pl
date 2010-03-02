#!/usr/bin/perl -w

use strict;

use File::Basename;
use POSIX ":sys_wait_h";

my $script = basename($0);
opendir my $dirh, ".";
my @files = grep { !/^\./ && !/$script/ } readdir($dirh);
closedir($dirh);

my @pids;
foreach my $f (@files) {
    while (scalar @pids >= 2) {
        print scalar( @pids)."\n";
        my $status = waitpid($pids[0], 0);
        print "$pids[0] $status\n";
        if ($status != 0) {
            print "reaping child $pids[0]\n";
            shift @pids;
        }
    }
    my $childpid = fork();
    if (!$childpid) {
        my $outname = $f;
        $outname =~ s/\..*$/\.wav/;
        print "afconvert \"$f\" -d LEI16\@44100 -f WAVE -c 2 -r 127 --src-complexity bats \"../samples/$outname\"\n";
        system("afconvert \"$f\" -d LEI16\@44100 -f WAVE -c 2 -r 127 --src-complexity bats -v \"../samples/$outname\"");
        exit;
    } else {
        print "starting child pid $childpid\n";
        push @pids,$childpid;
    }
#    sleep 10;
}