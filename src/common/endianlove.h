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

#pragma once

#include <iostream>
#include <stdint.h>
#include <string.h>
#ifdef _WIN32
#include <stdlib.h>
#endif

#include "common/NumericCast.h"

/*

This file used to handle integer encodings in a platform-neutral way using byte
shifts and ors, in the same vein as Rob Pike's article "The Byte Order Fallacy":

e.g. a little-endian uint32 would be read as (buf[0] << 0) | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);

Unfortunately, this is not optimized out on all platforms (*cough*MSVC*cough*),
and even when it is optimized for the singular case it does not optimize well
for the "readmany" case.

So, we do platform endianness detection and byte swapping.

*/

static inline bool is_little_endian() {
	// gets optimized to a constant, inlined, and used to eliminate dead code on
	// mainstream compilers
	const uint32_t value = 0x04030201;
	return *static_cast<const uint8_t*>(static_cast<const void*>(&value)) == 0x01;
}

static inline uint16_t byte_swap_16(uint16_t val) {
#ifdef _WIN32
	return _byteswap_ushort(val);
#else
	// this is optimized down to a rotation on Clang and GCC
	return static_cast<uint16_t>(((val & 0xff00) >> 8) | ((val & 0xff) << 8));
#endif
}

static inline uint32_t byte_swap_32(uint32_t val) {
	// this is optimized down to a bswap on Clang, GCC, and MSVC
	return static_cast<uint32_t>(((val & 0xff000000) >> 24) | ((val & 0xff0000) >> 8) | ((val & 0xff00) << 8) | ((val & 0xff) << 24));
}

static inline uint8_t read8(std::istream& s) {
	uint8_t t;
	s.read(reinterpret_cast<char*>(&t), 1);
	return t;
}

static inline int8_t readsigned8(std::istream& s) {
	// assumes two's complement
	uint8_t t;
	s.read(reinterpret_cast<char*>(&t), 1);
	return static_cast<int8_t>(t);
}

static inline void write8(std::ostream& s, uint8_t v) {
	s.write(reinterpret_cast<char*>(&v), 1);
}

static inline void writesigned8(std::ostream& s, int8_t v_) {
	// assumes two's complement
	uint8_t v = static_cast<uint8_t>(v_);
	s.write(reinterpret_cast<char*>(&v), 1);
}

static inline uint16_t read16le(const uint8_t* buf) {
	uint16_t val;
	memcpy(&val, buf, 2);
	return is_little_endian() ? val : byte_swap_16(val);
}

static inline uint16_t read16le(std::istream& s) {
	uint16_t val;
	s.read(reinterpret_cast<char*>(&val), 2);
	return is_little_endian() ? val : byte_swap_16(val);
}

static inline int16_t readsigned16le(std::istream& s) {
	// assumes two's complement
	uint16_t val;
	s.read(reinterpret_cast<char*>(&val), 2);
	val = is_little_endian() ? val : byte_swap_16(val);
	return static_cast<int16_t>(val);
}

static inline void write16le(std::ostream& s, uint16_t v) {
	uint16_t t = is_little_endian() ? v : byte_swap_16(v);
	s.write(reinterpret_cast<char*>(&t), 2);
}

static inline void writesigned16le(std::ostream& s, int16_t v_) {
	// assumes two's complement
	uint16_t v = static_cast<uint16_t>(v_);
	uint16_t t = is_little_endian() ? v : byte_swap_16(v);
	s.write(reinterpret_cast<char*>(&t), 2);
}

static inline void readmany16le(std::istream& s, uint16_t* out, size_t n) {
	// this needs to be fast! it's used in the sprite file reading functions
	s.read(reinterpret_cast<char*>(out), numeric_cast<std::streamsize>(n * 2));
	if (!is_little_endian()) {
		for (size_t i = 0; i < n; ++i) {
			out[i] = byte_swap_16(out[i]);
		}
	}
}

static inline uint32_t read32le(std::istream& s) {
	uint32_t t;
	s.read(reinterpret_cast<char*>(&t), 4);
	return is_little_endian() ? t : byte_swap_32(t);
}

static inline int32_t readsigned32le(std::istream& s) {
	// assumes two's complement
	uint32_t t;
	s.read(reinterpret_cast<char*>(&t), 4);
	t = is_little_endian() ? t : byte_swap_32(t);
	return static_cast<int32_t>(t);
}

static inline void write32le(std::ostream& s, uint32_t v) {
	uint32_t t = is_little_endian() ? v : byte_swap_32(v);
	s.write(reinterpret_cast<char*>(&t), 4);
}

static inline void writesigned32le(std::ostream& s, int32_t v_) {
	// assumes two's complement
	uint32_t v = static_cast<uint32_t>(v_);
	uint32_t t = is_little_endian() ? v : byte_swap_32(v);
	s.write(reinterpret_cast<char*>(&t), 4);
}

static inline void write32le(uint8_t* b, uint32_t v) {
	uint32_t t = is_little_endian() ? v : byte_swap_32(v);
	memcpy(b, &t, 4);
}

// big-endian integers are used in two places: c2e genome files (yes, for some
// crazy reason!), and macOS versions of spritefiles (.m16, .n16, and .blk)

static inline uint16_t read16be(std::istream& s) {
	uint16_t t;
	s.read(reinterpret_cast<char*>(&t), 2);
	return is_little_endian() ? byte_swap_16(t) : t;
}

static inline void write16be(std::ostream& s, uint16_t v) {
	uint16_t t = is_little_endian() ? byte_swap_16(v) : v;
	s.write(reinterpret_cast<char*>(&t), 2);
}

static inline void readmany16be(std::istream& s, uint16_t* out, size_t n) {
	s.read(reinterpret_cast<char*>(out), numeric_cast<std::streamsize>(n * 2));
	if (is_little_endian()) {
		for (size_t i = 0; i < n; ++i) {
			// gets optimized into fast SIMD instructions on Clang and MSVC at
			// -O2, and on GCC at -O3
			out[i] = byte_swap_16(out[i]);
		}
	}
}

static inline uint32_t read32be(std::istream& s) {
	uint32_t t;
	s.read(reinterpret_cast<char*>(&t), 4);
	return is_little_endian() ? byte_swap_32(t) : t;
}

static inline void write32be(std::ostream& s, uint32_t v) {
	uint32_t t = is_little_endian() ? byte_swap_32(v) : v;
	s.write(reinterpret_cast<char*>(&t), 4);
}