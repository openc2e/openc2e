/*
 *  pray.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Aug 28 2001.
 *  Copyright (c) 2001,2004 Alyssa Milburn. All rights reserved.
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

#include "agentfile.h"
#include "zlib.h"
#include "endianlove.h"

#include <exception>

class badPrayMajicException { };
class decompressFailure { };

// *** prayFile

void prayFile::read(istream &s) {
	unsigned char majic[4];
	s.read((char *)majic, 4);
	if (strncmp((char *)majic, "PRAY", 4) != 0) throw badPrayMajicException();

	while (true) {
		char stringid[4];
		for (int i = 0; i < 4; i++) s >> stringid[i];

		if (s.fail() || s.eof()) return;

		prayBlock *b;

		/*
			known pray blocks:

			AGNT, DSAG : tag blocks
			PHOT, GENE, FILE : various file blocks
			GLST: CreaturesArchive file blocks

			unknown pray blocks (there are more, I don't have a copy of C3 atm =/):

			CREA (CreaturesArchive file?), DSEX (tag block?)
		*/

		// TODO: read the taggable agent blocks from a configuration file
		// note we're probably missing warp blocks, playground blocks (if they exist), sea-monkeys blocks
		bool tagblock;
		tagblock = !strncmp(stringid, "AGNT", 4); // Creatures Adventures/Creatures 3 agent
		if (!tagblock) tagblock = !strncmp(stringid, "DSAG", 4); // Docking Station agent
		if (!tagblock) tagblock = !strncmp(stringid, "LIVE", 4); // Sea-Monkeys agent
		if (!tagblock) tagblock = !strncmp(stringid, "EXPC", 4); // Creatures creature info
		if (!tagblock) tagblock = !strncmp(stringid, "DSEX", 4); // DS creature info
		if (!tagblock) tagblock = !strncmp(stringid, "SFAM", 4); // starter family info
		if (!tagblock) tagblock = !strncmp(stringid, "EGGS", 4); // eggs info
		if (!tagblock) tagblock = !strncmp(stringid, "DFAM", 4); // DS starter family info
		if (tagblock)
			b = new tagPrayBlock();
		else
			b = new unknownPrayBlock();

		memcpy(b->blockid, stringid, 4);
		s >> *b;
		blocks.push_back(b);
	}
}

void prayFile::write(ostream &s) const {
	s << "PRAY";

	for (vector<block *>::iterator x = ((prayFile *)this)->blocks.begin(); x != ((prayFile *)this)->blocks.end(); x++)
		s << **x;
}

// *** prayBlock

void prayBlock::read(istream &s) {
	unsigned char x[128];
	s.read((char *)&x, 128);

	blockname = (char *)x;

	unsigned int size, usize, flags;
	s.read((char *)&size, 4); size = swapEndianLong(size);
	s.read((char *)&usize, 4); usize = swapEndianLong(usize);
	s.read((char *)&flags, 4); flags = swapEndianLong(flags);

	unsigned char *buf;
	buf = new unsigned char[size];
	s.read((char *)buf, size);

	if ((flags & 1) != 0) {
		// decompress the block
		unsigned char *dest = new unsigned char[usize];

		if (uncompress(dest, (uLongf *)&usize, buf, size) != Z_OK) {
			delete dest; delete buf;

			throw decompressFailure();
		}

		delete buf;
		buf = dest;
	}

	rawRead(size, buf);

	delete buf;
}

void prayBlock::write(ostream &s) const {
	s.write((char *)blockid, 4);

	unsigned char x[128];
	memset(x, 0, 128);
	memcpy(x, blockname.c_str(), blockname.size());
	s.write((char *)&x, 128);

	unsigned int usize, size;
	unsigned char *b = rawWrite(usize);

	bool compressed;

#ifdef NO_COMPRESS_PRAY
	compressed = false;
	size = usize;
#else
	unsigned char *newbuf = new unsigned char[usize + 12];
	size = usize + 12;
	if (compress(newbuf, (uLongf *)&size, b, usize) != Z_OK) {
		// fallback to uncompressed data
		delete newbuf;
		size = usize;
		compressed = false;
	} else if (size >= usize) {
		// the compressed block is larger than the uncompressed block
		// fallback to uncompressed data
		delete newbuf;
		size = usize;
		compressed = false;
	} else {
		delete b; b = newbuf;
		compressed = true;
	}
#endif

	unsigned int esize = swapEndianLong(size); s.write((char *)&esize, 4);
	unsigned int eusize = swapEndianLong(usize); s.write((char *)&eusize, 4);

	unsigned int flags = swapEndianLong(compressed ? 1 : 0);
	s.write((char *)&flags, 4);

	s.write((char *)b, size);
}

