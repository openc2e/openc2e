/*
 *  mmapifstream.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Jul 24 2004.
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

#include "mmapifstream.h"
#include "openc2e.h"
#include <cassert>
#ifdef _WIN32
#include <windows.h>
#else // assume POSIX
#include <sys/types.h>
#include <sys/mman.h>
#endif

mmapifstream::mmapifstream() {}

mmapifstream::mmapifstream(std::string filename) {

#ifdef _WIN32
	// todo: store the handle somewhere?
	HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);
	LARGE_INTEGER lifilesize;
	if (!GetFileSizeEx(hFile, &lifilesize)) {
		setstate(failbit);
		return;
	};
	filesize = lifilesize.QuadPart;
	HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	void *mapr = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
#else
	FILE *f = fopen(filename.c_str(), "r");
	assert(f);
	if (!f) {
		fclose(f);
		setstate(failbit);
		return;
	}

	// now do the mmap (work out filesize, then mmap)
	int fno = fileno(f);
	fseek(f, 0, SEEK_END);
	filesize = ftell(f);
	assert((int)filesize != -1);

	void *mapr = mmap(0, filesize, PROT_READ, MAP_PRIVATE, fno, 0);
	fclose(f); // we don't need it, now!
#endif

	assert(mapr != (void *)-1);
	map = (char *)mapr;

	buf = spanstreambuf(map, filesize);
}

mmapifstream::~mmapifstream() {
	if (map) {
#ifdef _WIN32
		UnmapViewOfFile(map);
#else
		munmap(map, filesize);
#endif
	}
}
/* vim: set noet: */
