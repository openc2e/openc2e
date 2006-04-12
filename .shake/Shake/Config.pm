# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Config;
use strict;
use warnings;
use IO::File;
use Carp;
use Shake::Base;
use base 'Shake::Base';

our $VERSION = 0.01;

sub initialize {
	my ($self, $cache) = @_;
	$self->{results} = {};
}


sub define {
	my ($self, $key, $value) = @_;
	$self->{results}{$key} = {
		value => $value,
	};
}

sub lookup {
	my ($self, $key, $what) = @_;
	return $self->{results}{$key}{$what || 'value'};
}

sub results { shift->{results} }

sub features {
	my $self = shift;
	grep exists $self->{results}{$_}{version}, keys %{ $self->{results} };
}

sub has {
	my ($self, $key) = @_;
	defined $self->{results}{$key}{value};
}
sub run {
	my ($self, $check) = @_;
	my $name = $check->fullname;
	print $check->desc, "... ";
	
	my $rv = eval {
		my $val = $check->run($self);
		$self->{results}{$name} = {
			version => $check->version,
			desc   => $check->human_name,
			value   => $val,
		};
		
		if (defined $val) {
			print "$val\n";
			return 1;
		} else {
			print "no\n";
			return 0;
		}
	};

	if ($@) {
		print "failed\n";
		die $@;
	}

	return $rv;
}

1;
