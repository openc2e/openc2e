#pragma once

#include "common/Image.h"

#include <string>

namespace ImageUtils {

MultiImage ReadImage(std::string path);

bool IsBackground(const MultiImage& images);
Image StitchBackground(const MultiImage& images);

Image Tint(const Image& image, uint8_t r, uint8_t g, uint8_t b, uint8_t rotation, uint8_t swap);

Color GetPixelColor(const Image& image, unsigned int x, unsigned int y);

Image ToRGB24(const Image& image);

} // namespace ImageUtils
