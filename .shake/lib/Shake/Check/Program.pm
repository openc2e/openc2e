# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::Program;
use strict;
use warnings;
use Shake::Check;
use File::Spec;
use base 'Shake::Check';

our $VERSION = 0.03;

sub initialize {
	my ($self, $program) = @_;
	$self->{program} = $program;
}

sub msg {
	my ($self) = @_;

	return "checking for $self->{program}";
}

sub can_cache { 1 }

sub shortname {
	my ($self) = @_;
	return $self->{program};
}

sub run {
	my ($self) = @_;
	my $prog = $self->{program};
	my $sep = ':';
	
	if ($^O eq 'MSWin32') {
		$prog .= ".exe";
		$sep = ';';
	}
	
	my @path = split($sep, $ENV{PATH});
	foreach my $dir (@path) {
		my $path = File::Spec->join($dir, $prog);
		if (-e $path) {
			return $path;
		}
	}
	
	return undef;
}

1;
