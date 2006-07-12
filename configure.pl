#!/usr/bin/perl
use strict;
use warnings;
use lib '.shake/lib';
use Shake::Script;

init('openc2e', '0.0', 'dylanwh@gmail.com');
checking( qw{
	prefix
	program program.version
	c.compiler endian c.header cpp.header
	sdl
	cpp.compiler cpp.boost.version
	perl.module
});


check('prefix');
check('endian');
check('perl.module', 'YAML');
check('program.version', 'flex', '2.5.31');
check('program', 'bison');

# this sets sdl, sdl.cflags, and sdl.lflags
check('sdl');

my $cxxflags = '-W -Wall -Wno-conversion -Wno-unused -DYYERROR_VERBOSE';
set CXXFLAGS => join(' ', lookup('sdl.cflags'), $ENV{CXXFLAGS} || '', $cxxflags);
set LDFLAGS  => join(' ', qw( 
	-lboost_program_options 
	-lboost_serialization 
	-lboost_filesystem ),
	lookup('sdl.lflags'),
	$ENV{'LDFLAGS'} || '',
	qw( -lz -lm -lSDL_net -lSDL_mixer -lpthread ));

check('cpp.boost.version', '1.33.0');
check( 'cpp.header' => $_ ) for qw(
	SDL_mixer.h
	SDL_net.h
	boost/filesystem/path.hpp
	boost/program_options.hpp
	boost/serialization/base_object.hpp
	boost/tokenizer.hpp
	boost/shared_ptr.hpp
);



configure('config.mk');

done();
