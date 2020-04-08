/*
 *  prayManager.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon Jan 16 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#include "prayManager.h"
#include "exceptions.h"
#include "World.h" // data_directories
#include "Catalogue.h"
#include <boost/filesystem.hpp>

prayManager::~prayManager() {
	while (files.size() != 0) {
		prayFile *f = files[0];
		removeFile(f);
		delete f;
	}

	assert(blocks.size() == 0);
}

void prayManager::addFile(prayFile *f) {
	std::vector<prayFile *>::iterator p = std::find(files.begin(), files.end(), f);
	assert(p == files.end());
	files.push_back(f);

	for (std::vector<prayBlock *>::iterator i = f->blocks.begin(); i != f->blocks.end(); i++) {
		if (blocks.find((*i)->name) != blocks.end()) // garr, block conflict
			continue;
		//assert(blocks.find((*i)->name) == blocks.end());
		blocks[(*i)->name] = *i;
	}
}

void prayManager::removeFile(prayFile *f) {
	std::vector<prayFile *>::iterator p = std::find(files.begin(), files.end(), f);
	assert(p != files.end());
	files.erase(p);
	
	for (std::vector<prayBlock *>::iterator i = f->blocks.begin(); i != f->blocks.end(); i++) {
		if (blocks.find((*i)->name) == blocks.end()) // garr, block conflict
			continue;
		/*assert(blocks.find((*i)->name) != blocks.end());
		assert(blocks[(*i)->name] == *i); */
		blocks.erase(blocks.find((*i)->name));
	}
}

void prayManager::update() {
	if (files.size() != 0) return; // TODO: Handle actual update cases, rather than just the initial init.

	if (!catalogue.hasTag("Pray System File Extensions")) {
		std::cout << "Warning: Catalogue tag \"Pray System File Extensions\" wasn't found, so no PRAY files will be loaded." << std::endl;
		return;
	}

	const std::vector<std::string> &extensions = catalogue.getTag("Pray System File Extensions");

	for (std::vector<fs::path>::iterator i = world.data_directories.begin(); i != world.data_directories.end(); i++) {
		assert(fs::exists(*i));
		assert(fs::is_directory(*i));

		fs::path praydir(*i / fs::path("My Agents/"));

		if (fs::exists(praydir) && fs::is_directory(praydir)) {
			fs::directory_iterator fsend;
			for (fs::directory_iterator d(praydir); d != fsend; ++d) {
				std::string x = d->path().extension().string();
				if (!x.empty()) x.erase(x.begin());
				if (std::find(extensions.begin(), extensions.end(), x) != extensions.end()) {
					// TODO: language checking!
					//std::cout << "scanning PRAY file " << d->path().string() << std::endl;
					try {
						prayFile *p = new prayFile(*d);
						addFile(p);
					} catch (creaturesException &e) {
						std::cerr << "PRAY file \"" << d->path().string() << "\" failed to load: " << e.what() << std::endl;
					}
				}
			}
		}
	}
}

std::string prayManager::getResourceDir(unsigned int type) {
	switch (type) {
		case 0: return ""; // main
		case 1: return "Sounds/"; // sounds
		case 2: return "Images/"; // images
		case 3: return "Genetics/"; // genetics
		case 4: return "Body Data/"; // body data
		case 5: return "Overlay Data/"; // overlay data
		case 6: return "Backgrounds/";// backgrounds
		case 7: return "Catalogue/"; // catalogue
		//case 8: return "Bootstrap/"; // bootstrap
		//case 9: return "My Worlds/"; // my worlds
		case 10: return "My Creatures/";// my creatures
		case 11: return "My Agents/"; // my agents
	}

	return "";
}

/* vim: set noet: */
