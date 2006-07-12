# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::Endian;
use strict;
use warnings;

use Fatal 'unlink';
use File::Temp 'mkstemps', 'mktemp';
use IO::File;
use Shake::Check;
use base 'Shake::Check';

use Shake::Check::C::Compiler;

use constant PROGRAM => <<'CODE';
#include <stdio.h>
int am_big_endian()
{
	long one = 1;
	return !(*((char *)(&one)));
}

int main()
{
	if (am_big_endian())
		puts("big");
	else
		puts("little");

	return 0;
}
CODE

our $VERSION = 0.03;

sub initialize {
	my ($self, %args) = @_;
	}

sub requires {
	return (
		new Shake::Check::C::Compiler
	);
}

sub dummy {
	shift->new(compiler => 'gcc');
}

sub msg {
	my ($self) = @_;

	return "endianness";
}

sub run {
	my ($self, $engine) = @_;
	my $cc = $engine->lookup('c.compiler');
	
	my ($fh, $srcfile) = mkstemps('shake-XXXXXX', '.c') or die "failed to get temp .c file";
	my $exefile = mktemp('shake-check-XXXXXX') or die "Failed to get temp .exe file";
	$exefile .= '.exe';
	
	print $fh PROGRAM;
	close $fh;
	
	system($cc, '-o', $exefile, $srcfile);
	die "failed to compile C file into executable" unless $? == 0;

	my $rv = `./$exefile`;
	die "failed to run endian test program" unless $? == 0;

	chomp $rv;
	eval {
		unlink($srcfile);
		unlink($exefile);
	};
	warn "failed to unlink temp files...\n" if $@;

	return $rv;
}

1;
