# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Config;
use strict;
use warnings;
use IO::File;
use Carp;
use Shake::Base;
use base 'Shake::Base';

our $VERSION = 0.10;

sub initialize {
	my ($self, $cache) = @_;
	$self->{results} = {};
}

sub save {
	require Data::Dumper;
	my ($self, $file) = @_;
	my $fh = new IO::File($file, 'w') or die "Can't open $file for output: $!";
	$fh->print(
		Data::Dumper::Dumper($self->results)
	);
	$fh->close;
}

sub load {
	my ($self, $file) = @_;
	$self->{results} = do $file;
	if ($@) {
		die $@;
	}
	if (not $self->{results}) {
		die "Failed loading saved config!";
	}
}


sub define {
	my ($self, $key, $value) = @_;
	$self->{results}{$key} = {
		value => $value,
	};
}

sub lookup {
	my ($self, $key, $what) = @_;
	unless (exists $self->{results}{$key}) {
		die "Unknown feature: $key\n";
	}
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

sub run_check {
	my ($self, $check) = @_;
	return (scalar $check->run($self), undef);
}

sub run {
	my ($self, $check) = @_;
	my $name = $check->name;
	print $check->msg, "... ";
	
	my $rv = eval {
		my ($val, $note) = $self->run_check($check);
		$self->{results}{$name} = {
			value   => $val,
			version => $check->version,
			msg     => $check->msg,
		};
		my $s = '';
		if ($note) {
			$s = " ($note)";
		}
		if (defined $val) {
			print "$val$s\n";
			return 1;
		} else {
			print "no$s\n";
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
