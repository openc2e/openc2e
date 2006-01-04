/*
 *  c16Image.cpp
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

#include "c16Image.h"
#include "openc2e.h"

void c16Image::readHeader(std::istream &in) {
	uint32 flags; uint16 spritecount;
	in.read((char *)&flags, 4); flags = swapEndianLong(flags);
	is_565 = (flags & 0x01);
	assert(flags & 0x02);
	in.read((char *)&spritecount, 2); m_numframes = swapEndianShort(spritecount);

	widths = new unsigned short[m_numframes];
	heights = new unsigned short[m_numframes];
	lineoffsets = new unsigned int *[m_numframes];

	// first, read the headers.
	for (unsigned int i = 0; i < m_numframes; i++) {
		uint32 offset;
		in.read((char *)&offset, 4); offset = swapEndianLong(offset);
		in.read((char *)&widths[i], 2); widths[i] = swapEndianShort(widths[i]);
		in.read((char *)&heights[i], 2); heights[i] = swapEndianShort(heights[i]);
		lineoffsets[i] = new unsigned int[heights[i]];
		lineoffsets[i][0] = offset;
		for (unsigned int j = 1; j < heights[i]; j++) {
			in.read((char *)&lineoffsets[i][j], 4); lineoffsets[i][j] = swapEndianLong(lineoffsets[i][j]);
		}
	}
}

void c16Image::duplicateTo(s16Image *img) {
	img->is_565 = is_565;
	img->m_numframes = m_numframes;
	img->offsets = 0;
	img->widths = new unsigned short[m_numframes];
	memcpy(img->widths, widths, m_numframes * sizeof(unsigned short));
	img->heights = new unsigned short[m_numframes];
	memcpy(img->heights, heights, m_numframes * sizeof(unsigned short));
	img->buffers = new void *[m_numframes];
	for (unsigned int i = 0; i < m_numframes; i++) {
		img->buffers[i] = new char[widths[i] * heights[i] * 2];
		memcpy(img->buffers[i], buffers[i], widths[i] * heights[i] * 2);
	}
}

void s16Image::duplicateTo(s16Image *img) {
	img->is_565 = is_565;
	img->m_numframes = m_numframes;
	img->offsets = 0;
	img->widths = new unsigned short[m_numframes];
	memcpy(img->widths, widths, m_numframes * sizeof(unsigned short));
	img->heights = new unsigned short[m_numframes];
	memcpy(img->heights, heights, m_numframes * sizeof(unsigned short));
	img->buffers = new void *[m_numframes];
	for (unsigned int i = 0; i < m_numframes; i++) {
		img->buffers[i] = new char[widths[i] * heights[i] * 2];
		memcpy(img->buffers[i], buffers[i], widths[i] * heights[i] * 2);
	}
}

c16Image::c16Image(mmapifstream *in) {
	stream = in;

	readHeader(*in);
	
	buffers = new void *[m_numframes];
	
	// then, read the files. this involves seeking around, and is hence immensely ghey
	// todo: we assume the file format is valid here. we shouldn't.
	for (unsigned int i = 0; i < m_numframes; i++) {
		buffers[i] = new char[widths[i] * heights[i] * 2];
		uint16 *bufferpos = (uint16 *)buffers[i];
		for (unsigned int j = 0; j < heights[i]; j++) {
			in->seekg(lineoffsets[i][j], std::ios::beg);
			while (true) {
				uint16 tag; in->read((char *)&tag, 2); tag = swapEndianShort(tag);
				if (tag == 0) break;
				bool transparentrun = ((tag & 0x0001) == 0);
				uint16 runlength = (tag & 0xFFFE) >> 1;
				if (transparentrun)
					memset((char *)bufferpos, 0, (runlength * 2));
				else {
					in->read((char *)bufferpos, (runlength * 2));
					for (unsigned int k = 0; k < runlength; k++) {
						bufferpos[k] = swapEndianShort(bufferpos[k]);
					}
				}
				bufferpos += runlength;
			}
		}
		delete[] lineoffsets[i];
	}
	delete[] lineoffsets;
}

void s16Image::readHeader(std::istream &in) {
	uint32 flags; uint16 spritecount;
	in.read((char *)&flags, 4); flags = swapEndianLong(flags);
	is_565 = (flags & 0x01);
	in.read((char *)&spritecount, 2); m_numframes = swapEndianShort(spritecount);
	
	widths = new unsigned short[m_numframes];
	heights = new unsigned short[m_numframes];
	offsets = new unsigned int[m_numframes];

	// first, read the headers.
	for (unsigned int i = 0; i < m_numframes; i++) {
		in.read((char *)&offsets[i], 4); offsets[i] = swapEndianLong(offsets[i]);
		in.read((char *)&widths[i], 2); widths[i] = swapEndianShort(widths[i]);
		in.read((char *)&heights[i], 2); heights[i] = swapEndianShort(heights[i]);
	}
}

void s16Image::writeHeader(std::ostream &s) {
	unsigned int dw; unsigned short w;
	
	dw = (is_565 ? 1 : 0);
	dw = swapEndianLong(dw); s.write((char *)&dw, 4);
	w = m_numframes;
	w = swapEndianShort(w); s.write((char *)&w, 2);
	
	for (unsigned int i = 0; i < m_numframes; i++) {
		dw = offsets[i];
		dw = swapEndianLong(dw); s.write((char *)&dw, 4);
		w = widths[i];
		w = swapEndianShort(w); s.write((char *)&w, 2);
		w = heights[i];
		w = swapEndianShort(w); s.write((char *)&w, 2);
	}
}

bool duppableImage::transparentAt(unsigned int frame, unsigned int x, unsigned int y) {
	unsigned int offset = (y * widths[frame]) + x;
	unsigned short *buffer = (unsigned short *)buffers[frame];
	return (buffer[offset] == 0);
}

s16Image::s16Image(mmapifstream *in) {
	stream = in;

	readHeader(*in);
	
	buffers = new void *[m_numframes];

	for (unsigned int i = 0; i < m_numframes; i++)
		buffers[i] = in->map + offsets[i];

	delete[] offsets;
}

s16Image::~s16Image() {
	delete[] widths;
	delete[] heights;
	if (!stream) { // make sure this isn't a damn mmapifstream..
		for (unsigned int i = 0; i < m_numframes; i++)
			delete (uint16 *)buffers[i];
		delete[] buffers;
	}
	// TODO: we should never have 'offsets' left over here, but .. we should check
}

c16Image::~c16Image() {
	delete[] widths;
	delete[] heights;
	for (unsigned int i = 0; i < m_numframes; i++)
		delete (uint16 *)buffers[i];
	delete[] buffers;
	// TODO: we should never have 'offsets' left over here, but .. we should check
}

void s16Image::tint(unsigned char r, unsigned char g, unsigned char b, unsigned char rotation, unsigned char swap) {
	assert(!stream); // this only works on duplicated images

	if (128 == r == g == b == rotation == swap) return; // duh

	/*
	 * CDN:
	 * if rotation >= 128
	 * absRot = rotation-128
	 * else
	 * absRot = 128 - rotation
	 * endif
	 * invRot = 127-absRot
	 */
	int absRot;
	if (rotation >= 128)
		absRot = (int)rotation - 128;
	else
		absRot = 128 - (int)rotation;
	int invRot = 127 - absRot;

	/*
	 * CDN:
	 * if swap >= 128
	 * absSwap = swap - 128
	 * else
	 * absSwap = 128 - swap
	 * endif
	 * invSwap = 127-absSwap
	 */
	int absSwap;
	if (swap >= 128)
		absSwap = (int)swap - 128;
	else
		absSwap = 128 - (int)swap;
	int invSwap = 127 - absSwap;
	
	/*
	 * CDN:
	 * redTint = red-128
	 * greenTint = green-128
	 * blueTint = blue-128
	 */

	int redTint = (int)r - 128;
	int greenTint = (int)g - 128;
	int blueTint = (int)b - 128;
	std::cout << "tint: " << redTint << ", " << greenTint << ", " << blueTint << std::endl;

	for (unsigned int i = 0; i < m_numframes; i++) {
		for (unsigned int j = 0; j < heights[i]; j++) {
			for (unsigned int k = 0; k < widths[i]; k++) {
				unsigned short v = ((unsigned short *)buffers[i])[(j * widths[i]) + k];
				if (v == 0) continue;

				/*
				 * CDN:
				 * tempRed = RedValue + redTint;
				 * tempGreen = GreenValue + greenTint;
				 * tempBlue = BlueValue + blueTint;
				 */
				int red = (((uint32)(v) & 0xf800) >> 8) + redTint;
				if (red < 0) red = 0; else if (red > 255) red = 255;
				int green = (((uint32)(v) & 0x07e0) >> 3) + greenTint;
				if (green < 0) green = 0; else if (green > 255) green = 255;
				int blue = (((uint32)(v) & 0x001f) << 3) + blueTint;
				if (blue < 0) blue = 0; else if (blue > 255) blue = 255;

				/*
				 * CDN:
				 * if (rotation < 128)
				 * rotRed = ((absRot * tempBlue) + (invRot * tempRed)) / 256
				 * rotGreen = ((absRot * tempRed) + (invRot * tempGreen)) / 256
				 * rotBlue = ((absRot * tempGreen) + (invRot * tempBlue)) / 256
				 * endif
				 */
				
				int rotRed, rotGreen, rotBlue;
				/*if (rotation < 128) {
					rotRed = ((absRot * blue) + (invRot * red)) / 256;
					rotGreen = ((absRot * red) + (invRot * green)) / 256;
					rotBlue = ((absRot * green) + (invRot * blue)) / 256;
				} else if (rotation > 128) {
					// TODO: This isn't actually given in the pseudocode, and I don't get how it works.
					rotRed = ((absRot * green) + (invRot * red)) / 256;
					rotGreen = ((absRot * red) + (invRot * blue)) / 256;
					rotBlue = ((absRot * blue) + (invRot * green)) / 256;

				} else*/ {
					rotRed = red; rotGreen = green; rotBlue = blue;
				}

				/*
				 * CDN:
				 * swappedRed = ((absSwap * rotBlue) + (invSwap * rotRed))/256
				 * swappedBlue = ((absSwap * rotRed) + (invSwap * rotBlue))/256
				 *
				 * fuzzie notes that this doesn't seem to be a no-op for swap=128..
				 */
				/*int swappedRed = ((absSwap * rotBlue) + (invSwap * rotRed)) / 256;
				int swappedBlue = ((absSwap * rotRed) + (invSwap * rotBlue)) / 256;*/
				
				int swappedRed = rotRed, swappedBlue = rotBlue;

				/*
				 * SetColour(definedcolour to (swappedRed,rotGreen,swappedBlue))
				 */
				swappedRed = (swappedRed << 8) & 0xf800;
				rotGreen = (rotGreen << 3) & 0x7e0;
				swappedBlue = (swappedBlue >> 3) & 0x1f;
				v = (swappedRed | rotGreen | swappedBlue);
				/*
				 * if definedcolour ==0 SetColour(definedcolour to (1,1,1))
				 */
				if (v == 0)
					v = (1 << 11 | 1 << 5 | 1);
				((unsigned short *)buffers[i])[(j * widths[i]) + k] = v;
			}
		}
	}
}

/* vim: set noet: */
