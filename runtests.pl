#!/usr/bin/perl
use strict;
use warnings;
use Test::Harness qw(&runtests $verbose);

my $testpath = $ARGV[0] || "tests";

my @tests = glob("$testpath/*.cos");
my @testfiles = ();

for my $test (@tests) {
		my @variants = ();
		if ($test =~ /\.((c[123av])(,c[123av])*)\.cos$/) {
			for my $v (split /,/, $1) {
				push @variants,$v;
			}
		}
		if (@variants == 0) {
			@variants = ('c3');
		}

		#my $varr = '("'.join("\",\"",@variants).'")';
		for my $v (@variants) {
			my $wrapper = $test;
			$wrapper =~ s/(\.[c123av,]*)?\.cos$/.$v.pl/;
			open FILE, ">$wrapper";
			print FILE <<EOF;
#!/usr/bin/perl
exec("./openc2e", "-n", "-d", "$v"."data", "-g", "$v", "-b", "$test");
EOF
			close FILE;
			chmod 0700, $wrapper;
			push @testfiles,$wrapper;
		}
}

$verbose = 0;
runtests(@testfiles);
map { unlink } @testfiles;
