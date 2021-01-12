/*
 *  mngfile.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue 16 Nov 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
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

#include "creaturesException.h"
#include "shared_array.h"

#include <cmath>
#include <fmt/core.h>
#include <istream>
#include <list>
#include <map>
#include <string>
#include <vector>

class MNGFileException : public creaturesException {
  public:
	int lineno;
	MNGFileException(const char* m) throw()
		: creaturesException(m) { lineno = 0; }
	MNGFileException(const std::string& m) throw()
		: creaturesException(m) { lineno = 0; }
	MNGFileException(const char* m, int l) throw()
		: creaturesException(m) { lineno = l; }
	MNGFileException(const std::string& m, int l) throw()
		: creaturesException(m) { lineno = l; }
};

class MNGFile {
  private:
	class mmapifstream* stream;

  public:
	MNGFile(std::string);
	~MNGFile();
	std::vector<std::string> getSampleNames() const;
	unsigned int getSampleForName(std::string name);
	std::string name;
	std::string script;
	std::map<std::string, unsigned int> samplemappings;
	std::vector<shared_array<uint8_t>> samples;

	std::string dump();
};
