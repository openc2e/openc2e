# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Feature;
use strict;
use warnings;
use Shake::Base;
use Carp;
use base 'Shake::Base';

our $VERSION = 0.04;

fields qw( name version value source can_cache );

sub initialize {
	my ($self, %data) = @_;
	my @keys = qw( name version value source );
	foreach my $key (@keys) {
		if (exists $data{$key}) {
			$self->{$key} = $data{$key};
		} else {
			croak "Shake::Feature->new requires the $key field!";
		}
	}
	$self->{can_cache} = $data{can_cache};
}

sub from_override {
	my ($this, $thing, $value) = @_;
	return undef if not defined $value;
	$this->new(
		name => $thing->name,
		version => $thing->version,
		source  => 'override',
		value   => $value,
	);
}

sub from_cache {
	my ($this, $cache, $name, $ver) = @_;
	my $info = $cache->lookup($name, $ver);
	$this->new(
		name    => $name,
		version => $ver,
		value   => $info->{value},
		source  => 'cache',
	);
}

sub from_check {
	my ($this, $engine, $check) = @_;
	my $value = $check->run($engine);

	return undef if not defined $value;
	return $this->new (
		name      => $check->name,
		version   => $check->version,
		value     => $value,
		source    => 'check',
		can_cache => $check->can_cache,
	);
}


1;
