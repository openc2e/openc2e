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

#include "Catalogue.h"
#include "World.h" // data_directories
#include "creaturesException.h"
#include "fileformats/PrayFileReader.h"

#include <cassert>
#include <ghc/filesystem.hpp>

PrayBlock::PrayBlock() {
}

PrayBlock::PrayBlock(const std::string& filename_, const std::string& type_, const std::string& name_, bool compressed_)
	: loaded(false), tagsloaded(false), buffer(), compressed(compressed_),
	  size(0), compressedsize(0), filename(filename_), type(type_), name(name_) {
}

PrayBlock::~PrayBlock() {
}

void PrayBlock::load() {
	if (loaded) {
		return;
	}

	std::ifstream in(filename, std::ios::binary);
	if (!in)
		throw creaturesException(std::string("couldn't open PRAY file \"") + filename + "\"");
	PrayFileReader file(in);

	for (size_t i = 0; i < file.getNumBlocks(); i++) {
		if (!(file.getBlockType(i) == type && file.getBlockName(i) == name)) {
			continue;
		}
		loaded = true;
		buffer = file.getBlockRawData(i);
		size = buffer.size();
		return;
	}

	throw creaturesException("Couldn't load " + type + " block '" + name + "' from file '" + filename + "'");
}

void PrayBlock::parseTags() {
	load(); // TODO: don't double read

	if (tagsloaded) {
		return;
	}

	std::ifstream in(filename, std::ios::binary);
	if (!in)
		throw creaturesException(std::string("couldn't open PRAY file \"") + filename + "\"");
	PrayFileReader file(in);

	for (size_t i = 0; i < file.getNumBlocks(); i++) {
		if (!(file.getBlockType(i) == type && file.getBlockName(i) == name)) {
			continue;
		}
		tagsloaded = true;
		auto tags = file.getBlockTags(i);
		integerValues = tags.first;
		stringValues = tags.second;
		return;
	}

	throw creaturesException("Couldn't load " + type + " block '" + name + "' from file '" + filename + "'");
}


prayManager::~prayManager() {
}

void prayManager::addFile(const fs::path& filename) {
	std::ifstream in(filename.string(), std::ios::binary);
	if (!in)
		throw creaturesException(std::string("couldn't open PRAY file \"") + filename.string() + "\"");
	PrayFileReader f(in);

	for (size_t i = 0; i < f.getNumBlocks(); i++) {
		if (blocks.find(f.getBlockName(i)) != blocks.end()) // garr, block conflict
			continue;
		//assert(blocks.find(f.getBlockName(i)) == blocks.end());
		blocks[f.getBlockName(i)] = std::unique_ptr<PrayBlock>(new PrayBlock(filename.string(), f.getBlockType(i), f.getBlockName(i), f.getBlockIsCompressed(i)));
	}
}

void prayManager::update() {
	blocks.clear();

	if (!catalogue.hasTag("Pray System File Extensions")) {
		std::cout << "Warning: Catalogue tag \"Pray System File Extensions\" wasn't found, so no PRAY files will be loaded." << std::endl;
		return;
	}

	const std::vector<std::string>& extensions = catalogue.getTag("Pray System File Extensions");

	for (auto dd : world.data_directories) {
		assert(fs::exists(dd));
		assert(fs::is_directory(dd));

		fs::path praydir(dd / fs::path("My Agents/"));

		if (fs::exists(praydir) && fs::is_directory(praydir)) {
			fs::directory_iterator fsend;
			for (fs::directory_iterator d(praydir); d != fsend; ++d) {
				std::string x = d->path().extension().string();
				if (!x.empty())
					x.erase(x.begin());
				if (std::find(extensions.begin(), extensions.end(), x) != extensions.end()) {
					// TODO: language checking!
					//std::cout << "scanning PRAY file " << d->path().string() << std::endl;
					try {
						addFile(*d);
					} catch (creaturesException& e) {
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
		case 6: return "Backgrounds/"; // backgrounds
		case 7:
			return "Catalogue/"; // catalogue
		//case 8: return "Bootstrap/"; // bootstrap
		//case 9: return "My Worlds/"; // my worlds
		case 10: return "My Creatures/"; // my creatures
		case 11: return "My Agents/"; // my agents
	}

	return "";
}

/* vim: set noet: */
