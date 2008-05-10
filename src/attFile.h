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
#include <fstream>

class attFile {
public:
	unsigned int attachments[16][20];
	unsigned int noattachments[16];
	unsigned int nolines;
	friend std::istream &operator >> (std::istream &, attFile &);
};

/* vim: set noet: */
