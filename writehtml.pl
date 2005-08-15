#!/usr/bin/perl
# vim: set noet: 
use strict;
use warnings;

use YAML;

# this may also appear as something like openc2e/mirror/trunk/writehtml.pl 43539 2005-08-15T00:44:28.013173Z bdonlan (between dollar-sign Id dollar-sign)
my $id = '$Id$';

sub captext ($);

BEGIN {
	eval {
		require Text::Capitalize;
		import Text::Capitalize;
	};
	if ($@) {
		*captext = sub ($) {
			my $t = shift;
			$t =~ s/^([a-z])/uc $1/e;
			return $t;
		};
	} else {
		*captext = sub ($) {
			return capitalize_title($_[0]);
		};
	}
}

sub esc ($) {
	my $t = shift;
	$t =~ s/\W/_/g;
	return $t;
}

my $data = YAML::LoadFile('commandinfo.yml');

my %catsort;
foreach my $key (sort keys %{$data->{ops}}) {
	$data->{ops}{$key}{key} = $key;
	push @{$catsort{$data->{ops}{$key}{category}}}, $data->{ops}{$key};
}

foreach my $key (keys %catsort) {
	$catsort{$key} = [
		map { $_->[0] }
		sort { $a->[1] cmp $b->[1] }
		map { [ $_, $_->{name} ] }
		@{$catsort{$key}}
	];
}

my @catl = map { { name => captext($_), ents => $catsort{$_}, anchor => esc($_) } } sort keys %catsort;
my $time = scalar gmtime;
print <<END;
<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/2002/REC-xhtml1-20020801/DTD/xhtml1-transitional.dtd">
<html>
	<head>
		<title>CAOS command reference - openc2e</title>
		<link rel="stylesheet" type="text/css" href="docs.css" />
	</head>
	<body>
		<h1>CAOS command reference - openc2e dev build</h1>
                <i>Last updated $time</i>
		<div id="index"><ul>
END

foreach my $c (@catl) {
	print "<li><a href=\"#c_", $c->{anchor}, "\">", $c->{name}, "</a></li>\n";
}
print "</ul></div>\n";

my %st_insert = (
	todo => ['st_todo', 'This command is not yet implemented.', 'stubs', 0],
	probablyok => ['st_maybe', 'This command probably works, but it has not been annotated with its status.', 'unknown', 0],
	maybe => ['st_maybe', 'This command is believed to work, but has not yet been thoroughly tested.', 'untested', 0],
	broken => ['st_todo', 'This command is partially implemented but missing large amounts of functionality, or is nonconformant in some vital way.', 'broken', 0],
	ok => ['st_ok', 'This command works properly.', 'done', 0],
);

$st_insert{stub} = $st_insert{todo};
$st_insert{done} = $st_insert{ok};

foreach my $op (values %{$data->{ops}}) {
	$st_insert{$op->{status}}[3]++;
}

my @cstat;
foreach my $clas (qw(ok broken maybe probablyok todo)) {
	if ($st_insert{$clas}[3] != 0) {
		push @cstat, "$st_insert{$clas}[3] $st_insert{$clas}[2]";
	}
}


print <<END;
	<div id="sidebar">
	<h6>Commands</h6>
	<ul>
END

foreach my $key (grep { /^c_/ } sort keys %{$data->{ops}}) {
	my $class = $st_insert{$data->{ops}{$key}{status}}[0] || 'st_wtf';
	print qq{<li><a class="$class" href="#k_$key">$data->{ops}{$key}{name}</a></li>};
}

print <<END;
	</ul><hr />
	<h6>Expressions</h6>
	<ul>
END

foreach my $key (grep { /^v_/ } sort keys %{$data->{ops}}) {
	my $class = $st_insert{$data->{ops}{$key}{status}}[0] || 'st_wtf';
	print qq{<li><a class="$class" href="#k_$key">$data->{ops}{$key}{name}</a></li>};
}
	
print '</ul></div><div id="content">';

print '<div class="summary">';
print scalar keys %{$data->{ops}}, " commands in total; ";
print join ", ", @cstat;
print ".</div>";

foreach my $cat (@catl) {
	print qq{<div class="category" id="c_$cat->{anchor}">\n};
	print qq{<h2>$cat->{name}</h2><hr/>\n};
	foreach my $op (@{$cat->{ents}}) {
		print qq{<div class="command" id="k_$op->{key}">\n};
		print qq{<div class="header">\n};
		print qq{<span class="cmdname">$op->{name}</span>\n};
		print qq{<span class="cmdtype">($op->{type})</span>\n};
		foreach my $arg (@{$op->{arguments}}) {
			print qq{<span class="argname">$arg->{name}</span>\n};
			print qq{<span class="argtype">($arg->{type})</span>\n};
		}
		print qq{</div><div class="description">};
		unless (defined $op->{description}) {
			print qq{<div class="nodocs">This command is currently undocumented.</div>\n};
		} else {
			print qq{<div class="docs">$op->{description}</div>\n};
		}
		print qq{</div><div class="status">};
		if (defined $st_insert{$op->{status}}[0]) {
			print qq{<div class="$st_insert{$op->{status}}[0]">};
			print $st_insert{$op->{status}}[1];
			print qq{</div>};
		} else {
			print qq{<div class="st_wtf">This command has an unknown status tag of: $op->{status}.};
			print qq{Please beat whoever set that status with a shovel or some other heavy object.};
			print qq{</div>};
		}
		print qq{</div>};
		print qq{<div class="administrivia"><ul>};
		print qq{<li>Implemented in: $op->{filename}</li>};
		print qq{<li>Implementation functions (may be wrong): $op->{implementation}</li>};
		if ($op->{pragma}) {
			print qq{<li>Pragmas:<ul>};
			foreach my $pk (sort keys %{$op->{pragma}}) {
				print qq{<li>$pk =&gt; $op->{pragma}{$pk}</li>};
			}
			print qq{</ul></li>};
		}
		print qq{</ul></div>};
		print qq{</div>};
	}
	print qq{</div>};
}

print qq{</div></body></html>};






		
