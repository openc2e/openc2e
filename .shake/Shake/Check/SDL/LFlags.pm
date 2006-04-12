# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::SDL::LFlags;
use strict;
use warnings;

use Shake::CheckOne;
use base 'Shake::CheckOne';

our $VERSION = 0.01;

sub desc {
	my ($self) = @_;

	return "checking sdl-config --libs";
}

sub can_cache { 1 }

sub human_name {
	my $self = shift;
	return "the lflags of sdl";
}

sub run {
	my ($self, $config) = @_;
	die "Test requires sdl-config" unless $config->has('program.sdl-config');
	my $rv = `sdl-config --libs`;
	chomp $rv;

	return $rv;
}

1;
