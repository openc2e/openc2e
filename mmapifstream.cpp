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
#include <sys/types.h>
#include <sys/mman.h>

mmapifstream::mmapifstream(std::string filename) {
	mmapopen(filename);
}

void mmapifstream::mmapopen(std::string filename) {
	open(filename.c_str());
	if (!is_open()) return;
	// todo: store the FILE* somewhere?
	FILE *f = fopen(filename.c_str(), "r");
	if (!f) {
		close();
		setstate(failbit);
		perror("fopen");
		return;
	}

	// now do the mmap (work out filesize, then mmap)
	int fno = fileno(f);
	seekg(0, std::ios::end);
	filesize = tellg();
	seekg(0, std::ios::beg);
	
	void *mapr = mmap(0, filesize, PROT_READ, MAP_PRIVATE, fno, 0);
	assert(mapr != (void *)-1);
	map = (char *)mapr;	
}

mmapifstream::~mmapifstream() {
	munmap(map, filesize);
}
