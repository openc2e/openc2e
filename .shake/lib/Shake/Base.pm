# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Base;
use strict;
use warnings;
use Carp;
use Exporter;
use base 'Exporter';

our @EXPORT = 'fields';
our $VERSION = 0.02;

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

sub fields (@) {
	no strict 'refs';
    my $package = caller;
    foreach my $field (@_) {
    	croak "$field is already defined in $package" if defined &{"${package}::$field"};
    	*{"${package}::$field"} = sub {
    		my $self = shift;
    		if (@_ == 0) {
    			return $self->{$field};
    		} else {
    			return $self->{$field} = shift;
    		}
    	};
    }
}

1;
