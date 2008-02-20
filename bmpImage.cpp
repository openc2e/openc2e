/*
 *  bmpImage.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed Feb 20 2008.
 *  Copyright (c) 2008 Alyssa Milburn. All rights reserved.
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

#include "bmpImage.h"
#include "endianlove.h"
#include "streamutils.h"
#include "exceptions.h"
#include "openc2e.h"

#include <iostream> // TODO: remove me

#define BI_RGB 0
#define BI_RLE8 1
#define BI_BITFIELDS 3

bmpImage::bmpImage(mmapifstream *in, std::string n) : creaturesImage(n) {
	char magic[2];
	in->read(magic, 2);
	if (std::string(magic, 2) != "BM")
		throw creaturesException(n + " doesn't seem to be a BMP file.");

	in->seekg(8, std::ios::cur); // skip filesize and reserved bytes

	uint32 dataoffset = read32(*in);

	uint32 biSize = read32(*in);
	if (biSize != 40) // win3.x format, which the seamonkeys files are in
		throw creaturesException(n + " is a BMP format we don't understand.");

	biWidth = read32(*in);
	biHeight = read32(*in);
	
	uint16 biPlanes = read16(*in);
	if (biPlanes != 1) // single image plane
		throw creaturesException(n + " contains BMP data we don't understand.");
	
	uint16 biBitCount = read16(*in);
	uint32 biCompression = read32(*in);

	// and now for some stuff we really don't care about
	uint32 biSizeImage = read32(*in);
	uint32 biXPelsPerMeter = read32(*in);
	uint32 biYPelsPerMeter = read32(*in);
	uint32 biClrUsed = read32(*in);
	uint32 biClrImportant = read32(*in);

	switch (biCompression) {
		case BI_RGB:
		case BI_BITFIELDS:
			break;

		case BI_RLE8:
			std::cout << "Warning: sprite " << n << " is compressed." << std::endl;
			break;

		default:
			throw creaturesException(n + " contains BMP data compressed in a way we don't understand.");
	}

	switch (biBitCount)  {
		case 8:
			imgformat = if_paletted;
			break;
			
		case 24:
			imgformat = if_24bit;
			break;

		default:
			throw creaturesException(n + " contains BMP data of an unsupported bit depth.");
	}

	is_mutable = false;

	bmpdata = in->map + dataoffset;
	buffers = 0;
	setBlockSize(biWidth, biHeight);
}

bmpImage::~bmpImage() {
	freeData();
}

void bmpImage::freeData() {
	for (unsigned int i = 0; i < m_numframes; i++) {
		delete[] (char *)buffers[i];
	}

	delete[] widths;
	delete[] heights;
	delete[] buffers;
}

void bmpImage::setBlockSize(unsigned int blockwidth, unsigned int blockheight) {
	if (buffers) freeData();

	unsigned int widthinblocks = biWidth / blockwidth;
	caos_assert(widthinblocks * blockwidth == biWidth);
	unsigned int heightinblocks = biHeight / blockheight;
	caos_assert(heightinblocks * blockheight == biHeight);

	m_numframes = widthinblocks * heightinblocks;
	widths = new unsigned short[m_numframes];
	heights = new unsigned short[m_numframes];
	buffers = new void *[m_numframes];

	unsigned int curr_row = 0, curr_col = 0;
	for (unsigned int i = 0; i < m_numframes; i++) {
		widths[i] = blockwidth;
		heights[i] = blockheight;

		unsigned int buffersize = blockwidth * blockheight;
		if (imgformat == if_24bit) { buffersize *= 3; }
		
		buffers[i] = new char *[buffersize];

		for (unsigned int j = 0; j < blockheight; j++) {
			unsigned int srcoffset = ((biHeight - 1) * biWidth) - (biWidth * blockheight * curr_row) - (biWidth * j) + (curr_col * blockwidth);
			unsigned int destoffset = blockwidth * j;
			unsigned int datasize = blockwidth;

			if (imgformat == if_24bit) { srcoffset *= 3; destoffset *= 3; datasize *= 3; }
			
			memcpy((char *)buffers[i] + destoffset, (char *)bmpdata + srcoffset, datasize);
		}

		curr_col++;
		if (curr_col == widthinblocks) {
			curr_col = 0;
			curr_row++;
		}
	}
}

/* vim: set noet: */
