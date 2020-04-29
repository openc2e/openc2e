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

#include <cassert>
#include <ghc/filesystem.hpp>

#include <map>
#include <string>

namespace fs = ghc::filesystem;

class PrayBlock {
protected:
	bool loaded;
	bool tagsloaded;
	std::vector<unsigned char> buffer;

	std::streampos offset;
	bool compressed;
	unsigned int size, compressedsize;

	std::string filename;

public:
	PrayBlock();
	PrayBlock(const std::string& filename, const std::string& type, const std::string& name, bool is_compressed);
	~PrayBlock();
	void load();
	void parseTags();

	std::string type;
	std::string name;
	std::map<std::string, std::string> stringValues;
	std::map<std::string, uint32_t> integerValues;

	bool isCompressed() { return compressed; }
	bool isLoaded() { return loaded; }
	unsigned char *getBuffer() { assert(loaded); return buffer.data(); }
	unsigned int getSize() { assert(loaded); return size; }
};

class prayManager {
protected:
	void addFile(const fs::path&);

public:
	std::map<std::string, std::unique_ptr<PrayBlock> > blocks;

	~prayManager();

	void update();

	static std::string getResourceDir(unsigned int id);
};

#endif
/* vim: set noet: */
