# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Script;
use strict;
use warnings;
use Getopt::Long;
use Exporter;
use base 'Exporter';
use Carp;

use Shake::Util 'module_to_filename';
use Shake::Cache;
use Shake::Config;
use Shake::Config::Cached;

our $VERSION   = 0.01;
our @EXPORT    = qw( %option shake_init shake_done check configure );
our @EXPORT_OK = qw( load_check );
our $Config;
our %Check;
our %option = (
	prefix => '/usr/local',
);


{ no strict 'refs';
	foreach my $f (qw( lookup has define override )) {
		push @EXPORT, $f;
		*$f = sub { $Config->$f(@_) };
	}
}

sub shake_init {
	my ($pkg, $version, $author) = @_;
	my (%define, @undefine);
	$option{define} = \%define;
	$option{undefine} = \@undefine;
	$|++;

	Getopt::Long::Configure('gnu_getopt');
	GetOptions(\%option, 
		qw(
			rerun|R
			define|D=s%
			undefine|undef|U=s@
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
	
	print "configuring $pkg $version (report bugs to $author)\n";
	if ($option{rerun}) {
		print "running in rerun mode...\n";
		$Config->load('.shake/config.pl');
	}
	$Config->define(PACKAGE => $pkg);
	$Config->define(VERSION => $version);
	$Config->define(AUTHOR  => $author);
	$Config->define(PREFIX  => $option{prefix});

	foreach (@undefine) {
		$define{$_} = undef;
	}

	if (%define) {
		print "will override: ", join(', ', keys %define), "\n";
	}
	$Config->define($_ => $define{$_}) foreach keys %define;
}


sub load_check {
	my ($name, $class) = @_;
	if (not defined $class) {
		$class = 'Shake::Check::' . join('::', map(ucfirst($_), split(/\./, $name)));
	}
	my $file = module_to_filename($class);
	eval {
		require $file;
	};
	if ($@) {
		die "Failed to load check module for $name ($class)\nError: $@";
	}
	$Check{$name} = $class;
}


sub check {
	my $name = shift;
	my $class = $Check{$name} or die "Unknown check: $name\n";
	$Config->run( $class->new(@_) );
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
	local $/ = undef;

	my $lookup = sub {
		my $f = shift;
		my $val = eval { $Config->lookup($f) };
		if (not defined $val) {
			return '';
		} else {
			return $val;
		}
	};
	my $eval = sub {
		my $rv = eval shift;
		die $@ if $@;
		return $rv;
	};

	print "configuring $file... ";
	my $content = $in->getline();
	$content =~ s/@([-:.\w]+?)@/$lookup->($1)/ge;
	$content =~ s/@\{(.+?)\}@/$eval->($1)/seg;
	$out->print($content);
	print "done.\n";


	$in->close;
	$out->close;
}

sub ensure {
	my ($required, $optional) = @_;
	my %required = hash_of_list($required ? @$required : $Config->features);
	my %optional = hash_of_list($optional ? @$optional : ());

	my $die = 0;
	foreach my $feature ($Config->features) {
		next if $optional{$feature};
		if (not $Config->has($feature)) {
			if ($required{$feature}) {
				$die++;
				warn "*** Error: required check ``$feature'' failed.\n";
			}
		}	
	}

	die "*** Fatal: Missing requirements.\n" if $die;
}

sub shake_done {
	my %args = @_;
	ensure($args{required}, $args{optional});
	save_config();
	if ($option{dist}) {
		require Shake::Script::Dist;
		Shake::Script::Dist::dist();
	}
}

sub hash_of_list {
	map { ($_, 1) } @_;
}


1;
