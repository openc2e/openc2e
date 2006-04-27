# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Base;
use strict;
use warnings;

our $VERSION = 0.01;

sub new {
	my $class = shift;
	my $self = bless {}, $class;
	$self->initialize(@_);

	return $self;
}

sub invoke {
	my ($self, $name, @rest) = @_;
	$self->$name(@rest);
}

sub initialize { }

1;
