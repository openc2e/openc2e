# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::CheckOne;
use strict;
use warnings;
use Shake::Check;
use base 'Shake::Check';

our $VERSION = 0.01;

sub fullname { shift->prefix }
sub name { undef }



1;
