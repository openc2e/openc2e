#!/usr/bin/perl

use strict;
use warnings;

use File::Copy;
use IO::File;

my @hitlist = (glob('*.c'), glob('*.cpp'), glob('*.pl'), glob('*.h'));

for my $f (@hitlist) {
	print "converting $f...\n";
	my $ch = 0;
	unlink "$f.bak";
	copy("$f","$f.bak") or die "unable to back up $f: $!";

	my $in = new IO::File("$f.bak", "r") or die "unable to reopen $f.bak for read: $!";
	my $out = new IO::File("$f", "w") or die "unable to overwrite $f: $!";

	while (<$in>) {
		if (/^(\t*)((?:    )+)(.*)$/s) {
			$ch++;
			$_ = $1 . "\t"x(length($2) / 4) . $3;
		}
		print $out $_ or die "write error generating $f: $!";
	}
	close $out;
	close $in;
	# unlink "$f.bak" unless $ch;
}
# vim: set noet: 
