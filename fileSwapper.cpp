/*
 *  fileSwapper.cpp
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

#include "fileSwapper.h"
#include <fstream>
#include "blkImage.h"
#include "c16Image.h"
#include "openc2e.h"

void fileSwapper::convertc16(std::string directory, std::string name) {
	assert(false); // unimplemented

	std::ifstream in((directory + name).c_str());
	if (!in.is_open()) return;
	std::ofstream out((directory + name + ".big").c_str());
	assert(out.is_open());
	
	// okay. read the damn file.
	c16Image img;
	img.readHeader(in);
}

void fileSwapper::converts16(std::string directory, std::string name) {
	std::ifstream in((directory + name).c_str());
	if (!in.is_open()) return;
	std::ofstream out((directory + name + ".big").c_str());
	assert(out.is_open());
	
	// okay. read the damn file.
	s16Image img;
	img.readHeader(in);

	img.buffers = new void *[img.m_numframes];
	
	for (unsigned int i = 0; i < img.m_numframes; i++) {
		img.buffers[i] = new unsigned short[img.widths[i] * img.heights[i]];
		in.seekg(img.offsets[i], std::ios::beg);
		in.read((char *)img.buffers[i], (img.widths[i] * img.heights[i] * 2));
		for (unsigned int k = 0; k < (unsigned int) (img.widths[i] * img.heights[i]); k++) {
			((unsigned short *)img.buffers[i])[k] = swapEndianShort(((unsigned short *)img.buffers[i])[k]);
		}
	}

	// okay, now we get the exciting bit, we get to write it to disk!
	// step one: calculate offsets
	unsigned int offset = 6 + (8 * img.m_numframes);
	for (unsigned int i = 0; i < img.m_numframes; i++) {
		img.offsets[i] = offset;
		offset += img.widths[i] * img.heights[i] * 2;
	}
	
	// step two: write data
	img.writeHeader(out);
	for (unsigned int i = 0; i < img.m_numframes; i++) {
		out.write((char *)img.buffers[i], img.widths[i] * img.heights[i] * 2);
	}
	
	for (unsigned int i = 0; i < img.m_numframes; i++) {
		delete (uint16 *)img.buffers[i];
	}
}

void fileSwapper::convertblk(std::string directory, std::string name) {
	std::ifstream in((directory + name).c_str());
	if (!in.is_open()) return;
	std::ofstream out((directory + name + ".big").c_str());
	assert(out.is_open());

	// okay. read the damn file.
	blkImage img;
	img.readHeader(in);
	
	img.buffers = new void *[img.m_numframes];
	
	for (unsigned int i = 0; i < img.m_numframes; i++) {
		in.seekg(img.offsets[i], std::ios::beg);
		img.buffers[i] = new uint16[128 * 128];
		in.read((char *)img.buffers[i], 128 * 128 * 2);
		for (unsigned int k = 0; k < 128 * 128; k++) {
			((unsigned short *)img.buffers[i])[k] = swapEndianShort(((unsigned short *)img.buffers[i])[k]);
		}
	}
	
	// okay, now we get the exciting bit, we get to write it to disk!
	// step one: calculate offsets
	unsigned int offset = 10 + (8 * img.m_numframes);
	for (unsigned int i = 0; i < img.m_numframes; i++) {
		img.offsets[i] = offset;
		offset += (128 * 128 * 2);
	}
	
	// step two: write data
	img.writeHeader(out);
	for (unsigned int i = 0; i < img.m_numframes; i++) {
		out.write((char *)img.buffers[i], 128 * 128 * 2);
	}

	for (unsigned int i = 0; i < img.m_numframes; i++) {
		delete (uint16 *)img.buffers[i];
	}
}
/* vim: set noet: */
