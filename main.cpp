/*
 *  main.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed 02 Jun 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005-2006 Bryan Donlan. All rights reserved.
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
#include <cstdlib> // EXIT_FAILURE

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#include "openc2e.h"
#include <iostream>
#include <stdio.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include "World.h"
#include "Engine.h"
#include "caosVM.h"
#include "SDLBackend.h"
#include "dialect.h" // registerDelegates

#ifdef _MSC_VER
#undef main // because SDL is stupid
#endif

namespace fs = boost::filesystem;
namespace po = boost::program_options;
extern fs::path homeDirectory(); // creaturesImage.cpp
extern fs::path cacheDirectory(); // creaturesImage.cpp
static const char data_default[] = "./data";

static void opt_version() {
	// We already showed the primary version bit, just throw in some random legalese
	std::cout << 
		"This is free software; see the source for copying conditions.  There is NO" << std::endl <<
		"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << std::endl << std::endl <<
		"...please don't sue us." << std::endl;
}

extern "C" int main(int argc, char *argv[]) {
	srand(time(NULL));

	try {
		
	std::cout << "openc2e (development build), built " __DATE__ " " __TIME__ "\nCopyright (c) 2004-2006 Alyssa Milburn and others\n\n";

	// variables for command-line flags
	int optret;
	bool enable_sound = true;
	std::vector<std::string> bootstrap;
	std::vector<std::string> data_vec;
	bool bs_specd = false, d_specd = false;
	world.gametype = "c3";

	// parse the command-line flags
	po::options_description desc;
	desc.add_options()
		("help,h", "Display help on command-line options")
		("version,V", "Display openc2e version")
		("silent,s", "Disable all sounds")
		("data-path,d", po::value< std::vector<std::string> >(&data_vec)->composing(),
		 "Set the path to the data directory")
		("bootstrap,b", po::value< std::vector<std::string> >(&bootstrap)->composing(),
		 "Sets or adds a path or COS file to bootstrap from")
		("gametype,g", po::value< std::string >(&world.gametype), "Set the game type (c1, c2, cv or c3)")
		("autokill,a", "Enable autokill")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	enable_sound = !vm.count("silent");
	
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if (vm.count("version")) {
		opt_version();
		return 0;
	}

	if (vm.count("autokill")) {
		std::cout << "Enabling autokill..." << std::endl;
		world.autokill = true;
	}

	if (vm.count("data-path") == 0)
		data_vec.push_back(data_default);

	// add all the data directories to the list
	for (std::vector<std::string>::iterator i = data_vec.begin(); i != data_vec.end(); i++) {
		fs::path datadir(*i, fs::native);
		if (!fs::exists(datadir)) {
			std::cerr << "data path '" << *i << "' doesn't exist, try --help" << std::endl;
			return 1;
		}
		world.data_directories.push_back(datadir);
	}
	
	// finally, add our cache directory to the end
	world.data_directories.push_back(cacheDirectory());
	
	// initial setup
	registerDelegates();
	std::cout << "Reading catalogue files..." << std::endl;
	world.initCatalogue();
	std::cout << "Initial setup..." << std::endl;
	world.init(); // just reads mouse cursor (we want this after the catalogue reading so we don't play "guess the filename")
	std::cout << "Reading PRAY files..." << std::endl;
	world.praymanager.update();
	std::cout << "Initialising backend..." << std::endl;
	engine.setBackend(new SDLBackend());
	engine.backend->init();
	if (enable_sound) engine.backend->soundInit();
	world.camera.setBackend(engine.backend); // TODO: hrr
	
	int listenport = engine.backend->networkInit();
	// inform the user of the port used, and store it in the relevant file
	std::cout << "listening on port " << listenport << std::endl;
	fs::path p = fs::path(homeDirectory().native_directory_string() + "/.creaturesengine", fs::native);
	if (!fs::exists(p))
		fs::create_directory(p);
	if (fs::is_directory(p)) {
		std::ofstream f((p.native_directory_string() + "/port").c_str(), std::ios::trunc);
		f << boost::str(boost::format("%d") % listenport);
	}

	if (world.data_directories.size() < 3) {
		// TODO: This is a hack for DS, basically. Not sure if it works properly. - fuzzie
		caosVar name; name.setString("engine_no_auxiliary_bootstrap_1");
		caosVar contents; contents.setInt(1);
		engine.eame_variables[name] = contents;
	}

	// execute the initial scripts!
	std::cout << "Executing initial scripts..." << std::endl;
	if (bootstrap.size() == 0) {
		world.executeBootstrap(false);
	} else {
		std::vector<std::string> scripts;
	
		for (std::vector< std::string >::iterator bsi = bootstrap.begin(); bsi != bootstrap.end(); bsi++) {
			fs::path scriptdir(*bsi, fs::native);
			if (!fs::exists(scriptdir)) {
				std::cerr << "couldn't find a specified script directory (trying " << *bsi << ")!\n";
				continue;
			}
			world.executeBootstrap(scriptdir);
		}
	}

	// if there aren't any metarooms, we can't run a useful game, the user probably
	// wanted to execute a CAOS script or something went badly wrong.
	if (world.map.getMetaRoomCount() == 0) {
		std::cerr << "\nNo metarooms found in given bootstrap directories or files, exiting." << std::endl;
		engine.backend->shutdown();
		return 0;
	}

	// do a first-pass draw of the world. TODO: correct?
	world.drawWorld();

	while (!engine.done) {
		engine.tick();
	} // main loop

	engine.backend->shutdown();

	} catch (std::exception &e) {
		std::cerr << "dying due to exception in main: " << e.what() << "\n";
		return 1;
	}
	return 0;
}

/* vim: set noet: */
