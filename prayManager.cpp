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

#include "pray.h"
#include "exceptions.h"
#include "World.h" // data_directories
#include <boost/filesystem/convenience.hpp>

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

	if (!world.catalogue.hasTag("Pray System File Extensions"))
		throw creaturesException("couldn't find the catalogue tag \"Pray System File Extensions\"");

	const std::vector<std::string> &extensions = world.catalogue.getTag("Pray System File Extensions");

	for (std::vector<fs::path>::iterator i = world.data_directories.begin(); i != world.data_directories.end(); i++) {
		assert(fs::exists(*i));
		assert(fs::is_directory(*i));

		fs::path praydir(*i / fs::path("/My Agents/", fs::native));
		
		if (fs::exists(praydir) && fs::is_directory(praydir)) {
			fs::directory_iterator fsend;
			for (fs::directory_iterator d(praydir); d != fsend; ++d) {
				for (std::vector<std::string>::const_iterator z = extensions.begin(); z != extensions.end(); z++) {
					std::string x = fs::extension(*d);
					x.erase(x.begin());
					if (std::find(extensions.begin(), extensions.end(), x) != extensions.end()) {
						// TODO: language checking!
						//std::cout << "scanning PRAY file " << d->native_directory_string() << std::endl;
						try {
							prayFile *p = new prayFile(*d);
							addFile(p);
						} catch (creaturesException &e) {
							std::cerr << "PRAY file " << d->native_directory_string() << "failed to load: " << e.what() << std::endl;
						}
					}
				}
			}
		}
	}
}

/* vim: set noet: */
