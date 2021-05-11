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

#include "fileformats/bmpImage.h"

#include "Backend.h"
#include "Engine.h"
#include "caos_assert.h"
#include "creaturesException.h"
#include "utils/endianlove.h"

#include <fstream>
#include <memory>

#define BI_RGB 0
#define BI_RLE8 1
#define BI_RLE4 2
#define BI_BITFIELDS 3

Image ReadBmpFile(const std::string& path) {
	std::ifstream in(path, std::ios_base::binary);
	return ReadBmpFile(in);
}

Image ReadBmpFile(std::istream& in) {
	char magic[2];
	in.read(magic, 2);
	if (std::string(magic, 2) != "BM")
		throw creaturesException("Doesn't seem to be a BMP file.");

	in.seekg(12, std::ios::cur); // skip filesize, reserved bytes, and data offset

	return ReadDibFile(in);
}

Image ReadDibFile(std::istream& in) {
	uint32_t biWidth, biHeight;
	shared_array<Color> palette;
	imageformat imgformat;
	shared_array<uint8_t> bmpdata;

	uint32_t biSize = read32le(in);
	if (biSize != 40) // win3.x format, which the seamonkeys files are in
		throw creaturesException("BMP format we don't understand.");

	biWidth = read32le(in);
	biHeight = read32le(in);
	caos_assert((int)biHeight > 0);

	uint16_t biPlanes = read16le(in);
	if (biPlanes != 1) // single image plane
		throw creaturesException("Contains BMP data we don't understand.");

	uint16_t biBitCount = read16le(in);
	uint16_t biCompression = read32le(in);

	// and now for some stuff we really don't care about
	uint32_t biSizeImage = read32le(in);
	(void)read32le(in); // biXPelsPerMeter
	(void)read32le(in); // biYPelsPerMeter
	uint32_t biColorsUsed = read32le(in);
	(void)read32le(in); // biColorsImportant

	switch (biCompression) {
		case BI_RGB:
			break;

		case BI_RLE4:
			if (biBitCount != 4)
				throw creaturesException("Contains BMP data compressed in a way which isn't possible.");
			break;

		case BI_RLE8:
			if (biBitCount != 8)
				throw creaturesException("Contains BMP data compressed in a way which isn't possible.");
			break;

		case BI_BITFIELDS:
		default:
			throw creaturesException("Contains BMP data compressed in a way we don't understand: " + std::to_string(biCompression));
	}

	switch (biBitCount) {
		case 4:
		case 8: {
			imgformat = if_index8;
			unsigned int num_palette_entries = biColorsUsed != 0 ? biColorsUsed : (biBitCount == 4 ? 16 : 256);
			std::vector<uint8_t> filepalette(num_palette_entries * 4);
			in.read((char*)filepalette.data(), filepalette.size());
			palette = shared_array<Color>(num_palette_entries);
			for (unsigned int i = 0; i < num_palette_entries; i++) {
				palette[i].r = filepalette[(i * 4) + 2];
				palette[i].g = filepalette[(i * 4) + 1];
				palette[i].b = filepalette[i * 4];
				palette[i].a = 0xff;
			}
		} break;

		case 24:
			imgformat = if_bgr24;
			break;

		default:
			throw creaturesException("Contains BMP data of an unsupported bit depth.");
	}

	if (biSizeImage == 0) {
		biSizeImage = biWidth * biHeight * biBitCount / 8;
	}

	if (biCompression == BI_RGB) {
		size_t rowsize = biWidth * biBitCount / 8;
		size_t stride = (biWidth * biBitCount + 31) / 32 * 4; // ceil(biWidth * biBitCount / 32) * 4

		bmpdata = shared_array<uint8_t>(rowsize * biHeight);
		for (size_t i = 0; i < biHeight; ++i) {
			in.read((char*)bmpdata.data() + (biHeight - 1 - i) * rowsize, rowsize);
			in.seekg(stride - rowsize, std::ios_base::cur);
		}
	} else {
		bmpdata = shared_array<uint8_t>(biSizeImage);
		in.read((char*)bmpdata.data(), biSizeImage);
	}

	if (biBitCount == 4 && biCompression == BI_RGB) {
		auto srcdata = bmpdata;
		bmpdata = shared_array<uint8_t>(biWidth * biHeight);

		for (size_t i = 0; i < srcdata.size(); ++i) {
			bmpdata[i * 2] = (srcdata[i] >> 4) & 0xf;
			bmpdata[i * 2 + 1] = srcdata[i] & 0xf;
		}
	}

	if (biCompression == BI_RLE4) {
		// decode an RLE-compressed 4-bit image
		// TODO: sanity checking
		auto srcdata = bmpdata;
		bmpdata = shared_array<uint8_t>(biWidth * (biHeight + 2)); // TODO
		memset(bmpdata.data(), 0, bmpdata.size());

		size_t p = 0;
		unsigned int x = 0;
		unsigned int y = 0;
		while (true) {
			assert(srcdata.size() - p >= 2);
			const unsigned char nopixels = srcdata[p++];
			const unsigned char val = srcdata[p++];
			if (nopixels == 0) { // special
				if (val == 0) { // end of line
					x = 0;
					y += 1;
				} else if (val == 1) { // end of bitmap
					break;
				} else if (val == 2) { // delta
					assert(srcdata.size() - p >= 2);
					const unsigned char horz = srcdata[p++];
					const unsigned char vert = srcdata[p++];
					x += horz;
					y += vert;
				} else { // absolute mode
					uint8_t remaining = val;
					while (remaining) {
						assert(srcdata.size() - p >= 1);
						const unsigned char raw = srcdata[p++];
						const unsigned char upper = (raw >> 4) & 0xf;
						const unsigned char lower = raw & 0xf;
						if (x < biWidth) {
							bmpdata[x + (biHeight - 1 - y) * biWidth] = upper;
							x++;
						}
						remaining--;
						if (remaining) {
							if (x < biWidth) {
								bmpdata[x + (biHeight - 1 - y) * biWidth] = lower;
								x++;
							}
							remaining--;
						}
					}
					if (p % 2 == 1) {
						assert(srcdata[p] == 0);
						p++; // skip padding byte
					}
				}
			} else { // encoded mode
				const unsigned char upper = (val >> 4) & 0xf;
				const unsigned char lower = val & 0xf;
				uint8_t remaining = nopixels;
				while (remaining) {
					if (x < biWidth) {
						bmpdata[x + (biHeight - 1 - y) * biWidth] = upper;
						x++;
					}
					remaining--;
					if (remaining) {
						if (x < biWidth) {
							bmpdata[x + (biHeight - 1 - y) * biWidth] = lower;
							x++;
						}
						remaining--;
					}
				}
			}
		}
	}

	if (biCompression == BI_RLE8) {
		// decode an RLE-compressed 8-bit image
		// TODO: sanity checking
		auto srcdata = bmpdata;
		bmpdata = shared_array<uint8_t>(biWidth * biHeight); // TODO

		unsigned int x = 0, y = 0;
		for (unsigned int i = 0; i < biSizeImage;) {
			unsigned char nopixels = srcdata[i];
			i++;
			unsigned char val = srcdata[i];
			i++;
			if (nopixels == 0) { // special
				if (val == 0) { // end of line
					x = 0;
					y += 1;
				} else if (val == 1) { // end of bitmap
					break;
				} else if (val == 2) { // delta
					unsigned char horz = srcdata[i];
					i++;
					unsigned char vert = srcdata[i];
					i++;
					x += horz;
					y += vert;
				} else { // absolute mode
					for (unsigned int j = 0; j < val; j++) {
						if (x + (y * biWidth) >= biHeight * biWidth)
							break;
						bmpdata[x + ((biHeight - 1 - y) * biWidth)] = srcdata[i];
						i++;
						x++;
					}
					if (val % 2 == 1)
						i++; // skip padding byte
				}
			} else { // run of pixels
				for (unsigned int j = 0; j < nopixels; j++) {
					if (x + (y * biWidth) >= biHeight * biWidth)
						break;
					bmpdata[x + ((biHeight - 1 - y) * biWidth)] = val;
					x++;
				}
			}

			while (x > biWidth) {
				x -= biWidth;
				y++;
			}

			if (x + (y * biWidth) >= biHeight * biWidth)
				break;
		}
	}

	Image image;
	image.width = biWidth;
	image.height = biHeight;
	image.format = imgformat;
	image.data = bmpdata;
	image.palette = palette;
	return image;
}