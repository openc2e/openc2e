# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::Cpp::Compiler;
use strict;
use warnings;

use Shake::Check::Program;
use Shake::Check;
use base 'Shake::Check';

our $VERSION = 0.02;

sub initialize {
	my ($self) = @_;
	$self->{checks} = [
		new Shake::Check::Program 'g++',
		new Shake::Check::Program 'c++',
	];
}

sub dummy { shift->new }

sub msg {
	my ($self) = @_;

	return "for a C++ compiler";
}

sub run {
	my $self = shift;
	my @checks = @{ $self->{checks} };

	foreach my $check (@checks) {
		my $val = $check->run(@_);
		if ($val) {
			return $val;
		}
	}
	return undef;
}

1;
