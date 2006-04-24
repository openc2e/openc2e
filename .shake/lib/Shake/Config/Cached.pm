# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Config::Cached ;
use strict;
use warnings;
use IO::File;
use Carp;
use Shake::Base;
use base 'Shake::Config';

our $VERSION = 0.10;

sub initialize {
	my ($self, $cache) = @_;
	$self->{cache}   = $cache;
	$self->{results} = {};
}

sub run_check {
	my ($self, $check) = @_;
	my $cache = $self->{cache};
	my ($name, $version) = ($check->name, $check->version);
	my $rv;
	
	if ($check->can_cache) {
		if ($cache->is_cached($name, $version)) {
			my $res = $cache->lookup($name, $version);
			return ($res->{value}, 'cached');
		} else {
			my $val = $check->run($self);
			$cache->add($name, $version, $val);
			return ($val, undef);
		}
	} else {
		return ($check->run($self), undef);
	}
}

1;
