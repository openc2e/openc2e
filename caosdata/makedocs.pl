#!/usr/bin/perl

# todo: handle XYX# as well as XYX:

my @files = <../caosVM_*.cpp>;

my @conditions = ["DOIF", "ELIF", "UNTL"];

my $parsingdoc = 0;
my $doclines;

my %funcs;
my %voids;

sub writedocs {
	my ($name) = @_;

	if ($name =~ /_/) {
		my $one = $name;
		$one =~ s/(.*)_(.*)/$1/;
		my $two = $name;
		$two =~ s/(.*)_(.*)/$2/;
		if (length($one) == 3) {
			$one = $one . ":";
		}
		$name = $one . " " . $two;
	}
	$doclines =~ s/\n/<br>/;
	$doclines = "undocumented!" unless $doclines;

	print docfile "<h2>", $name, "</h2>";
	print docfile "<p>", $doclines, "</p>\n";
	$doclines = "";
}

open(docfile, ">caosdocs.html");

foreach my $fname (@files) {
	open(filehandle, $fname);
	while(<filehandle>) {
		if (/^void caosVM::/) {
			my $type = 0;
			$_ =~ s/^void caosVM:://;
			$_ =~ s/\(\) {$//;
			$_ =~ s/\n//;
			if (/^c_/) {
				$_ =~ s/^c_//;
				$type = 1;
			} elsif (/^v_/) {
				$_ =~ s/^v_//;
				$type = 2;
			} else {
				printf "failed to understand " . $_ . "\n";
				next;
			}
			writedocs($_);
			$_ =~ s/(.*)(_.*)/$1/;
			if (length($_) == 3) {
				$_ = $_ . ":";
			}
			if ($type == 1) {
				$voids{$_} = 1;
			} elsif ($type == 2) {
				$funcs{$_} = 1;
			}
		} elsif (/^\/\*\*$/) {
			$parsingdoc = 1;
		} elsif (/\*\//) {
			$parsingdoc = 0;
		} elsif ($parsingdoc) {
			$doclines = $doclines . $_;
		}
	}
	close(filehandle);
}

close(docfile);

# now, generate prehash files

open(outhandle, ">prehash_cmds.txt");
foreach my $key (keys(%voids)) {
	print outhandle $key, "\n";
}
close(outhandle);
open(outhandle, ">prehash_funcs.txt");
foreach my $key (keys(%funcs)) {
	print outhandle $key, "\n";
}
close(outhandle);
