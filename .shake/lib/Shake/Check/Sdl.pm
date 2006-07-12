# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::Sdl;
use strict;
use warnings;

use Shake::Check;
use base 'Shake::Check';

use Shake::Check::Program;

our $VERSION = 0.04;

sub initialize {
	my ($self) = @_;
	$self->requires(
		new Shake::Check::Program('sdl-config'),
	);
}

sub msg {
	my ($self) = @_;
	return "for sdl";
}

sub can_cache { 0 }

sub run {
	my ($self, $engine) = @_;

	my $prog = $engine->lookup('program.sdl-config');

	my $cflags = `"$prog" --cflags` or die "$prog --cflags failed!";
	my $lflags = `"$prog" --libs`   or die "$prog --libs failed!";
	chomp $cflags;
	chomp $lflags;

	$engine->add_feature(
		new Shake::Feature (
			name    => 'sdl.cflags',
			version => $self->version,
			value   => $cflags,
			source  => 'check',
			can_cache => 0,
		)
	);
	$engine->add_feature(
		new Shake::Feature (
			name    => 'sdl.lflags',
			version => $self->version,
			value   => $lflags,
			source  => 'check',
			can_cache => 0,
		)
	);
	return "yes";
}

1;
