# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::C::Endian;
use strict;
use warnings;

use Fatal 'unlink';
use File::Temp 'mkstemps', 'mktemp';
use IO::File;
use Shake::CheckOne;
use base 'Shake::CheckOne';


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

our $VERSION = 0.01;

sub initialize {
	my ($self, %args) = @_;
	$self->{compiler} = $args{compiler} or die "Test must be passed a C compiler to use!";
}

sub msg {
	my ($self) = @_;

	return "checking endianness";
}

sub can_cache { 1 }

sub run {
	my ($self, $config) = @_;
	my $cc = $self->{compiler};
	
	my ($fh, $srcfile) = mkstemps('shake-XXXXXX', '.c') or die "failed to get temp .c file";
	my $exefile = mktemp('shake-check-XXXXXX') or die "Failed to get temp .exe file";
	$exefile .= '.exe';
	
	print $fh PROGRAM;
	close $fh;
	
	system($cc, '-o', $exefile, $srcfile);
	die "failed to compile C file into executable" unless $? == 0;

	my $rv = `./$exefile`;
	
	if ($? == 0) {
		chomp $rv;
	} else {
		$rv = undef;
	}
	
	unlink($exefile);
	unlink($srcfile);

	return $rv;
}

1;
