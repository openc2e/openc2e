# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::C::Header;
use strict;
use warnings;
use Shake::Check;
use base 'Shake::Check';

use Fatal 'unlink';
use File::Temp 'mkstemps';
use File::Spec;
use Shake::Check::C::Compiler;

our $VERSION = 0.05;

sub initialize {
	my ($self, $header, %args) = @_;
	$self->{header} = $header;
	$self->requires(
		new Shake::Check::C::Compiler,
	);
}

sub msg {
	my ($self) = @_;

	return "for $self->{header}";
}

sub shortname {
	my ($self) = @_;
	my $name = $self->{header};
	$name =~ s/\.h$//;
	$name =~ s/[^.\w]/_/g;
	return $name;
}

sub run {
	my ($self, $engine) = @_;
	my $cc = $engine->lookup('c.compiler');
	my $flags = $engine->lookup('CFLAGS') || '';
	my $devnull = do {
		if ($^O eq 'cygwin') {
			# I smell the stench of Cygwin...
			"nul";
		} else {
			File::Spec->devnull
		}
	};
	my $srcfile = $self->tempfilled(
		"#include <$self->{header}>\n",
		SUFFIX => '.c'
	);
	my $status = system($cc, split(/\s+/, $flags), '-E', '-o', $devnull, $srcfile);
	if ($status != 0) {
		return undef;
	} else {
		return "found";
	}
}

1;
