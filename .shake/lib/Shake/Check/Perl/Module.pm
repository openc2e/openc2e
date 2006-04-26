# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::Perl::Module;
use strict;
use warnings;

use Shake::Check;
use base 'Shake::Check';

our $VERSION = 0.01;

sub initialize {
	my ($self, $module) = @_;
	$self->{module} = $module;
}

sub msg {
	my ($self) = @_;

	return "checking for $self->{module}";
}

sub can_cache { 1 }

sub shortname {
	my ($self) = @_;
	return $self->{module};
}

sub run {
	my ($self) = @_;
	my $mod = $self->{module};
	my $sep = ':';

	eval "require $mod";
	if ($@) {
		return undef;
	} else {
		return "found";
	}
}

1;
