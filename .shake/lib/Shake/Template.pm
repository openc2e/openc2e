# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Template;
use strict;
use warnings;

use Shake::Cache::SQL;
use Shake::Base;
use base 'Shake::Base';

our $VERSION  = 0.01;

fields 'file';

sub initialize {
	my ($self, $file) = @_;
	$self->file($file);
}

sub process {
	my ($self, $engine) = @_;
	my $file = $self->file;
	my $in = new IO::File("$file.in", 'r') or die "Can't open $file.in for input\n";
	my $out = new IO::File($file, 'w')     or die "Can't open $file for output\n";
	local $/ = undef;

	my $lookup = sub {
		my $f = shift;
		my $val = eval { $engine->lookup($f) };
		if (not defined $val) {
			return '';
		} else {
			return $val;
		}
	};
	my $eval = sub {
		my $code = shift;
		$code = 'package Shake::Script; sub { my $engine = shift; ' . $code . '}';
		my $fun = eval $code;
		die $@ if $@;

		return $fun->($engine);
	};
	my $content = $in->getline();
	$content =~ s/@([-:.\w]+?)@/$lookup->($1)/ge;
	$content =~ s/@\{(.+?)\}@/$eval->($1)/seg;
	$out->print($content);
	$in->close;
	$out->close;
}

1;
