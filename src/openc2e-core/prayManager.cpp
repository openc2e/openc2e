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
#include "PathResolver.h"
#include "common/Exception.h"
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
		throw Exception(std::string("couldn't open PRAY file \"") + filename + "\"");
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

	throw Exception("Couldn't load " + type + " block '" + name + "' from file '" + filename + "'");
}

void PrayBlock::parseTags() {
	load(); // TODO: don't double read

	if (tagsloaded) {
		return;
	}

	std::ifstream in(filename, std::ios::binary);
	if (!in)
		throw Exception(std::string("couldn't open PRAY file \"") + filename + "\"");
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

	throw Exception("Couldn't load " + type + " block '" + name + "' from file '" + filename + "'");
}


prayManager::~prayManager() {
}

void prayManager::addFile(const fs::path& filename) {
	std::ifstream in(filename.string(), std::ios::binary);
	if (!in)
		throw Exception(std::string("couldn't open PRAY file \"") + filename.string() + "\"");
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

	for (auto ext : extensions) {
		for (auto f : findAgentFiles("*." + ext)) {
			// TODO: language checking!
			//std::cout << "scanning PRAY file " << d->path().string() << std::endl;
			try {
				addFile(f);
			} catch (Exception& e) {
				std::cerr << "PRAY file \"" << f << "\" failed to load: " << e.what() << std::endl;
			}
		}
	}
}

/* vim: set noet: */
