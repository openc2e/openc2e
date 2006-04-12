# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Checks;
use strict;
use warnings;
use Exporter;
use base 'Exporter';
use Shake::Script '$Config';
use Shake::Util 'module_to_filename';

our $VERSION = 0.01;

sub import {
	my $this = shift;
	my ($pkg) = caller;
	foreach my $name (@_) {
		my $class = "Shake::Check::$name";
		my $file = module_to_filename($class);
		require $file;
		my $fun = lc $class;
		$fun =~ s/^shake:://;
		$fun =~ s/::/_/g;

		no strict 'refs';
		*{"${pkg}::$fun"} = sub (@) {
			my $check = $class->new(@_);
			$Config->run($check);
		};
	}
}


1;
