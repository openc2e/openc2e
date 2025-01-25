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

#include "common/io/Reader.h"

#include <map>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

using PrayTagBlock = std::pair<std::map<std::string, uint32_t>, std::map<std::string, std::string>>;

struct PrayBlockMetadata {
	std::string type;
	std::string name;
	size_t offset;
	bool is_compressed;
};

class PrayFileReader {
  protected:
	Reader& stream;
	std::vector<PrayBlockMetadata> blocks;

  public:
	PrayFileReader(Reader&);
	~PrayFileReader();

	size_t getNumBlocks();
	std::string getBlockType(size_t index);
	std::string getBlockName(size_t index);
	bool getBlockIsCompressed(size_t index);
	std::vector<uint8_t> getBlockRawData(size_t index);
	PrayTagBlock getBlockTags(size_t i);
};

std::vector<PrayBlockMetadata> readPrayMetadata(Reader&);
std::vector<uint8_t> readPrayBlockRawData(Reader&, const PrayBlockMetadata&);
PrayTagBlock readPrayBlockTags(Reader&, const PrayBlockMetadata&);