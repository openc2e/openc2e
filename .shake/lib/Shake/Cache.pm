# vim: set ts=4 sw=4 noexpandtab si ai sta tw=100:
# This module is copyrighted, see end of file for details.
package Shake::Cache;
use strict;
use warnings;

use Shake::Base;
use base 'Shake::Base';

use constant SCHEMA => q{
	CREATE TABLE cache (
		id      INTEGER      PRIMARY KEY,
		name    VARCHAR(255) NOT NULL,
		value   TEXT         NULL,
		version FLOAT        NOT NULL,
		created UNIXTIME     NOT NULL DEFAULT now(),
		updated UNIXTIME     NOT NULL DEFAUKT now(),
		UNIQUE (name, version)
	)
};
use constant LOOKUP => 'SELECT * FROM cache WHERE name = ? AND version = ? LIMIT 1';
use constant ADD    => q{
	INSERT INTO cache (name, version, value, created, updated) 
	VALUES            (?,    ?,     ?,       ?,       ?)
};
use constant UPDATE => q{
	UPDATE cache
	SET value = ?, updated = ?
	WHERE name = ? AND version = ?
};

use constant IS_CACHED => q{
	SELECT count(*) FROM cache WHERE name = ? AND version = ?
};

our $VERSION = 0.01;
our $Disabled = undef;

eval {
	require DBI;
	require DBD::SQLite;
};
$Disabled = $@;

sub initialize {
	my ($self, $file) = @_;
	$file ||= find_cache();

	my $need_setup = not -e $file;
	my $dbh        = DBI->connect("dbi:SQLite:dbname=$file", "", "", 
		{ PrintError => 0, RaiseError => 1, AutoCommit => 1 });
	$self->{dbh}   = $dbh;
	$self->setup() if $need_setup;
	
	$dbh->func( 'now', 0, sub { return time }, 'create_function' );
	$dbh->func( 'regexp', 2, sub { $_[0] =~ $_[1] }, 'create_function' );
}

sub dbh { shift->{dbh} }

sub is_disabled { $Disabled }

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

	$dbh->do(SCHEMA);
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
	$sth->execute($name, $version, $value, time, time);
}

sub update {
	my ($self, $name, $version, $value) = @_;
	my $dbh = $self->{dbh};
	my $sth = $dbh->prepare(UPDATE);
	$sth->execute($value, time, $name, $version);
}

1;
