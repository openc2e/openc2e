#!/usr/bin/perl

use strict;
use warnings;

use YAML;

my @variants = qw(c3 cv);

$SIG{__WARN__} = sub { die $_[0] };

my %fnmap = ( # default category mappings
	'caosVM_agent.cpp' => 'Agents',
	'caosVM_camera.cpp' => 'Cameras',
	'caosVM_core.cpp' => 'Core functions',
	'caosVM_creatures.cpp' => 'Creatures',
	'caosVM_compound.cpp' => 'Compound agents',
	'caosVM_debug.cpp' => 'Debugging',
	'caosVM_files.cpp' => 'Files',
	'caosVM_flow.cpp' => 'Flow control',
	'caosVM_input.cpp' => 'Input',
	'caosVM_map.cpp' => 'Map',
	'caosVM_motion.cpp' => 'Motion',
	'caosVM_ports.cpp' => 'Ports',
	'caosVM_resources.cpp' => 'Resources',
	'caosVM_scripts.cpp' => 'Scripts',
	'caosVM_sounds.cpp' => 'Sound',
	'caosVM_time.cpp' => 'Time',
	'caosVM_variables.cpp' => 'Variables',
	'caosVM_vehicles.cpp' => 'Vehicles',
);

my %data;
my %ns;

my $prev;

while (<>) {
	my $file = $ARGV;
	
	unless (m{/\*\*}) {
		if (m/STUB|TODO/ && defined $prev && !defined $prev->{status}) {
			$prev->{status} = 'stub';
		}
		next;
	}
	$_ = <>;
	defined $_ or exit;
	next unless m{
		^\s*
		# DBG: and the like
		( \S+ \s+ )?
		# eg MOWS (command), LAWN (agent)
		(\S+) \s* \((\w+)\) \s*
		( (?:
			# argument bit
			# we parse this in more detail later
			(?:\w+) \s*
			(?:\([^)]+\)) \s*
		)* )
		\s*$
	}x;
	my ($cns, $cname, $ctype, $argdata) = ($1, $2, $3, $4);
	if (defined $cns) {
		$cns =~ s/\s//g;
	}

	my $fullname = ($cns ? "$cns " : "") . $cname;

	my $impl;
	if ($ctype eq 'command') {
		$impl .= 'c_';
	} else {
		$impl .= 'v_';
	}
	if ($cns && $cns ne '') {
		$_ = $cns . "_";
		$_ =~ s/[^a-zA-Z0-9_]//g;
		$impl .= uc $_;
	}
	$_ = $cname;
	$_ =~ s/[^a-zA-Z0-9_]//g;
	$impl .= $_;
	my $key = $impl;
	$impl = "caosVM::$impl";

	my @args;
	while ($argdata =~ s/.*?(\w+)\s*\(([^)]+)\)\s*//) {
		my ($argname, $argtype) = ($1, $2);
		push @args, {
			name => $argname,
			type => $argtype,
		};
	}

	my @lines;
	DOCLINE: while (<>) {
		last DOCLINE if m{\*/};
		$_ =~ m{^\s*(.*?)\s*$};
		push @lines, $1;
	}
	shift @lines while (@lines && $lines[0] eq '');
	pop @lines while (@lines && $lines[-1] eq '');

	my %pragma;
	my $status;
	my $cat;
	while (@lines && ($lines[0] =~ s{^\%([a-zA-Z]+)\s+}{} || $lines[0] =~ m{^\s*$})) {
		my $l = shift @lines;
		chomp $l;
		next unless $1;
		if ($1 eq 'pragma') {
			unless ($l =~ m{(\w+)\s*(.*)}) {
				warn "bad pragma";
			}
			$pragma{$1} = $2;
			chomp $pragma{$1};
			if ($pragma{$1} eq '') {
				$pragma{$1} = 1;
			}
		} elsif ($1 eq 'status') {
			if ($status) {
				die "Set status twice";
			}
			$status = $l;
			chomp $status;
		} elsif ($1 eq 'category') {
			if ($cat) { 
				die "set category twice";
			}
			$cat = $l;
		} else {
			die "Unrecognized directive: $1";
		}
	}

	if (!$cat) {
		$cat = lc $fnmap{$file} || 'unknown';
	}


	if ($pragma{implementation}) {
		$impl = $pragma{implementation};
	}

	my $desc = join("\n", @lines);
	$desc .= "\n";
	
	
	my $cd = {
		type => $ctype,
		name => $fullname,
		match => $cname,
		arguments => \@args,
		description => @lines ? $desc : undef,
		filename => $file,
		implementation => $impl,
		status => $status,
		category => $cat,
	};
	if (%pragma) {
		$cd->{pragma} = \%pragma;
	}
	if ($cns && $cns ne '') {
		$cd->{namespace} = lc $cns;
	}
	unless ($cd->{status}) {
		$cd->{status} = 'probablyok';
	}
	$prev = $cd;
	
	my @v = @variants;
	if ($pragma{variants}) {
		@v = grep { $_ ne ''; } split ' ', $pragma{variants};
	}

	for my $v (@v) {
		if (exists $data{$v}{$key}) {
			print STDERR "Name collision for ($key) in variant $v\n";
			exit 1;
		}
		$data{$v}{$key} = $cd;
	}
}

print Dump {
	variants => \%data,
	namespaces => [keys %ns],
};
# vim: set noet: 
