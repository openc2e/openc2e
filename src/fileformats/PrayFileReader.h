/*
 *  PrayFileReader.h
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

#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

class prayFileBlock;

class PrayFileReader {
  protected:
	std::istream& stream;
	std::vector<size_t> block_offsets;

  public:
	PrayFileReader(std::istream& stream);
	~PrayFileReader();

	size_t getNumBlocks();
	std::string getBlockType(size_t index);
	std::string getBlockName(size_t index);
	bool getBlockIsCompressed(size_t index);
	std::vector<unsigned char> getBlockRawData(size_t index);
	std::pair<std::map<std::string, uint32_t>, std::map<std::string, std::string>> getBlockTags(size_t i);
};
