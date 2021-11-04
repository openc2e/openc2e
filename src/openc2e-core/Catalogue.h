/*
 *  Catalogue.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat 13 Nov 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
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

#include "common/Exception.h"

#include <ghc/filesystem.hpp>
#include <iostream>
#include <istream>
#include <map>
#include <string>
#include <vector>

class Catalogue {
  public:
	std::map<std::string, std::vector<std::string> > data;

	const std::vector<std::string>& getTag(std::string t) const { return (*data.find(t)).second; }
	bool hasTag(std::string t) const { return (data.find(t) != data.end()); }
	const std::string getAgentName(unsigned char family, unsigned char genus, unsigned short species) const;
	std::string calculateWildcardTag(std::string tag, unsigned char family, unsigned char genus, unsigned short species) const;

	void reset();
	void addFile(ghc::filesystem::path path);
	void initFrom(ghc::filesystem::path path, std::string language);

  private:
	void addVals(const std::string& title, bool override, const std::vector<std::string>& vals);
};

extern Catalogue catalogue;
