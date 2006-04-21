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
	my $src = <<"	CODE";
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
	my ($fh, $file) = mkstemps("shakeXXXXX", ".c");
	my $exe = mktemp('checkXXXX');
	print $fh $src;
	close $fh;
	system($cc, '-o', $exe, $file);
	my $rv = `./$exe`;
	chomp $rv;
	unlink($exe);
	unlink($file);

	return $rv;
}

1;
