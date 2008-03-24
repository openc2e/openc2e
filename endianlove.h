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

#ifndef _ENDIANLOVE_H
#define _ENDIANLOVE_H

#include <stdlib.h> // load the standard libraries for these defines

#if HAVE_STDINT_H

#include <stdint.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

#else

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

#endif

#ifndef OC2E_BIG_ENDIAN
#	ifdef __GNU__
#		include <endian.h>
#	endif
#	if __BYTE_ORDER == __LITTLE_ENDIAN || defined(_MSC_VER) || defined(__i386__)
#		define OC2E_BIG_ENDIAN 0
#	else
#		define OC2E_BIG_ENDIAN 1
#	endif
#endif

#if OC2E_BIG_ENDIAN

# if HAVE_BYTESWAP_H

#include <byteswap.h>
static inline uint16 swapEndianShort(uint16 a) {
	return bswap_16(a);
}

static inline uint32 swapEndianLong(uint32 a) {
	return bswap_32(a);
}

# else // HAVE_BYTESWAP_H

static inline uint16 swapEndianShort(uint16 a) {
	return ((((uint16)(a) & 0xff00) >> 8) |
				   (((uint16)(a) & 0x00ff) << 8));
}

static inline uint32 swapEndianLong(uint32 a) {
	return ((((uint32)(a) & 0xff000000) >> 24) |
				   (((uint32)(a) & 0x00ff0000) >> 8)  |
				   (((uint32)(a) & 0x0000ff00) << 8)  |
				   (((uint32)(a) & 0x000000ff) << 24));
}

# endif

#else // OC2E_BIG_ENDIAN

static inline uint16 swapEndianShort(uint16 a) {
	return a;
}

static inline uint32 swapEndianLong(uint32 a) {
	return a;
}

#endif

#endif // _ENDIANLOVE_H

/* vim: set noet: */
