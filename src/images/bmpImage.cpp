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
#include "exceptions.h"
#include "openc2e.h"
#include <memory>

#define BI_RGB 0
#define BI_RLE8 1
#define BI_BITFIELDS 3

bmpData::bmpData(std::ifstream *in, std::string n) {
	palette = 0;
	stream = in;

	char magic[2];
	in->read(magic, 2);
	if (std::string(magic, 2) != "BM")
		throw creaturesException(n + " doesn't seem to be a BMP file.");

	in->seekg(8, std::ios::cur); // skip filesize and reserved bytes

	uint32_t dataoffset = read32le(*in);

	uint32_t biSize = read32le(*in);
	if (biSize != 40) // win3.x format, which the seamonkeys files are in
		throw creaturesException(n + " is a BMP format we don't understand.");

	biWidth = read32le(*in);
	biHeight = read32le(*in);
	caos_assert((int)biHeight > 0);
	
	uint16_t biPlanes = read16le(*in);
	if (biPlanes != 1) // single image plane
		throw creaturesException(n + " contains BMP data we don't understand.");
	
	uint16_t biBitCount = read16le(*in);
	biCompression = read32le(*in);

	// and now for some stuff we really don't care about
	uint32_t biSizeImage = read32le(*in);
	uint32_t biXPelsPerMeter = read32le(*in);
	uint32_t biYPelsPerMeter = read32le(*in);
	uint32_t biClrUsed = read32le(*in);
	uint32_t biClrImportant = read32le(*in);

	switch (biCompression) {
		case BI_RGB:
			break;

		case BI_RLE8:
			if (biBitCount != 8) throw creaturesException(n + " contains BMP data compressed in a way which isn't possible.");
			break;

		case BI_BITFIELDS:
		default:
			throw creaturesException(n + " contains BMP data compressed in a way we don't understand.");
	}

	switch (biBitCount)  {
		case 4:
		case 8:
			{
			imgformat = if_paletted;
			unsigned int num_palette_entries = (biBitCount == 4 ? 16 : 256);
			std::vector<uint8_t> filepalette(num_palette_entries * 4);
			in->read((char*)filepalette.data(), filepalette.size());
			palette = new uint8_t[256 * 4];
			for (unsigned int i = 0; i < num_palette_entries; i++) {
				palette[i * 4] = filepalette[(i * 4) + 2];
				palette[(i * 4) + 1] = filepalette[(i * 4) + 1];
				palette[(i * 4) + 2] = filepalette[i * 4];
				palette[(i * 4) + 3] = 0;
			}
			}
			break;
			
		case 24:
			imgformat = if_24bit;
			break;

		default:
			throw creaturesException(n + " contains BMP data of an unsupported bit depth.");
	}

	if (biSizeImage == 0) {
		biSizeImage = biWidth * biHeight * biBitCount / 8;
	}

	in->seekg(dataoffset);
	bmpdata = new char[biSizeImage];
	in->read((char*)bmpdata, biSizeImage);

	if (biBitCount == 4) {
		char *srcdata = (char *)bmpdata;
		char *dstdata = new char[biWidth * biHeight];
		bmpdata = dstdata;

		for (char *dest = dstdata; dest < dstdata + biWidth * biHeight; dest += biWidth) {
			uint8_t pixel = 0;
			for (unsigned int i = 0; i < biWidth; i++) {
				if (i % 2 == 0) {
					pixel = *srcdata;
					srcdata++;
				}
				*(dest + i) = (pixel >> 4);
				pixel <<= 4;
			}
		}
		delete srcdata;
	}

	if (biCompression == BI_RLE8) {
		// decode an RLE-compressed 8-bit image
		// TODO: sanity checking
		char *srcdata = (char *)bmpdata;
		char *dstdata = new char[biWidth * biHeight];
		for (unsigned int i = 0; i < biWidth * biHeight; i++) dstdata[i] = 0; // TODO
		bmpdata = dstdata;
		
		unsigned int x = 0, y = 0;
		for (unsigned int i = 0; i < biSizeImage;) {
			unsigned char nopixels = srcdata[i]; i++;
			unsigned char val = srcdata[i]; i++;
			if (nopixels == 0) { // special
				if (val == 0) { // end of line
					x = 0;
					y += 1;
				} else if (val == 1) { // end of bitmap
					break;
				} else if (val == 2) { // delta
					unsigned char horz = srcdata[i]; i++;
					unsigned char vert = srcdata[i]; i++;
					x += horz;
					y += vert;
				} else { // absolute mode
					for (unsigned int j = 0; j < val; j++) {
						if (x + (y * biWidth) >= biHeight * biWidth) break;
						dstdata[x + (y * biWidth)] = srcdata[i];
						i++; x++;
					}
					if (val % 2 == 1) i++; // skip padding byte
				}
			} else { // run of pixels
				for (unsigned int j = 0; j < nopixels; j++) {
					if (x + (y * biWidth) >= biHeight * biWidth) break;
					dstdata[x + (y * biWidth)] = val;
					x++;
				}
			}
			
			while (x > biWidth) {
				x -= biWidth; y++;
			}

			if (x + (y * biWidth) >= biHeight * biWidth) break;
		}
		delete srcdata;
	}

}

