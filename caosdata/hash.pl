#!/usr/bin/perl

my @data = (
{ file => "prehash_cmds_l.txt", endian => "LITTLE", suffix => "cmd" },
{ file => "prehash_cmds_b.txt", endian => "BIG", suffix => "cmd" },
{ file => "prehash_funcs_l.txt", endian => "LITTLE", suffix => "func" },
{ file => "prehash_funcs_b.txt", endian => "BIG", suffix => "func" }
);

open(cppfile, ">caoshashes.cpp");
open(hfile, ">caoshashes.h");
print cppfile "#include \"caoshashes.h\"\n\n";
print hfile "#include \"endianlove.h\"\n\n";

foreach my $dat (@data) {
	system('./hash/perfect -hms < ' . $$dat{file});
	open(phash, "phash.h");
	while(<phash>) {
		if (/PHASHRANGE/) {
			$_ =~ s/^#define PHASHRANGE (.*)$/$1/;
			print hfile "#ifdef __C2E_LITTLEENDIAN\n" if $$dat{endian} eq "LITTLE";
			print hfile "#ifdef __C2E_BIGENDIAN\n" if $$dat{endian} eq "BIG";
			print hfile "#define " . $$dat{suffix} . "_RANGE " . $_ . "\n";
			print hfile "#endif\n";
		}
	}
	close(phash);
	print cppfile "#ifdef __C2E_LITTLEENDIAN\n" if $$dat{endian} eq "LITTLE";
	print cppfile "#ifdef __C2E_BIGENDIAN\n" if $$dat{endian} eq "BIG";
	open(phash, "phash.c");
	while (<phash>) {
		next if (/^#/);
		next if (/val;$/);
		$_ =~ s/ub1/unsigned char/;
		$_ =~ s/ub2/unsigned short/;
		$_ =~ s/ub4/unsigned int/;
		$_ =~ s/tab/tab_$$dat{suffix}/;
		$_ =~ s/phash/phash_$$dat{suffix}/;
		$_ =~ s/\(val\)/\(unsigned int val\)/;
		print cppfile $_;
	}
	close(phash);
	print cppfile "#endif\n";
}

close(cppfile);
close(hfile);