// *** unknownPrayBlock

void unknownPrayBlock::rawRead(unsigned int v, unsigned char *b) {
	buf = new unsigned char[v];
	memcpy(buf, b, v);
	len = v;
}

unsigned char *unknownPrayBlock::rawWrite(unsigned int &l) const {
	unsigned char *b = new unsigned char[len];
	memcpy(b, buf, len);
	l = len;
	return b;
}

// *** tagPrayBlock

/*
  format of tag pray blocks:

  4-byte nointvalues, then the values: [string name, int value]
  4-byte nostrvalues, then the values: [string name, string value]
  (string = 4-byte length followed by actual data)
*/

char *tagStringRead(unsigned char *&ptr) {
	unsigned int len = *(unsigned int *)ptr;
	len = swapEndianLong(len);
	ptr += 4;

	// TODO: fixme: rewrite this code properly
	static char b[50001];
	memcpy(b, ptr, (len < 50000) ? len : 50000);
	b[(len < 50000) ? len : 50000] = 0;
	ptr += len;
	return b;
}

void tagStringWrite(unsigned char *&ptr, string &s) {
	*(unsigned int *)ptr = swapEndianLong(s.size()); ptr += 4;
	memcpy(ptr, s.c_str(), s.size()); ptr += s.size();
}

void tagPrayBlock::rawRead(unsigned int v, unsigned char *b) {
	unsigned char *ptr = b;

	unsigned int nointvalues = swapEndianLong(*(unsigned int *)ptr);
	ptr += 4;

	for (unsigned int i = 0; i < nointvalues; i++) {
		pair<string, unsigned int> value;

		value.first = tagStringRead(ptr);
		value.second = swapEndianLong(*(unsigned int *)ptr);
		ptr += 4;

		intvalues.push_back(value);
	}

	unsigned int nostrvalues = swapEndianLong(*(unsigned int *)ptr);
	ptr += 4;

	for (unsigned int i = 0; i < nostrvalues; i++) {
		pair<string, string> value;

		value.first = tagStringRead(ptr);
		value.second = tagStringRead(ptr);

		strvalues.push_back(value);
	}
}

unsigned char *tagPrayBlock::rawWrite(unsigned int &l) const {
	l = 8;

	for (vector<pair<string, unsigned int> >::iterator x = ((tagPrayBlock *)this)->intvalues.begin(); x != ((tagPrayBlock *)this)->intvalues.end(); x++)
		l = l + 8 + (*x).first.size();

	for (vector<pair<string, string> >::iterator x = ((tagPrayBlock *)this)->strvalues.begin(); x != ((tagPrayBlock *)this)->strvalues.end(); x++)
		l = l + 8 + (*x).first.size() + (*x).second.size();

	unsigned char *buf = new unsigned char[l];
	unsigned char *ptr = buf;

	*(unsigned int *)ptr = swapEndianLong(intvalues.size()); ptr += 4;
	for (vector<pair<string, unsigned int> >::iterator x = ((tagPrayBlock *)this)->intvalues.begin(); x != ((tagPrayBlock *)this)->intvalues.end(); x++) {
		tagStringWrite(ptr, (*x).first);
		*(unsigned int *)ptr = swapEndianLong((*x).second); ptr += 4;
	}

	*(unsigned int *)ptr = swapEndianLong(strvalues.size()); ptr += 4;
	for (vector<pair<string, string> >::iterator x = ((tagPrayBlock *)this)->strvalues.begin(); x != ((tagPrayBlock *)this)->strvalues.end(); x++) {
		tagStringWrite(ptr, (*x).first);
		tagStringWrite(ptr, (*x).second);
	}

	return buf;
}
