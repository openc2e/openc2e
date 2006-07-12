# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Frontend;
use strict;
use warnings;
use Shake::Base;
use base 'Shake::Base';

use Carp;
use Fatal 'mkdir';
use IO::File;
use Shake::Engine;
use Shake::Template;
use Data::Dumper;

use constant SHAKE_DIR => '.shake';
use constant SHAKE_LOG => SHAKE_DIR . "/log";
use constant SHAKE_DUMP => SHAKE_DIR . "/dump.pl";

our $VERSION = 0.01;
our %Option = (
	define   => {},
	undefine => [],
);

fields('engine', 'is_script');

sub module_to_filename {
	my $class = shift;
	$class =~ s!::!/!g;
	return "$class.pm";
}

sub initialize {
	my ($self, @r) = @_;
	my %option = (%Option, @r);
	$self->{checks} = {};
	$self->{engine} = new Shake::Engine ( nocache => $option{nocache} );
	$self->{option} = \%option;
	$self->ignore($_) for @{ delete $option{ignore} || [] };
	$self->{checks} = {};
}

sub init {
	my ($self, $pkg, $version, $author) = @_;
	my ($define, $undefine)      = map { delete $self->{option}{$_} } qw( define undefine );
	my $engine = $self->engine;
	my $option = $self->{option};

	mkdir SHAKE_DIR unless -d SHAKE_DIR() or $option->{nosave};

	$self->display('init', "configuring $pkg $version (report bugs to $author)\n");
	if ($self->{option}{rerun}) {
		$self->display('init', "reading in previous state...\n");
		$engine->load(SHAKE_DUMP);
	}

	$engine->set(PACKAGE => $pkg);
	$engine->set(VERSION => $version);
	$engine->set(AUTHOR  => $author);

	foreach (@$undefine) {
		$define->{$_} = undef;
	}

	if (%$define) {
		$self->display('init', "will override: ", join(', ', sort keys %$define), "\n");
	}

	$engine->override(
		$_ => $define->{$_}
	) foreach keys %$define;
}

sub ignore {
	my ($self, $level) = @_;
	$self->{ignore}{$level} = 1;
}

sub display {
	my ($self, $level, @rest) = @_;
	return if exists $self->{ignore}{$level};
	print @rest;
}

sub _check {
	my ($self, $check) = @_;
	my $engine = $self->engine;
	$self->_check($_) foreach grep { not $engine->checked($_->name) } $check->requires;
	$self->display('check', "checking ", $check->msg, "... ");

	eval { 
		my $feature = $engine->check($check);
		
		if (defined $feature) {
			my $s = '';
			my $src = $feature->source;
			if ($src ne 'check') {
				$s = " (from $src)";
			}
			$self->display('check', $feature->value, "$s\n");
		} else {
			$self->display('check', "no\n");
		}
	};
	$self->display('check', "error: $@") if $@;

}

sub check {
	my $self = shift;
	my $name = shift;
	my $class = $self->{checks}{ $name } or croak "Unknown check: $name";
	$self->_check( $class->new(@_) );
}

sub set {
	my ($self, $name, $val) = @_;
	my $engine = $self->engine;
	$self->display('set', "setting $name to ");
	my $f = $engine->set($name => $val);
	my $s = '';
	if ($f->source eq 'override') {
		$s = ' (overridden)';
	}
	$self->display('set', $f->value, "$s\n");
}


sub checking (@) {
	my $self = shift;
	$self->load_check($_) foreach @_;
}

sub load_check  {
    my ($self, $name, $class) = @_;
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
    $self->name_check($name, $class);
}

sub name_check ($$) {
	my ($self, $name, $class) = @_;
	$self->{checks}{$name} = $class;
}

sub configure {
	my ($self, $file) = @_;
	$self->display('configure', "configuring $file... ");
	my $tmpl = new Shake::Template($file);
	$tmpl->process($self->{engine});
	$self->display('configure', "done.\n");
}

sub done {
	my ($self, %args) = @_;
	if ($self->{option}{nosave}) {
		$self->display('done', "Not saving because of --nosave\n");
		return;
	}
	my $engine = $self->engine;
	my $s = "";

	if ($self->is_script) {	
		my @files;
		foreach my $file (keys %INC) {
			next unless $file =~ /^Shake/;
			next if $INC{$file} =~ m!\.shake.lib!;
			my $mod = $file;
			$mod =~ s!/!::!g;
			$mod =~ s/\.pm$//;
			$self->display('warn', "*** Warning: Module `$mod' is not in .shake/lib!\n");
			push @files, [$file, $INC{$file}];
		}
		if (@files) {
			$self->display('note', "*** Note: you should run 'shake dist' before distributing this.\n");
			$self->display('done', "writing external modules list... ");
			my $mods = new IO::File(SHAKE_DIR . "/modules", 'w');
			print $mods Dumper(\@files);
			$mods->close;
			$self->display('done', "done.\n");
		}
	}

	$self->display('done', "saving config$s... ");
	$engine->save(SHAKE_DUMP);
	$self->display('done', "done.\n");
}

1;
