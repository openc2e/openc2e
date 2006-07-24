# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::Program::Version;
use strict;
use warnings;

use Shake::Check;
use base 'Shake::Check';

use Shake::Util 'version_ge';
use Shake::Check::Program;
use Carp;

our $VERSION = 0.03;

sub initialize {
	my ($self, $program, $version) = @_;
	$self->{program} = $program or croak "Check requires a program name as first argument";
	$self->{version} = $version or croak "Check requires a version number as the second argument";
	$self->requires(
		new Shake::Check::Program $program,
	);
}
sub dummy { shift->new('dummy', '1.10') }

sub msg {
	my ($self) = @_;

	return "for $self->{program} >= $self->{version}";
}

sub shortname {
	my ($self) = @_;
	return $self->{program};
}

sub run {
	my ($self, $engine) = @_;
	my $prog = $self->{program};
	my $v = $self->{version};
	no warnings;
	my $out = `"$prog" --version`;
	die "Failed to run $prog\n" unless defined $out;
	chomp $out;
	my ($line) = split(/\n/, $out);
	my @a  = split(/\s+/, $line);
	my $pv = $a[-1];

	if (version_ge($pv, $v)) {
		return $pv;
	} else {
		return undef;
	}
}

1;
