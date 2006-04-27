#!/usr/bin/perl
use strict;
use warnings;
use lib '.shake/lib';
use Shake::Script;
use Shake::Checks qw( 
	Program
	Program::Version
	C::Compiler
	Cpp::Compiler
	C::Endian
	C::Header
	Perl::Module 
	SDL::CFlags
	SDL::LFlags
);

shake_init("openc2e", "???", 'dylanwh@gmail.com');

check_c_compiler();
check_cpp_compiler();
check_c_endian(compiler => lookup('cpp.compiler'));
check_perl_module('YAML');
check_program('flex');
check_program_version('flex', '2.5.31');
check_program('sdl-config');
check_program('bison');
check_sdl_cflags();
check_sdl_lflags();

check_c_header($_,
	cflags => lookup('sdl.cflags'),
	compiler => lookup('cpp.compiler'),
) foreach qw( SDL_mixer.h SDL_net.h );

my @boost_crap = qw( 
	boost/filesystem/path.hpp
	boost/program_options.hpp
	boost/serialization/base_object.hpp
	boost/tokenizer.hpp
	boost/shared_ptr.hpp
);

check_c_header($_,
	compiler => lookup('cpp.compiler'),
) foreach @boost_crap;

ensure (
	default  => 'required',
);

configure('config.mk');
save_config();
