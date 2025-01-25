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
#include "common/io/FileReader.h"
#include "fileformats/PrayFileReader.h"

#include <cassert>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

PrayBlock::PrayBlock() {
}

PrayBlock::PrayBlock(const fs::path& filename_, PrayBlockMetadata metadata_)
	: tagsloaded(false), filename(filename_), metadata(metadata_) {
}

PrayBlock::~PrayBlock() {
}

void PrayBlock::parseTags() {
	if (tagsloaded) {
		return;
	}

	FileReader in(filename);
	auto tags = readPrayBlockTags(in, metadata);
	integerValues = tags.first;
	stringValues = tags.second;
}

bool PrayBlock::isCompressed() const {
	return metadata.is_compressed;
}

bool PrayBlock::isLoaded() const {
	return tagsloaded;
}

std::vector<uint8_t> PrayBlock::getBuffer() {
	FileReader in(filename);
	return readPrayBlockRawData(in, metadata);
}


prayManager::~prayManager() {
}

void prayManager::addFile(const fs::path& filename) {
	FileReader in(filename);

	auto metadata = readPrayMetadata(in);

	for (auto& m : metadata) {
		if (blocks.find(m.name) != blocks.end()) // garr, block conflict
			continue;
		//assert(blocks.find(m.name) == blocks.end());
		blocks[m.name] = PrayBlock(filename, m);
	}
}

void prayManager::update() {
	blocks.clear();

	if (!catalogue.hasTag("Pray System File Extensions")) {
		fmt::print("Warning: Catalogue tag \"Pray System File Extensions\" wasn't found, so no PRAY files will be loaded.\n");
		return;
	}

	const std::vector<std::string>& extensions = catalogue.getTag("Pray System File Extensions");

	for (auto ext : extensions) {
		for (auto f : findAgentFiles("*." + ext)) {
			// TODO: language checking!
			//fmt::print("scanning PRAY file {}\n", d->path().string());
			try {
				addFile(f);
			} catch (Exception& e) {
				fmt::print(stderr, "PRAY file \"{}\" failed to load: {}\n", f.string(), e.what());
			}
		}
	}
}

/* vim: set noet: */
