/*
 *  cobFile.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri Jan 18 2008.
 *  Copyright (c) 2008 Alyssa Milburn. All rights reserved.
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

#ifndef COBFILE_H
#define COBFILE_H

#include <vector>
#include <string>
#include <fstream>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

class cobBlock;

class cobFile {
protected:
	fs::path path;
	std::ifstream file;

public:
	std::vector<cobBlock *> blocks;

	cobFile(fs::path filepath);
	~cobFile();
	fs::path getPath() { return path; }
	std::istream &getStream() { return file; }
};

class cobBlock {
protected:
	bool loaded;
	cobFile *parent;
	unsigned char *buffer;

	std::streampos offset;
	unsigned int size;

public:
	cobBlock(cobFile *p);
	~cobBlock();
	void load();
	void free();

	std::string type;

	bool isLoaded() { return loaded; }
	cobFile *getParent() { return parent; }
	std::streampos getOffset() { return offset; }
	std::string getType() { return type; }
	unsigned char *getBuffer() { assert(loaded); return buffer; }
	unsigned int getSize() { return size; }
};

class cobAgentBlock {
protected:
	cobBlock *parent;

public:
	cobAgentBlock(cobBlock *p);
	~cobAgentBlock();

	cobBlock *getParent() { return parent; }

	unsigned short quantityremaining;
	unsigned int lastusage;
	unsigned int reuseinterval;
	unsigned char usebyday;
	unsigned char usebymonth;
	unsigned short usebyyear;

	std::string name;
	std::string description;
	std::string installscript;
	std::string removescript;
	std::vector<std::string> scripts;

	std::vector<unsigned short> deptypes;
	std::vector<std::string> depnames;

	unsigned short thumbnailwidth;
	unsigned short thumbnailheight;
	unsigned short *thumbnail;
};

class cobFileBlock {
protected:
	cobBlock *parent;

public:
	cobFileBlock(cobBlock *p);
	~cobFileBlock();
	
	cobBlock *getParent() { return parent; }

	unsigned short filetype;
	unsigned int filesize;
	std::string filename;
	unsigned char *getFileContents();
};

#endif
/* vim: set noet: */
