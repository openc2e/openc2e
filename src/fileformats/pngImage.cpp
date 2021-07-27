#include "pngImage.h"

#include "ImageUtils.h"
#include "creaturesException.h"
#include "utils/scope_guard.h"

#include <fstream>
#include <memory>
#include <png.h>
#include <zlib.h>

void WritePngFile(const Image& image, const std::string& path) {
	std::ofstream out(path, std::ios_base::binary);
	return WritePngFile(image, out);
}

void WritePngFile(const Image& image, std::ostream& out) {
	if (image.width == 0 || image.height == 0 || !image.data) {
		throw creaturesException("Can't write image with no data");
	}
	if (image.format == if_index8 && !image.palette) {
		throw creaturesException("Can't write indexed image with no palette");
	}

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) {
		abort();
	}
	auto png_sg = make_scope_guard([&] { png_destroy_write_struct(&png, nullptr); });

	png_infop info = png_create_info_struct(png);
	if (!info) {
		abort();
	}
	auto info_sg = make_scope_guard([&] { png_free_data(png, info, PNG_FREE_ALL, -1); });

	if (setjmp(png_jmpbuf(png))) {
		abort();
	}

	auto write_data_fn = [](png_structp png, png_bytep buf, png_size_t size) {
		std::ostream* out = (std::ostream*)png_get_io_ptr(png);
		out->write((char*)buf, size);
	};
	auto flush_data_fn = [](png_structp png) {
		std::ostream* out = (std::ostream*)png_get_io_ptr(png);
		out->flush();
	};

	png_set_write_fn(png, &out, write_data_fn, flush_data_fn);

	shared_array<uint8_t> out_buffer;
	if (image.format == if_rgb565 || image.format == if_rgb555) {
		png_set_IHDR(png, info, image.width, image.height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		png_color_8 sbit;
		sbit.red = 5;
		sbit.green = image.format == if_rgb565 ? 6 : 5;
		sbit.blue = 5;
		sbit.gray = 0;
		sbit.alpha = 0;
		png_set_sBIT(png, info, &sbit);

		png_write_info(png, info);

		out_buffer = ImageUtils::ToRGB24(image).data;

	} else if (image.format == if_index8) {
		png_set_IHDR(png, info, image.width, image.height,
			8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		if (!image.palette) {
			throw creaturesException("No palette on paletted image");
		}

		std::vector<png_color_struct> palette(image.palette.size());
		for (size_t i = 0; i < palette.size(); ++i) {
			png_color_struct color;
			color.red = image.palette[i].r;
			color.green = image.palette[i].g;
			color.blue = image.palette[i].b;
			palette[i] = color;
		}
		png_set_PLTE(png, info, palette.data(), palette.size());
		png_write_info(png, info);

		out_buffer = image.data;

	} else if (image.format == if_bgr24) {
		png_set_IHDR(png, info, image.width, image.height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		png_write_info(png, info);

		out_buffer = ImageUtils::ToRGB24(image).data;

	} else {
		throw creaturesException("Unsupported format");
	}

	// libpng by default uses Z_FILTERED, which has good compression but is slower
	// than alternatives. Z_RLE is 2x faster for images from Creatures 2 (especially
	// Back.s16) and Docking Station, and 0.3x faster for images from Creatures 1.
	// There's a size penalty of 10–20%, which is neglible given these files aren't
	// very big to start with. zlib.h says "Z_RLE is designed to be almost as fast
	// as Z_HUFFMAN_ONLY, but give better compression for PNG image data"
	png_set_compression_strategy(png, Z_RLE);

	for (size_t y = 0; y < image.height; ++y) {
		png_write_row(png, out_buffer.data() + y * png_get_rowbytes(png, info));
	}
	png_write_end(png, info);
}