# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check;
use strict;
use warnings;
use Shake::Base;
use base 'Shake::Base';

our $VERSION = 0.03;

sub prefix {
	my ($self) = @_;
	my $prefix = ref $self;
	$prefix =~ s/^Shake::Check:://;
	$prefix =~ s/::/./g;
	
	return lc $prefix
}

sub name {
	my ($self) = @_;
	my $name = $self->shortname;
	return $name ? $self->prefix . "." . $name : $self->prefix;
}

sub shortname { undef }
sub version   { shift->VERSION() }
sub is_fresh  { 1 }
sub can_cache { 1 }

sub run {
	my ($self) = @_;
	die "implement me";

	#return $val;
}
sub dummy {
	shift->new()
}
1;
