/*
 *  caosVM_ser_real.cpp
 *  openc2e
 *
 *  Created by Bryan Donlan on Sun 21 May 2006
 *  Copyright (c) 2006 Bryan Donlan. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "caosVM.h"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
//#include "ser/s_map.h"
#include "World.h"
#include "ser/s_Scriptorium.h"
#include "serialization.h"

#include <fstream>

// Note: this file may require an exorbitant amount of RAM to compile
// You have been warned.

void c_SERS_MAPP(caosVM* vm) {
	VM_PARAM_STRING(filename)
#if 0
	std::ofstream outf(filename, std::ios::binary);
	{
		boost::archive::text_oarchive oa(outf);
		oa << (const Map &)world.map;
		std::string test("TEST MARKER ETC");
		oa << (const std::string &) test;
	}
	outf.close();
#endif
}

void c_SERL_MAPP(caosVM* vm) {
	VM_PARAM_STRING(filename);
#if 0
	std::ifstream inf(filename, std::ios::binary);
	{
		boost::archive::text_iarchive ia(inf);
		try {
			world.map->Reset();
			ia >> world.map; // if this fails the game will explode
		} catch (std::exception &e) {
			fmt::print(stderr, "Deserialization failed; game is in an unknown state!\n" );
			fmt::print(stderr, "Exception was: {}\n", e.what() );
			fmt::print(stderr, "Aborting...\n" );
			throw;
		}
	}
#endif
}

void c_SERS_SCRP(caosVM* vm) {
	VM_PARAM_STRING(filename);
	std::ofstream ofs(filename, std::ios::binary);
	{
		boost::archive::text_oarchive oa(ofs);
		oa << (const Scriptorium&)world.scriptorium;
	}
	ofs.close();
}

void c_SERL_SCRP(caosVM* vm) {
	VM_PARAM_STRING(filename);
	std::ifstream inf(filename, std::ios::binary);
	{
		boost::archive::text_iarchive ia(inf);
		try {
			ia >> world.scriptorium; // if this fails the game will explode
		} catch (std::exception& e) {
			fmt::print(stderr, "Deserialization failed; game is in an unknown state!\n");
			fmt::print(stderr, "Exception was: {}\n", e.what());
			fmt::print(stderr, "Aborting...\n");
			throw;
		}
	}
}
