/*
 *  prayManager.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jan 22 2006.
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

#ifndef PRAYMANAGER_H
#define PRAYMANAGER_H

#include "fileformats/PrayFileReader.h"

#include <cassert>
#include <ghc/filesystem.hpp>
#include <map>
#include <string>

class PrayBlock {
  protected:
	bool tagsloaded;
	ghc::filesystem::path filename;

  public:
	PrayBlock();
	PrayBlock(const ghc::filesystem::path& filename, PrayBlockMetadata);
	PrayBlock(const PrayBlock&) = delete;
	PrayBlock(PrayBlock&&) = default;
	PrayBlock& operator=(const PrayBlock&) = delete;
	PrayBlock& operator=(PrayBlock&&) = default;
	~PrayBlock();
	void parseTags();

	PrayBlockMetadata metadata;
	std::map<std::string, std::string> stringValues;
	std::map<std::string, uint32_t> integerValues;

	bool isCompressed() const;
	bool isLoaded() const;
	std::vector<uint8_t> getBuffer();
};

class prayManager {
  protected:
	void addFile(const ghc::filesystem::path&);

  public:
	std::map<std::string, PrayBlock> blocks;

	~prayManager();

	void update();
};

#endif
/* vim: set noet: */
