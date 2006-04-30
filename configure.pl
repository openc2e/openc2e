#!/usr/bin/perl
use strict;
use warnings;
use lib '.shake/lib';
use Shake::Script;
use Shake::Script::Checks qw( 
	program program.version
	c.compiler c.endian c.header
	sdl.cflags sdl.lflags
	cpp.compiler cpp.boost.version
	perl.module
);

shake_init("openc2e", "???", 'dylanwh@gmail.com');

check('c.compiler');
check('cpp.compiler');
check('c.endian', compiler => lookup('cpp.compiler'));
check('perl.module', 'YAML');
check('program', 'flex');
check('program.version', 'flex', '2.5.31');
check('program', 'sdl-config');
check('program', 'bison');
check('sdl.cflags');
check('sdl.lflags');
check('cpp.boost.version', '1.33.0', compiler => lookup('cpp.compiler'));


check(
	'c.header' => $_,
	cflags     => lookup('sdl.cflags'),
	compiler   => lookup('cpp.compiler'),
) foreach qw( SDL_mixer.h SDL_net.h );

my @boost_crap = qw( 
	boost/filesystem/path.hpp
	boost/program_options.hpp
	boost/serialization/base_object.hpp
	boost/tokenizer.hpp
	boost/shared_ptr.hpp
);

check(
	'c.header' => $_,
	compiler   => lookup('cpp.compiler'),
) foreach @boost_crap;

shake_done();

configure('config.mk');
