# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::Program::Version;
use strict;
use warnings;

use Shake::Check;
use base 'Shake::Check';

our $VERSION = 0.02;

sub initialize {
	my ($self, $program, $version) = @_;
	$self->{program} = $program;
	$self->{version} = $version;
}
sub dummy { shift->new('dummy', '1.10') }

sub msg {
	my ($self) = @_;

	return "checking for $self->{program} >= $self->{version}";
}

sub can_cache { 1 }

sub shortname {
	my ($self) = @_;
	return $self->{program};
}

sub run {
	my ($self) = @_;
	my $prog = $self->{program};
	my $v = $self->{version};
	my $out = `$prog --version`;
	my ($pn, $pv) = split(/\s+/, $out);

	if ($pv ge $v) {
		return $pv;
	} else {
		return undef;
	}
}

1;
