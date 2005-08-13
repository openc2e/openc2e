#!/usr/bin/perl
# vim: set noet: 
use strict;
use warnings;

use YAML;
use Template;

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

foreach my $key (keys %{$data->{ops}}) {
	$data->{ops}{$key}{key} = $key;
	$data->{ops}{$key}{hasdocs} = defined($data->{ops}{$key}{description});
	if ($data->{ops}{$key}{pragma}) {
	   my @pragma;
	   foreach my $pkey (sort keys %{$data->{ops}{$key}{pragma}}) {
		   push @pragma, { key => $pkey, value => $data->{ops}{$key}{pragma}{$pkey}};
	   }
	   $data->{ops}{$key}{pragma} = \@pragma;
   }
}

my %catsort;
foreach my $key (sort keys %{$data->{ops}}) {
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

my $vars = {
	categories => \@catl
};

my $tt = new Template;
$tt->process(\*DATA, $vars) or die $tt->error(), "\n";

__END__
<html>
	<head>
		<title>CAOS command reference - openc2e</title>
		<link rel="stylesheet" type="text/css" href="docs.css" />
	</head>
	<body>
		<h1>CAOS command reference - openc2e dev build</h1>
		<div id="index"><ul>
			[% FOREACH cat = categories %]
			<li><a href="#c_[%- cat.anchor -%]">[% cat.name %]</a></li>
			[% END %]
		</ul></div>
		[% FOREACH cat = categories %]
		<div class="category" id="c_[%- cat.anchor -%]">
			<h2>[% cat.name %]</h2><hr/>
			<!-- XXX: hr is not semantic -->

			[% FOREACH op = cat.ents %]
			<div class="command" id="k_[% op.key %]">
				<div class="header">
					<span class="cmdname">
						[% op.name %]
					</span>
					<span class="cmdtype">
						([% op.type %])
					</span>
					[% FOREACH arg = op.arguments -%]
					<span class="argname">
						[% arg.name %]
					</span>
					<span class="argtype">
						([% arg.type %])
					</span>
					[% END %]
				</div>
				<div class="description">
				[% UNLESS op.hasdocs %]
				<div class="nodocs">This command is currently undocumented.</div>
				[% ELSE %]
				<div class="docs">
					[% op.description %]
				</div>
				[% END %]
				</div>
				<div class="status">
					[% IF op.status == 'todo' || op.status == 'stub' %]
					<div class="st_todo">
						This command is not yet implemented.
					</div>
					[% ELSIF op.status == 'probablyok' %]
					<div class="st_maybe">
						This command probably works, but it has not been annotated with its status.
					</div>
					[% ELSIF op.status == 'maybe' %]
					<div class="st_maybe">
						This command is believed to work, but has not yet been thoroughly tested.
					</div>
					[% ELSIF op.status == 'broken' %]
					<div class="st_todo">
						This command is partially implemented but missing large amounts
						of functionality.
					</div>
					[% ELSIF op.status == 'ok' || op.status == 'done' %]
					<div class="st_ok">
						This command works properly.
					</div>
					[% ELSE %]
					<div class="st_wtf">
						This command has an unknown status tag of: [% op.status %]. Please beat whoever set that status with a shovel or some other heavy object.
					</div>
					[% END %]
				</div>	

				<div class="administrivia">
					<ul>
						<li>Implemented in: [% op.filename %]</li>
						<li>Implementation function (may be wrong):
							[% op.implementation %]
						</li>
						[% IF op.pragma %]
						<li>Pragmas:
						<ul>
							[% FOREACH pragma = op.pragma %]
							<li> [% pragma.key %] =&gt; [% pragma.value %] </li>
							[% END %]
						</ul>
						[% END %]
					</ul>
				</div>
			</div> <!-- command -->
			[% END %]
		</div> <!-- category -->
		[% END %]
	</body>
</html>
