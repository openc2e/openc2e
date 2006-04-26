package Shake::Cache::SQL;
use strict;
use warnings;
use Exporter;
use base 'Exporter';

our @EXPORT = qw( 
	SCHEMA LOOKUP ADD UPDATE IS_CACHED CLEAR CLEAR_ALL
);


use constant SCHEMA => q{
	CREATE TABLE cache (
		name    VARCHAR(255) NOT NULL,
		value   TEXT         NULL,
		version FLOAT        NOT NULL,
		created TIMESTAMP     NOT NULL DEFAULT CURRENT_TIMESTAMP,
		updated TIMESTAMP     NOT NULL DEFAULT CURRENT_TIMESTAMP,
		PRIMARY KEY (name, version)
	)
};
use constant LOOKUP => 'SELECT * FROM cache WHERE name = ? AND version = ? LIMIT 1';
use constant ADD    => q{
	INSERT INTO cache (name, version, value) 
	VALUES            (?,    ?,       ?)
};
use constant UPDATE => q{
	UPDATE cache
	SET value = ?, updated = CURRENT_TIMESTAMP
	WHERE name = ? AND version = ?
};

use constant IS_CACHED => q{
	SELECT count(*) FROM cache WHERE name = ? AND version = ?
};

use constant CLEAR => q{
	DELETE FROM cache WHERE name = ? AND version = ?
};

use constant CLEAR_ALL => q{
	DELETE FROM cache
};


