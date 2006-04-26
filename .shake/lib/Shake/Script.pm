# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Script;
use strict;
use warnings;
use Getopt::Long;
use Exporter;
use base 'Exporter';
use Carp;

use Shake::Cache;
use Shake::Config;
use Shake::Config::Cached;

our $VERSION   = 0.01;
our @EXPORT    = qw( $checking %option shake_init lookup have checks ensure save_config configure );
our @EXPORT_OK = qw( $Config );
our $Config;
our $checking = 1;
our %option = (
	prefix => '/usr/local',
);

sub lookup {
	$Config->lookup(@_);
}

sub have {
	$Config->has(@_);
}

sub shake_init {
	my ($pkg, $version, $author) = @_;

	Getopt::Long::Configure('gnu_getopt');
	GetOptions(\%option, 
		qw(
			define|D=s%
			nocache
			prefix=s
			dist
		)
	) or exit(1);

	unless ($Config) {
		if (exists $option{'nocache'} or Shake::Cache->is_disabled) {
			$Config = new Shake::Config;
		} else {
			$Config = new Shake::Config::Cached (
				new Shake::Cache,
			)
		}
	}
	
	if ($option{dist}) {
		require Shake::Dist;
		Shake::Dist::make_dist();
		exit 0;
	}
	
	print "configuring $pkg $version (report bugs to $author)\n";
	$Config->define(PACKAGE => $pkg);
	$Config->define(VERSION => $version);
	$Config->define(AUTHOR  => $author);
	$Config->define(PREFIX  => $option{prefix});
}

sub save_config {
	my $file = ".shake/config.pl";
	
	unless (-d '.shake') {
		mkdir '.shake' or die "mkdir(.shake): $!";
	}
	
	print "saving config... ";
	$Config->save($file);
	print "done.\n";
}

sub configure {
	my ($file) = @_;
	my $in = new IO::File("$file.in", 'r') or die "Can't open $file.in for input\n";
	my $out = new IO::File($file, 'w')     or die "Can't open $file for output\n";
	my $line;

	print "writing $file... ";
	while (defined ($line = $in->getline)) {
		$line =~ s/@([-:.\w]+?)@/$Config->lookup($1) || ''/ge;
		$out->print($line);
	}
	print "done.\n";

	$in->close;
	$out->close;
}
sub ensure {
	my (%args) = @_;
	my $default = $args{default};
	my %required = hash_of_list(@{ $args{required} });
	my %optional = hash_of_list(@{ $args{optional} });

	if ($default ne 'optional' and $default ne 'required') {
		croak "default must be one of required or optional!";
	}

	my $die = 0;
	foreach my $feature ($Config->features) {
		next if $optional{$feature};
		if (not $Config->has($feature)) {
			if ($default eq 'required' or $required{$feature}) {
				$die++;
				warn "*** Error: required check ``$feature'' failed.\n";
			}
		}	
	}

	die "*** Fatal: Missing requirements.\n" if $die;	
}

sub hash_of_list {
	map { ($_, 1) } @_;
}


1;
