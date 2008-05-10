/*
 *  streamutils.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Thu Mar 10 2005.
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

#include "endianlove.h"
#include <fstream>

uint16 read16(std::istream &s, bool littleend = true);
void write16(std::ostream &s, uint16 v, bool littleend = true);
uint32 read32(std::istream &s);
void write32(std::ostream &s, uint32 v);

/* vim: set noet: */
