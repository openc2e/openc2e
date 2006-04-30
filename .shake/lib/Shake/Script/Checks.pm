# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Script::Checks;
use strict;
use warnings;
use Exporter;
use base 'Exporter';
use Shake::Script 'load_check';

our $VERSION = 0.01;

sub import {
	my $this = shift;
	load_check($_) foreach @_;
}


1;
