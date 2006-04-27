# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Cache;
use strict;
use warnings;

use Shake::Cache::SQL;
use Shake::Base;
use base 'Shake::Base';

our $VERSION  = 0.01;
our $Disabled;

BEGIN {
	eval {
		require DBI;
		require DBD::SQLite;
	};
	$Disabled = $@;
}

sub initialize {
	my ($self, $file) = @_;
	$file ||= find_cache();

	die "I am disabled!" if $Disabled;
	my $need_setup = not -e $file;
	my $dbh        = DBI->connect("dbi:SQLite:dbname=$file", "", "", 
		{ PrintError => 0, RaiseError => 1, AutoCommit => 1 });
	$self->{dbh}   = $dbh;
	
	$dbh->func( 'regexp', 2, sub { $_[1] =~ $_[0] }, 'create_function' );
	$self->setup() if $need_setup;
}

sub dbh { shift->{dbh} }

sub find_cache {
	if (exists $ENV{SHAKE_CACHE}) {
		return $ENV{SHAKE_CACHE};
	} else {
		if ($^O eq 'MSWin32') {
			return 'C:\\shake-cache.db';
		} else {
			return "$ENV{HOME}/.shake-cache.db";
		}
	}
}

sub setup {
	my ($self) = @_;
	my $dbh = $self->{dbh};

	eval {
		$dbh->do(SCHEMA);
	};
	if ($@) {
		unlink(find_cache());
		die $@;
	}
}

sub lookup {
	my ($self, $name, $version) = @_;
	my $dbh = $self->{dbh};
	my $sth = $dbh->prepare(LOOKUP);
	$sth->execute($name, $version);
	
	my $hash = $sth->fetchrow_hashref;

	$sth = undef;
	return $hash;
}

sub is_cached {
	my ($self, $name, $version) = @_;
	my $dbh = $self->{dbh};
	my $sth = $dbh->prepare(IS_CACHED);
	$sth->execute($name, $version);
	
	my ($cnt) = $sth->fetchrow_array;
	
	return $cnt == 1;
}

sub add {
	my ($self, $name, $version, $value) = @_;
	my $dbh = $self->{dbh};
	my $sth = $dbh->prepare(ADD);
	$sth->execute($name, $version, $value);
}

sub update {
	my ($self, $name, $version, $value) = @_;
	my $dbh = $self->{dbh};
	my $sth = $dbh->prepare(UPDATE);
	$sth->execute($value, $name, $version);
}

sub clear {
	my ($self, $name, $version) = @_;
	my $dbh = $self->{dbh};
	my $sth = $dbh->prepare(CLEAR);
	$sth->execute($name, $version);
}

sub clear_all {
	my ($self) = @_;
	my $dbh = $self->{dbh};
	my $sth = $dbh->prepare(CLEAR_ALL);
	$sth->execute();
}

sub is_disabled { $Disabled }


1;
