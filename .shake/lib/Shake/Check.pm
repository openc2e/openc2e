# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check;
use strict;
use warnings;
use Shake::Base;
use base 'Shake::Base';
use File::Temp ();

our $VERSION = 0.03;

sub tempfile {
	my ($self, @args) = @_;
	my $t = $self->prefix;
	$t =~ s![\\/.]!_!g;
	my $file = new File::Temp (TEMPLATE => "shake_$t-XXXXXX", @args);
	push @{ $self->{files} }, $file;
	return $file;
}

sub tempfilled {
	my ($self, $content, @args) = @_;
	my $file = $self->tempfile(@args);
	print $file $content;
	close $file;
	return $file->filename;
}

sub tempname {
	my ($self, @args) = @_;
	my $file = $self->tempfile(@args);
	close $file;
	unlink $file;
	return "$file";
}

sub tempexe { shift->tempname($^O eq 'MSWin32' ? (SUFFIX => '.exe') : () ) }

sub prefix {
	my ($self) = @_;
	my $prefix = ref $self;
	$prefix =~ s/^Shake::Check:://;
	$prefix =~ s/::/./g;
	
	return lc $prefix
}

sub name {
	my ($self) = @_;
	my $name = $self->shortname;
	return $name ? $self->prefix . "." . $name : $self->prefix;
}

sub shortname { undef }
sub is_fresh  { 1     }
sub can_cache { 1     }
sub version   { 
	shift->VERSION() 
}

sub requires {
	my $self = shift;
	if (@_ == 0) {
		return @{ $self->{requires} || [] };
	} else {
		$self->{requires} = [ @_ ];
	}
}

sub run {
	my ($self) = @_;
	die "implement me";

	#return $val;
}

sub dummy {
	shift->new()
}
1;