bmpData::~bmpData() {
	delete[] (char *)bmpdata;
	if (palette) delete[] palette;
	delete stream;
}

bmpImage::bmpImage(std::ifstream *in, std::string n) : creaturesImage(n) {
	buffers = 0;

	bmpdata = shared_ptr<bmpData>(new bmpData(in, n));
	imgformat = bmpdata->imgformat;
	setBlockSize(bmpdata->biWidth, bmpdata->biHeight);
}

bmpImage::~bmpImage() {
	if (buffers) freeData();
}

void bmpImage::freeData() {
	for (unsigned int i = 0; i < m_numframes; i++) {
		delete[] (char *)buffers[i];
	}

	delete[] widths;
	delete[] heights;
	delete[] buffers;

	buffers = 0;
}

void bmpImage::setBlockSize(unsigned int blockwidth, unsigned int blockheight) {
	if (buffers) freeData();
	m_numframes = 0;

	// Note that the blockwidth/height isn't always a multiple of the image width/height, there can be useless pixels.
	unsigned int widthinblocks = bmpdata->biWidth / blockwidth;
	unsigned int heightinblocks = bmpdata->biHeight / blockheight;

	m_numframes = widthinblocks * heightinblocks;
	caos_assert(m_numframes > 0);

	widths = new unsigned short[m_numframes];
	heights = new unsigned short[m_numframes];
	buffers = new void *[m_numframes];

	unsigned int pitch = bmpdata->biWidth;
	if (imgformat == if_24bit) pitch = (((pitch * 3) + 3) / 4) * 4;
	else if (bmpdata->biCompression == BI_RGB) pitch = ((pitch + 3) / 4) * 4;

	unsigned int curr_row = 0, curr_col = 0;
	for (unsigned int i = 0; i < m_numframes; i++) {
		widths[i] = blockwidth;
		heights[i] = blockheight;

		unsigned int buffersize = blockwidth * blockheight;
		if (imgformat == if_24bit) { buffersize *= 3; }
		
		buffers[i] = new char *[buffersize];

		for (unsigned int j = 0; j < blockheight; j++) {
			unsigned int srcoffset = (curr_col * blockwidth);
			unsigned int destoffset = blockwidth * j;
			unsigned int datasize = blockwidth;
			if (imgformat == if_24bit) { srcoffset *= 3; destoffset *= 3; datasize *= 3; }
			srcoffset += ((bmpdata->biHeight - 1) - (blockheight * curr_row) - j) * pitch;
			
			memcpy((char *)buffers[i] + destoffset, (char *)bmpdata->bmpdata + srcoffset, datasize);
		}

		curr_col++;
		if (curr_col == widthinblocks) {
			curr_col = 0;
			curr_row++;
		}
	}
}

/* vim: set noet: */
