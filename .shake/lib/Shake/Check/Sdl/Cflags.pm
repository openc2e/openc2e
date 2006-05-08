# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::Sdl::Cflags;
use strict;
use warnings;

use Shake::Check;
use base 'Shake::Check';

our $VERSION = 0.03;

sub initialize {
	my ($self, $sdlconfig) = @_;
	$self->{sdlconfig} = $sdlconfig;
}

sub msg {
	my ($self) = @_;
	return "checking sdl compiler flags";
}

sub run {
	my ($self, $config) = @_;

	my $sdlconfig = $self->{sdlconfig};
	my $rv = `"$sdlconfig" --cflags`;
	chomp $rv;

	return $rv;
}

1;
