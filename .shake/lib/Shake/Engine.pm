# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Engine;
use strict;
use warnings;
use IO::File;
use Carp;
use Shake::Base;
use Shake::Cache;
use Shake::Feature;

use base 'Shake::Base';

our $VERSION = 0.10;

sub cache { shift->{cache} }

sub initialize {
	my ($self, %opt) = @_;
	$self->{features} = {};
	$self->{checked}  = {};
	$self->{override} = {};

	$self->{file}     = $opt{file} || '.shake/data.pl';

	if (Shake::Cache->is_enabled and not $opt{nocache}) {
		$self->{cache} = new Shake::Cache;
	} else {
		$self->{cache} = undef;
	}
}

sub checked {
	my ($self, $name) = @_;
	exists $self->{checked}{$name};
}

sub has {
	my ($self, $name) = @_;
	exists $self->{features}{$name};
}

sub fetch {
	my ($self, $name) = @_;
	if (exists $self->{features}{$name}) {
		$self->{features}{$name};
	} else {
		undef;
	}
}

sub lookup {
	my ($self, $name) = @_;
	my $f = $self->fetch($name);
	if ($f) {
		$f->value;
	} else {
		undef;
	}
}

sub set {
	my ($self, $name, $value) = @_;
	my $f = new Shake::Feature (
		name   => $name,
		value  => $value,
		source => 'set',
		version => undef,
		can_cache => 0,
	);
	if ($self->will_override($f->name)) {
		$f = Shake::Feature->from_override($f, $self->{override}{$f->name});
	}

	$self->add_feature($f);
}

sub override {
	my ($self, $name, $value) = @_;
	$self->{override}{$name} = $value;
}

sub will_override {
	my ($self, $name) = @_;
	exists $self->{override}{$name};
}

sub features {
	my $self = shift;
	values %{ $self->{features} };
}

sub check {
	my ($self, $check) = @_;
	my $cache = $self->cache;
	my $name  = $check->name;
	my $ver   = $check->version;

	if ($self->checked($name)) {
		if ($self->has($name)) {
			$self->fetch($name);
		} else {
			undef;
		}
	} else {
		my $f;
		$self->{checked}{$name} = 1;
		if ($self->will_override($name)) {
			$f = Shake::Feature->from_override($check, $self->{override}{$name});
		} elsif ($cache and $check->can_cache and $cache->is_cached($name, $ver)) {
			$f = Shake::Feature->from_cache($cache, $name, $ver);
		} else {
			$f = Shake::Feature->from_check($self, $check);
		}
		return $self->add_feature($f) if defined $f;
		return undef;
	}
}

sub add_feature {
	my ($self, $f) = @_;

	$self->{features}{$f->name} = $f;
}

sub missing {
	my $self = shift;
	
	return grep { not $self->has($_) } keys %{ $self->{checked} };
}

sub save {
	my ($self, $file) = @_;
	my $cache = $self->cache;
	if ($cache) {
		foreach my $feature (values %{ $self->{features} }) {
			next unless $feature->source eq 'check';
			next unless $feature->can_cache;
			if ($cache->is_cached($feature->name, $feature->version)) {
				croak "Already cached feature: ", $feature->name;
			}
			$cache->add($feature->name, $feature->version, $feature->value);
		}
	}

	require Data::Dumper;
	my $fh = new IO::File($file, 'w') or die "Can't open $file for output: $!";
	$fh->print(
		Data::Dumper::Dumper( $self )
	);
	$fh->close;
}

sub load {
	my ($self, $file) = @_;
	%$self = %{ do $file };
	foreach (values %{ $self->{features} }) {
		$_->{source} = 'rerun';
	}
}


1;
