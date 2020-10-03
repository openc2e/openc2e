#pragma once

#include "Image.h"

namespace ImageUtils {

MultiImage ReadImage(std::string path);

Image ToRGB24(const Image& image);

bool IsBackground(const MultiImage& images);
Image StitchBackground(const MultiImage& images);

Image Tint(const Image& image, uint8_t r, uint8_t g, uint8_t b, uint8_t rotation, uint8_t swap);
	


} // namespace ImageUtils