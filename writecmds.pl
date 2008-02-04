#!/usr/bin/perl

use strict;
use warnings;

use YAML;
use POSIX qw(strftime);

my %tdisp = (
	'any' => 'CI_ANYVALUE',
	'float' => 'CI_NUMERIC',
	'integer' => 'CI_NUMERIC',
	'string' => 'CI_STRING',
	'agent' => 'CI_AGENT',
	'bytestring' => 'CI_BYTESTR',
	'variable' => 'CI_VARIABLE',
	'any' => 'CI_OTHER',
	'anything' => 'CI_OTHER',
	'condition' => undef,
	'comparison' => undef,
	'decimal' => 'CI_NUMERIC',
	'decimal variable' => 'CI_OTHER',
	'byte-string' => 'CI_BYTESTR',
	'label' => undef,
	'vector' => 'CI_VECTOR',
	'bareword' => 'CI_BAREWORD',
	'token' => 'CI_BAREWORD',
	'subcommand' => 'CI_SUBCOMMAND',
	'command' => 'CI_COMMAND',
);

# zero-tolerance policy
$SIG{__WARN__} = sub { die $_[0] };

my $data = YAML::LoadFile($ARGV[0]);

my $disp_id = 1;
my %disp_tbl;
my @init_funcs;

print qq{// THIS IS AN AUTOMATICALLY GENERATED FILE\n};
print qq{// DO NOT EDIT\n};
print qq{// Generated at }, strftime("%c", localtime(time)), qq{\n};
print qq{\n\n};
print qq{#include <string>\n};
print qq{#include <cstdio>\n};
print qq{#include <climits>\n};
print qq{#include "cmddata.h"\n};
print qq{#include "caosVM.h"\n};
print qq{#include "dialect.h"\n};
print qq{#include "openc2e.h"\n};
print qq{\n\n};

foreach my $variant_name (sort keys %{$data->{variants}}) {
	my $variant = $data->{variants}{$variant_name};
	for my $key (keys %$variant) {
		$variant->{$key}{key} = $key;
	}
	my @cmds = values %$variant;

	inject_ns(\@cmds);
	writelookup(\@cmds);
	checkdup(\@cmds, "$variant_name commands");
	sortname(\@cmds);
	miscprep($variant_name, \@cmds);

	printarr(\@cmds, $variant_name, "${variant_name}_cmds");

	printinit($variant_name, "${variant_name}_cmds");
}

printdispatch();

print "void registerAutoDelegates() {\n";
for my $f(@init_funcs) {
	print "\t$f();\n";
}
print "}\n";


exit 0;

sub miscprep {
	my ($variant, $cmds) = @_;
	
	for my $cmd (@$cmds) {
		$cmd->{evalcost}{$variant} = $cmd->{evalcost}{default} unless defined $cmd->{evalcost}{$variant};
		if ($cmd->{type} ne 'command' && $cmd->{evalcost}{$variant} != 0) {
			print STDERR "$cmd->{lookup_key} has non-zero evalcost in an expression cost.";
			print STDERR "This causes a race condition which can potentially lead to crashes.";
			print STDERR "If you really need this, please contact bd_. Aborting for now.";
			exit 1;
		}
	}
}

sub printinit {
	my ($variant, $cmdarr, $exparr) = @_;
	print "static void init_$variant() {\n";
	print qq{\tdialects["$variant"] = boost::shared_ptr<Dialect>(new Dialect($cmdarr, std::string("$variant")));\n};
	print "}\n";
	push @init_funcs, "init_$variant";
}

sub printdispatch {
	print "#ifdef VCPP_BROKENNESS\n";
	print "void dispatchCAOS(class caosVM *vm, int idx) {\n";
	print "\tswitch (idx) {\n";
	for my $impl (keys %disp_tbl) {
		print "\t\tcase $disp_tbl{$impl}: vm->$impl(); break;\n";
	}
	print qq{\t\tdefault:\n\t\t\{\n};
	print qq{\t\t\tchar buf[256];\n};
	print qq{\t\t\tsprintf(buf, "%d", idx);\n};
	print qq{\t\t\tthrow caosException(std::string("Unknown dispatchCAOS index: ") + buf);\n};
	print qq{\t\t\}\n};
	print "\t}\n";
	print "}\n";
	print "#endif\n";
}

sub writelookup {
	my $cmds = shift;

	for my $cmd (@$cmds) {
		my $prefix = 'expr ';
		if ($cmd->{type} eq 'command') {
			$prefix = 'cmd ';
		}
		$cmd->{lookup_key} = $prefix . lc($cmd->{name});
	}
}


sub printarr {
	my ($cmds, $variant, $arrname) = @_;
	my $buf = '';
	$buf .= "static const struct cmdinfo $arrname\[\] = {\n";
	my $idx = 0;
	for my $cmd (@$cmds) {
		my $argp = 'NULL';
		if (defined $cmd->{arguments}) {
			my $args = '';
			for my $arg (@{$cmd->{arguments}}) {
				my $type = $tdisp{$arg->{type}};
				if (!defined $type) {
					undef $args;
					last;
				}
				$args .= "$type, ";
			}
			if (defined $args) {
				$argp = "${arrname}_t_$cmd->{type}_$cmd->{key}";
				print "static const enum ci_type $argp\[\] = { ";
				print $args;
				print "CI_END };\n";
			}
		}

		$buf .= "\t{ // $idx $cmd->{key}\n";
		$idx++;
		
		$buf .= "#ifndef VCPP_BROKENNESS\n";
		unless (defined($cmd->{implementation})) {
			$cmd->{implementation} = 'caosVM::dummy_cmd';
		}
		unless (defined($cmd->{saveimpl})) {
			$cmd->{saveimpl} = 'caosVM::dummy_cmd';
		}
		$buf .= "\t\t&$cmd->{implementation}, // handler\n";
		$buf .= "\t\t&$cmd->{saveimpl}, // savehandler\n";
		$buf .= "#else\n";
		$buf .= sprintf "\t\t%d, // handler_idx\n", handler_idx($cmd->{implementation});
		$buf .= sprintf "\t\t%d, // savehandler_idx\n", handler_idx($cmd->{saveimpl});
		$buf .= "#endif\n";

		$buf .= qq{\t\t"$cmd->{lookup_key}", // lookup_key\n};
		$buf .= qq{\t\t"$cmd->{key}", // key\n};
		$buf .= qq{\t\t"}. lc $cmd->{match}. qq{", // name\n};
		$buf .= qq{\t\t"$cmd->{name}", // fullname\n};
		$buf .= qq{\t\t"}. cescape($cmd->{description}). qq{", // docs\n};
		$buf .= "\t\t". scalar(@{$cmd->{arguments}}). ", // argc\n";
		$buf .= "\t\t$cmd->{stackdelta}, // stackdelta\n";
		$buf .= "\t\t$argp, // argtypes\n";
		
		my $rettype = $tdisp{$cmd->{type}};
		if (!defined $rettype) {
			die "Unknown return type $cmd->{type} in $cmd->{name}: ".YAML::Dump($cmd);
		}
		$buf .= "\t\t$rettype, // rettype\n";
		my $cost = $cmd->{evalcost}{$variant};
		$buf .= "\t\t$cost // evalcost\n";
		$buf .= "\t},\n";

	}
	$buf .= "\t{ NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL, CI_END, 0 }\n";

	$buf .= "};\n";
	print $buf;
}
sub sortname {
	my $cmds = shift;
	@$cmds = sort { $a->{lookup_key} cmp $b->{lookup_key} } @$cmds;
}

sub inject_ns {
	my $cmds = shift;
	my %ns;
	my %names;
	for my $cmd (@$cmds) {
		my $type = ($cmd->{type} eq 'command') ? 'command' : 'any';
		$ns{$cmd->{namespace}}{$type}++ if defined $cmd->{namespace};
		$names{lc "$type $cmd->{name}"}++;
	}
	for my $ns (keys %ns) {
		for my $type (keys %{$ns{$ns}}) {
			next if $ns eq 'face'; # hack
			next if exists $names{"$type " . lc $ns};
			my $key = 'k_' . uc $ns;
			$key =~ s/[^a-zA-Z0-9_]//g;
			push @$cmds, {
				arguments => [ {
					name => "cmd",
					type => "subcommand",
				} ],
				category => "internal",
				description => "",
				evalcost => { default => 0 },
				filename => "",
				implementation => undef,
				match => uc $ns,
				name => lc $ns,
				pragma => {},
				status => 'internal',
				key => $key,
				type => $type,
				syntaxstring => (uc $ns) . " (command/expr) subcommand (subcommand)\n",
				stackdelta => "INT_MAX",
			};
		}
	}
}

sub checkdup {
	my ($cmds, $desc) = @_;
	my %mark;
	for my $cmd (@$cmds) {
		if (!defined $cmd->{lookup_key}) {
			print STDERR "No name for $cmd->{key}\n";
			exit 1;
		}
		push @{$mark{$cmd->{lookup_key}}}, $cmd;
		if (scalar @{$mark{$cmd->{lookup_key}}} > 1) {
			# Please do not disable this assert
			# bsearch()'s behavior is unpredictable with duplicate keys
			print STDERR "Duplicate command in $desc: $cmd->{lookup_key}\n";
			print STDERR YAML::Dump($mark{$cmd->{lookup_key}});
			exit 1;
		}
	}
}



our %cescapes;
BEGIN { %cescapes = (
	"\n" => "\\n",
	"\r" => "\\r",
	"\t" => "\\t",
	"\\" => "\\\\",
	"\"" => "\\\"",
); }

sub cescape {
	my $str = shift;
	if (!defined $str) { return ""; }
	my $ces = join "", keys %cescapes;
	$str =~ s/([\Q$ces\E])/$cescapes{$1}/ge;
	return $str;
}

sub handler_idx {
	my $impl = $_[0];
	unless (defined($disp_tbl{$impl})) {
		$disp_tbl{$impl} = $disp_id++;
	}
	return $disp_tbl{$impl};
}
# vim: set noet: 
