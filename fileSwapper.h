/*
 *  fileSwapper.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri Jul 23 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include <string>

/*
  Creatures file formats are little-endian. In order to mmap() them on big-endian,
  we need to convert them. This class is a grouping for the conversion code.
 */
class fileSwapper {
public:
	void convertc16(std::string directory, std::string name);
	void converts16(std::string directory, std::string name);
	void convertblk(std::string directory, std::string name);
};
