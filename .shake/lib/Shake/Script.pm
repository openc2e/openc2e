# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Script;
use strict;
use warnings;
use Fatal qw( mkdir );
use Getopt::Long;
use Carp;
use Exporter;
use base 'Exporter';

use Shake::Frontend;

use constant SHAKE_DIR => '.shake';
use constant SHAKE_LOG => SHAKE_DIR . "/log";
use constant SHAKE_DUMP => SHAKE_DIR . "/dump.pl";


our $VERSION   = 0.01;
our @EXPORT    = qw( $Frontend init done check checked configure lookup has load_check set checking );
our @EXPORT_OK = qw( load_check );
our $Frontend;

{
	no strict 'refs';
	foreach my $name (qw( has lookup checked )) {
		*{$name} = sub {
			$Frontend->engine->$name(@_);
		};
	}
	foreach my $name (qw( check checking configure done set load_check )) {
		*{$name} = sub {
			$Frontend->$name(@_);
		};
	}

}

sub init {
	my %option;

	Getopt::Long::Configure('gnu_getopt');
	GetOptions(\%option, 
		qw(
			rerun|R
			nocache|C
			define|with|D=s%
			undefine|without|U=s@
			prefix=s
			ignore|i=s@
			help
			version
		)
	) or exit(1);

	if (exists $option{prefix}) {
		$option{define}{prefix} = delete $option{prefix};
	}
	$Frontend = new Shake::Frontend(%option);
	$Frontend->is_script(1);
	$Frontend->init(@_);
}

1;
