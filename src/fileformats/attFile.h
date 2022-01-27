/*
 *  attFile.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri 25 Feb 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
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

#include <array>
#include <iosfwd>
#include <vector>

class attFile {
  public:
	std::array<std::array<unsigned int, 12>, 16> attachments = {};
	std::array<unsigned int, 16> noattachments = {};
	unsigned int nolines = 0;
	std::vector<uint8_t> extra_data;
};

attFile ReadAttFile(std::istream& in);
