/*
 *  c2cobfile.h
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

#pragma once

#include "Image.h"

#include <cassert>
#include <fstream>
#include <string>
#include <vector>

class cobBlock;

class c2cobfile {
  protected:
	std::string path;
	std::ifstream file;

  public:
	std::vector<cobBlock*> blocks;

	c2cobfile(std::string filepath);
	~c2cobfile();
	std::string getPath() { return path; }
	std::istream& getStream() { return file; }
};

class cobBlock {
  protected:
	bool loaded;
	c2cobfile* parent;
	unsigned char* buffer;

	std::streampos offset;
	unsigned int size;

  public:
	cobBlock(c2cobfile* p);
	~cobBlock();
	void load();
	void free();

	std::string type;

	bool isLoaded() { return loaded; }
	c2cobfile* getParent() { return parent; }
	std::streampos getOffset() { return offset; }
	std::string getType() { return type; }
	unsigned char* getBuffer() {
		assert(loaded);
		return buffer;
	}
	unsigned int getSize() { return size; }
};

class cobAgentBlock {
  protected:
	cobBlock* parent;

  public:
	cobAgentBlock(cobBlock* p);
	~cobAgentBlock();

	cobBlock* getParent() { return parent; }

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

	Image thumbnail;
};

class cobFileBlock {
  protected:
	cobBlock* parent;

  public:
	cobFileBlock(cobBlock* p);
	~cobFileBlock();

	cobBlock* getParent() { return parent; }

	unsigned short filetype;
	unsigned int filesize;
	std::string filename;
	unsigned char* getFileContents();
};

class cobAuthBlock {
  protected:
	cobBlock* parent;

  public:
	cobAuthBlock(cobBlock* p);
	~cobAuthBlock();

	uint8_t daycreated;
	uint8_t monthcreated;
	uint16_t yearcreated;
	uint8_t version;
	uint8_t revision;
	std::string authorname;
	std::string authoremail;
	std::string authorurl;
	std::string authorcomments;
};

/* vim: set noet: */
