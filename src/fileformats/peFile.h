/*
 *  peFile.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon Apr 28 2008.
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

#include "common/Image.h"
#include "common/optional.h"

#include <fstream>
#include <ghc/filesystem.hpp>
#include <map>
#include <string>
#include <vector>

namespace fs = ghc::filesystem;

enum PeResourceType {
	PE_RESOURCETYPE_CURSOR = 1,
	PE_RESOURCETYPE_BITMAP = 2,
	PE_RESOURCETYPE_ICON = 3,
	PE_RESOURCETYPE_MENU = 4,
	PE_RESOURCETYPE_DIALOG = 5,
	PE_RESOURCETYPE_STRING = 6,
	PE_RESOURCETYPE_ACCELERATOR = 9,
	PE_RESOURCETYPE_GROUP_CURSOR = 12,
	PE_RESOURCETYPE_GROUP_ICON = 14,
	PE_RESOURCETYPE_VERSION = 16,
	PE_RESOURCETYPE_TOOLBAR = 241,
};

enum PeLanguage {
	PE_LANGUAGE_NEUTRAL = 0x00,
	PE_LANGUAGE_GERMAN = 0x07,
	PE_LANGUAGE_ENGLISH = 0x09,
	PE_LANGUAGE_FRENCH = 0x0c,
	PE_LANGUAGE_ITALIAN = 0x10,
	PE_LANGUAGE_DUTCH = 0x13,
	PE_LANGUAGE_JAPANESE = 0x11, // sometimes used for english?
	PE_LANGUAGE_SPANISH = 0x0a,
};

enum PeSubLanguage {
	PE_SUBLANG_NEUTRAL = 0x00,
	PE_SUBLANG_DEFAULT = 0x01,
	PE_SUBLANG_GERMAN = 0x01,
	PE_SUBLANG_ENGLISH_US = 0x01,
	PE_SUBLANG_ENGLISH_UK = 0x02,
	PE_SUBLANG_FRENCH = 0x01,
	PE_SUBLANG_ITALIAN = 0x01,
	PE_SUBLANG_DUTCH = 0x01,
	PE_SUBLANG_SPANISH = 0x01,
	PE_SUBLANG_SPANISH_MODERN = 0x03,
	PE_SUBLANG_JAPANESE = 0x01,
};

class resourceInfo {
  public:
	PeResourceType type;
	PeLanguage lang;
	PeSubLanguage sublang;
	uint32_t name;
	uint32_t offset;
	uint32_t size;
};

class peFile {
  protected:
	struct peSection {
		uint32_t vaddr;
		uint32_t offset;
		uint32_t size;
	};

	fs::path path;
	std::ifstream file;
	unsigned int currtype, currname, currlang, currsublang;

	void parseResourcesLevel(peSection& s, unsigned int off, unsigned int level);
	shared_array<uint8_t> getResourceData(resourceInfo);

  public:
	peFile(fs::path filepath);
	~peFile();

	optional<resourceInfo> findResource(PeResourceType type, PeLanguage lang, uint32_t name);
	optional<resourceInfo> findResource(PeResourceType type, PeLanguage lang, PeSubLanguage sublang, uint32_t name);
	std::vector<std::string> getResourceStrings(resourceInfo);
	Image getBitmap(uint32_t name);

	std::vector<resourceInfo> resources;

	static std::string resource_type_to_string(PeResourceType);
	static std::string language_to_string(PeLanguage, PeSubLanguage);
};
