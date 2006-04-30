# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Check::Cpp::Boost::Version;
use strict;
use warnings;

use Fatal 'unlink';
use File::Temp 'mkstemps', 'mktemp';
use IO::File;
use Shake::Check;
use base 'Shake::Check';

use constant PROGRAM => <<'CODE';
#include <cstdio>
#include <boost/version.hpp>

//  BOOST_VERSION % 100 is the sub-minor version
//  BOOST_VERSION / 100 % 1000 is the minor version
//  BOOST_VERSION / 100000 is the major version

int main(int argc, char *argv[])
{
	printf("%d.%d.%d\n", 
		BOOST_VERSION % 100, 
		BOOST_VERSION / 100 % 1000,
		BOOST_VERSION / 100000);
	return (0);
}
CODE

our $VERSION = 0.02;

sub initialize {
	my ($self, $version, %args) = @_;
	$self->{compiler} = $args{compiler} or die "Test must be passed a C++ compiler to use!";
	$self->{version}  = $version
}

sub dummy {
	shift->new('1.0.0', compiler => 'g++');
}

sub msg {
	my ($self) = @_;

	return "checking boost version >= $self->{version}";
}

sub can_cache { 1 }

sub run {
	my ($self, $config) = @_;
	my $cc = $self->{compiler};
	
	my ($fh, $srcfile) = mkstemps('shake-XXXXXX', '.cpp') or die "failed to get temp .cpp file";
	my $exefile = mktemp('shake-check-XXXXXX') or die "Failed to get temp .exe file";
	$exefile .= '.exe';
	
	print $fh PROGRAM;
	close $fh;
	
	system($cc, '-o', $exefile, $srcfile);
	die "failed to compile C++ file into executable" unless $? == 0;

	my $rv = `./$exefile`;
	die "failed to run boost version test program" unless $? == 0;

	chomp $rv;
	eval {
		unlink($srcfile);
		unlink($exefile);
	};
	warn "failed to unlink temp files...\n" if $@;

	if (length($rv) != length($self->{version})) {
		die "version strings must be the same length: $rv vs. $self->{version}";
	}
	
	if ($rv ge $self->{version}) {
		return $rv;
	} else {
		return undef;
	}
}

1;
