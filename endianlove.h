/*
 *  endianlove.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat May 22 2004.
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

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

// you must define one, and only one, of the following two. otherwise things won't work.
#define __C2E_BIGENDIAN
//#define __C2E_LITTLEENDIAN

#ifdef __C2E_LITTLEENDIAN

#define swapEndianShort(A) A 
#define swapEndianLong(A) A

#else

#define swapEndianShort(A)  ((((uint16)(A) & 0xff00) >> 8) | \
                   (((uint16)(A) & 0x00ff) << 8))
#define swapEndianLong(A)  ((((uint32)(A) & 0xff000000) >> 24) | \
                   (((uint32)(A) & 0x00ff0000) >> 8)  | \
                   (((uint32)(A) & 0x0000ff00) << 8)  | \
                   (((uint32)(A) & 0x000000ff) << 24))

#endif

#include <fstream>

uint16 read16(std::istream &s);
void write16(std::ostream &s, uint16 v);
uint32 read32(std::istream &s);
void write32(std::ostream &s, uint32 v);

