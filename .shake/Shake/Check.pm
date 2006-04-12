# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check;
use strict;
use warnings;
use Shake::Base;
use base 'Shake::Base';

our $VERSION = 0.01;

sub prefix {
	my ($self) = @_;
	my $prefix = ref $self;
	$prefix =~ s/^Shake::Check:://;
	$prefix =~ s/::/./g;
	
	return lc $prefix
}

sub name    {
	die "implement me";
}

sub fullname {
	my ($self) = @_;
	
	return $self->prefix . "." . $self->name
}

sub human_name {
	shift->fullname;
}

sub desc {
	die "implement me";
}

sub version {
	return shift->VERSION();
}

sub is_fresh {
	my ($self, $cache) = @_;
	
	return undef;
}

sub can_cache { return 1 }

sub run {
	my ($self) = @_;
	die "implement me";
}

1;
__END__
=head1 NAME

Shake::Check - Class representing a single shake t,,est.

=head1 SYNOPSIS

  use Shake::Check;
  # Small code example.

=head1 DESCRIPTION

FIXME

=head1 CONSTRUCTOR

List required parameters for new().

=head1 METHODS

This class implements the following methods:


=head2 name(Z<>)

Returns the base name of the test, e.g. "gcc".

=head2 prefix(Z<>)

Returns the prefix of the test, e.g. "program".

=head2 fullname(Z<>)

Returns C<join('.', $self-E<gt>prefix(), $self-E<gt>name())>

=head2 desc(Z<>)

Returns a string like "checking for foo".
The string MUST NOT end with a newline.

=head2 version(Z<>)

Returns the version of the test. This should be the same between all instances
of a class.

=head2 is_fresh($val)

Returns 0 if the test needs to be run() again and 1 if the test data is still fresh.
$val is the previously-returned value of run().

=head2 can_cache(Z<>)

This returns true if the test can be cached.

=head2 run($config)

Executes the test. A return value of undef means the check failed to find what it wanted.
A true value (a string, usually) will indicate whatever was found.
If a check fails (for example, can't compile example program) an exception is raised.

$config is a L<Shake::Config> object (or compatible, in the case of L<Shake::Cache>.

=head1 BUGS

None known. Bug reports are welcome.

=head1 AUTHOR

Dylan William Hardison, E<lt>dhardison@cpan.orgE<gt>

=head1 SEE ALSO

L<perl(1)>.

=head1 COPYRIGHT and LICENSE

Copyright (C) 2006 by Dylan William Hardison. All Rights Reserved.

This module is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This module is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this module; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

