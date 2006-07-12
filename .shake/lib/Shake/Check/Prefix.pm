# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::Prefix;
use strict;
use warnings;
use Shake::Check;
use File::Spec;
use base 'Shake::Check';

our $VERSION = 0.01;

sub initialize {
	my ($self) = @_;
}

sub msg {
	my ($self) = @_;

	return "install prefix";
}

sub can_cache { 0 }

sub run {
	my ($self) = @_;

	return '/usr/local';
}

1;
