/*
 *  streamutils.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat 13 Nov 2004.
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
#include "endianlove.h"
#include <fstream> // don't actually need this

#ifdef _MSC_VER
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

uint16 read16(std::istream &s, bool littleend) {
	uint16 t;
	s.read((char *)&t, 2);
	if (littleend)
		return swapEndianShort(t);
	else
		return ntohs(t);
}

void write16(std::ostream &s, uint16 v, bool littleend) {
	uint16 t;
	if (littleend)
		t = swapEndianShort(v);
	else
		t = htons(v);
	s.write((char *)&t, 2);
}

uint32 read32(std::istream &s) {
	uint32 t;
	s.read((char *)&t, 4);
	return swapEndianLong(t);
}

void write32(std::ostream &s, uint32 v) {
	uint32 t = swapEndianLong(v);
	s.write((char *)&t, 4);
}

/* vim: set noet: */
