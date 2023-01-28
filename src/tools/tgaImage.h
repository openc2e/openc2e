#pragma once

#include "common/Image.h"
#include "common/endianlove.h"

#include <fstream>
#include <iostream>
#include <string>

Image ToBGR24(const Image& oldimage) {
	if (oldimage.format == if_bgr24)
		return oldimage;

	Image newimage = oldimage;
	newimage.data = shared_array<uint8_t>(newimage.width * newimage.height * 3);
	newimage.format = if_bgr24;

	for (int j = 0; j < oldimage.width * oldimage.height; ++j) {
		uint8_t r, g, b;
		if (oldimage.format == if_rgb565) {
			uint16_t pixel = *(((uint16_t*)oldimage.data.data()) + j);
			r = ((pixel & 0xF800) >> 11) * 255 / 31;
			g = ((pixel & 0x07E0) >> 5) * 255 / 63;
			b = ((pixel & 0x001F)) * 255 / 31;

		} else if (oldimage.format == if_rgb555) {
			uint16_t pixel = *(((uint16_t*)oldimage.data.data()) + j);
			r = ((pixel & 0x7C00) >> 10) * 255 / 31;
			g = ((pixel & 0x03E0) >> 5) * 255 / 31;
			b = ((pixel & 0x001F)) * 255 / 31;

		} else if (oldimage.format == if_rgb24) {
			r = oldimage.data[j * 3];
			g = oldimage.data[j * 3 + 1];
			b = oldimage.data[j * 3 + 2];

		} else if (oldimage.format == if_index8) {
			r = oldimage.palette[oldimage.data[j]].r;
			g = oldimage.palette[oldimage.data[j]].g;
			b = oldimage.palette[oldimage.data[j]].b;

		} else {
			throw Exception("Expected format RGB565, RGB555, RGB24, or INDEX8");
		}
		newimage.data[j * 3] = b;
		newimage.data[j * 3 + 1] = g;
		newimage.data[j * 3 + 2] = r;
	}
	return newimage;
}

void WriteTgaFile(const Image& image, std::ostream& out) {
	auto img = ToBGR24(image);

	write8(out, 0); // id length
	write8(out, 0); // has color map?
	write8(out, 2); // image type - uncompressed true color
	write16le(out, 0); // color map first entry index
	write16le(out, 0); // color map length
	write8(out, 0); // color map entry size
	write16le(out, 0); // x-origin
	write16le(out, 0); // y-origin
	write16le(out, img.width); // image width
	write16le(out, img.height); // image height
	write8(out, 24); // bits per pixel
	write8(out, 32); // image descriptor bitfield - origin is in upper-left

	out.write((char*)img.data.data(), img.data.size());
}

void WriteTgaFile(const Image& image, const std::string& path) {
	std::ofstream out(path, std::ios_base::binary);
	return WriteTgaFile(image, out);
}