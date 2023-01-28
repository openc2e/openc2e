#include "ImageUtils.h"

#include "blkImage.h"
#include "bmpImage.h"
#include "c16Image.h"
#include "charsetdta.h"
#include "common/Ascii.h"
#include "common/Exception.h"
#include "common/mappedfile.h"
#include "common/spanstream.h"
#include "s16Image.h"
#include "sprImage.h"

#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

namespace ImageUtils {

MultiImage ReadImage(std::string path) {
	if (!fs::exists(path)) {
		throw Exception("File '" + path + "' doesn't exist");
	}

	std::string ext = fs::path(path).extension();
	ext = to_ascii_lowercase(ext);

	mappedfile m(path);
	spanstream in(m);
	if (ext == ".spr") {
		return ReadSprFile(in);
	}
	if (ext == ".s16" || ext == ".photo") {
		return ReadS16File(in);
	}
	if (ext == ".c16") {
		return ReadC16File(in);
	}
	if (ext == ".blk") {
		return {ReadBlkFile(in)};
	}
	if (ext == ".bmp") {
		return {ReadBmpFile(in)};
	}

	std::string filename = fs::path(path).filename();
	filename = to_ascii_lowercase(filename);

	if (filename == "charset.dta" || filename == "eurocharset.dta") {
		return ReadCharsetDtaFile(in);
	}

	throw Exception("Don't know how to read image '" + path + "'");
}

bool IsBackground(const MultiImage& images) {
	if (images.size() == 464 || images.size() == 928) {
		for (const auto& image : images) {
			if (image.width != 144 || image.height != 150) {
				return false;
			}
		}
		return true;
	}
	return false;
}

Image StitchBackground(const MultiImage& images) {
	if (!IsBackground(images)) {
		throw Exception("Stitching non-background images not implemented");
	}

	const int sprwidth = 144;
	const int sprheight = 150;
	const int widthinsprites = 58;
	const unsigned int heightinsprites = [&] {
		if (images.size() == 464)
			return 8;
		if (images.size() == 928)
			return 16;
		throw Exception("Number of frames should be 464 or 928");
	}();
	const int bytes_per_pixel = [&] {
		if (images[0].format == if_index8)
			return 1;
		if (images[0].format == if_rgb555 || images[0].format == if_rgb565)
			return 2;
		throw Exception("Format should be 8-bit, RGB555, or RGB565");
	}();
	const int totalwidth = sprwidth * widthinsprites;
	const int totalheight = sprheight * heightinsprites;

	Image newimage;
	newimage.width = totalwidth;
	newimage.height = totalheight;
	newimage.format = images[0].format;
	newimage.palette = images[0].palette;
	newimage.data = shared_array<uint8_t>(totalwidth * totalheight * bytes_per_pixel);

	for (unsigned int i = 0; i < heightinsprites; i++) {
		for (int j = 0; j < widthinsprites; j++) {
			const unsigned int whereweare = j * heightinsprites + i;
			uint8_t* sprite = (uint8_t*)images[whereweare].data.data();
			for (int blocky = 0; blocky < sprheight; blocky++) {
				uint8_t* start = &newimage.data[(i * sprheight + blocky) * totalwidth * bytes_per_pixel + j * sprwidth * bytes_per_pixel];
				std::copy(&sprite[blocky * sprwidth * bytes_per_pixel], &sprite[blocky * sprwidth * bytes_per_pixel + sprwidth * bytes_per_pixel], start);
			}
		}
	}

	return newimage;
}

Image Tint(const Image& oldimage, unsigned char r, unsigned char g, unsigned char b,
	unsigned char rotation, unsigned char swap) {
	if (!(oldimage.format == if_rgb565 || oldimage.format == if_rgb555)) {
		throw Exception("Tried to tint a sprite in an unsupported format");
	}

	if (128 == r && 128 == g && 128 == b && 128 == rotation && 128 == swap)
		return oldimage; // duh

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

	Image newimage;
	newimage.width = oldimage.width;
	newimage.height = oldimage.height;
	newimage.format = oldimage.format;
	newimage.data = shared_array<uint8_t>(oldimage.data.size());

	for (int j = 0; j < oldimage.height; j++) {
		for (int k = 0; k < oldimage.width; k++) {
			unsigned short v = ((unsigned short*)oldimage.data.data())[(j * oldimage.width) + k];
			if (v == 0) {
				((unsigned short*)newimage.data.data())[(j * newimage.width) + k] = 0;
				continue;
			};

			/*
			 * CDN:
			 * tempRed = RedValue + redTint;
			 * tempGreen = GreenValue + greenTint;
			 * tempBlue = BlueValue + blueTint;
			 */
			// TODO: should this work differently for 565 vs 555 color?
			int red = (((uint32_t)(v)&0xf800) >> 8) + redTint;
			if (red < 0)
				red = 0;
			else if (red > 255)
				red = 255;
			int green = (((uint32_t)(v)&0x07e0) >> 3) + greenTint;
			if (green < 0)
				green = 0;
			else if (green > 255)
				green = 255;
			int blue = (((uint32_t)(v)&0x001f) << 3) + blueTint;
			if (blue < 0)
				blue = 0;
			else if (blue > 255)
				blue = 255;

			/*
			 * CDN:
			 * if (rotation < 128)
			 * rotRed = ((absRot * tempBlue) + (invRot * tempRed)) / 256
			 * rotGreen = ((absRot * tempRed) + (invRot * tempGreen)) / 256
			 * rotBlue = ((absRot * tempGreen) + (invRot * tempBlue)) / 256
			 * endif
			 */

			int rotRed, rotGreen, rotBlue;
			rotRed = ((blue * absRot) + (red * invRot)) / 128;
			rotGreen = ((red * absRot) + (green * invRot)) / 128;
			rotBlue = ((green * absRot) + (blue * invRot)) / 128;


			/*
			 * CDN:
			 * swappedRed = ((absSwap * rotBlue) + (invSwap * rotRed))/256
			 * swappedBlue = ((absSwap * rotRed) + (invSwap * rotBlue))/256
			 *
			 * fuzzie notes that this doesn't seem to be a no-op for swap=128..
			 */
			int swappedRed = ((absSwap * rotBlue) + (invSwap * rotRed)) / 128;
			int swappedBlue = ((absSwap * rotRed) + (invSwap * rotBlue)) / 128;

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
			((unsigned short*)newimage.data.data())[(j * newimage.width) + k] = v;
		}
	}

	return newimage;
}

Color GetPixelColor(const Image& image, unsigned int x, unsigned int y) {
	if (image.format == if_index8) {
		uint8_t palette_index = image.data[y * image.width + x];
		return image.palette[palette_index];

	} else if (image.format == if_rgb555) {
		uint16_t pixel = ((uint16_t*)image.data.data())[y * image.width + x];
		uint8_t r = (pixel & 0x7C00) >> 10;
		uint8_t g = (pixel & 0x03E0) >> 5;
		uint8_t b = (pixel & 0x001F);
		r = r * 255 / 31;
		g = g * 255 / 31;
		b = b * 255 / 31;
		return Color{r, g, b, 255};

	} else if (image.format == if_rgb565) {
		uint16_t pixel = ((uint16_t*)image.data.data())[y * image.width + x];
		uint8_t r = (pixel & 0xF800) >> 11;
		uint8_t g = (pixel & 0x07E0) >> 5;
		uint8_t b = (pixel & 0x001F);
		r = r * 255 / 31;
		g = g * 255 / 63;
		b = b * 255 / 31;
		return Color{r, g, b, 255};

	} else if (image.format == if_bgr24) {
		uint8_t b = image.data[(y * image.width + x) * 3];
		uint8_t g = image.data[(y * image.width + x) * 3 + 1];
		uint8_t r = image.data[(y * image.width + x) * 3 + 2];
		return Color{r, g, b, 255};

	} else {
		throw Exception("GetPixelColor unimplemented for format " + std::to_string(image.format));
	}
}

} // namespace ImageUtils