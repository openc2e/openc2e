#include "ImageUtils.h"
#include "common/Exception.h"
#include "common/Image.h"
#include "common/endianlove.h"

#include <fstream>
#include <zlib.h>

void WritePngFile(const Image& image, const std::string& filename) {
	std::ofstream out(filename, std::ios_base::binary);

	out.write("\x89PNG\r\n\x1a\n", 8);

	uLong crc;
	auto chunk_write = [&](const char* buf, size_t len) {
		crc = crc32_z(crc, reinterpret_cast<const unsigned char*>(buf), len);
		out.write(buf, len);
	};
	auto chunk_write8 = [&](uint8_t val) {
		chunk_write(reinterpret_cast<char*>(&val), 1);
	};
	auto chunk_write32 = [&](uint32_t val) {
		val = is_little_endian() ? byte_swap_32(val) : val;
		chunk_write(reinterpret_cast<char*>(&val), 4);
	};

	// write IHDR chunk
	write32be(out, 13); // ihdr length 13 bytes
	crc = crc32(0L, Z_NULL, 0);
	chunk_write("IHDR", 4);
	chunk_write32(image.width);
	chunk_write32(image.height);
	chunk_write8(8); // bitdepth - 8 bits per channel
	chunk_write8([&] {
		switch (image.format) {
			case if_index8:
				return 3;
			case if_rgb555:
			case if_rgb565:
			case if_bgr24:
			case if_rgb24:
				return 2;
		}
	}()); // colortype
	chunk_write8(0); // compression type 0
	chunk_write8(0); // filter type 0
	chunk_write8(0); // interlace method 0, no interlacing
	write32be(out, crc);

	// write PLTE chunk
	if (image.format == if_index8) {
		write32be(out, 256 * 3); // plte length
		crc = crc32(0L, Z_NULL, 0);
		chunk_write("PLTE", 4);
		for (size_t i = 0; i < 256; ++i) {
			chunk_write8(image.palette[i].r);
			chunk_write8(image.palette[i].g);
			chunk_write8(image.palette[i].b);
		}
		write32be(out, crc);
	}

	// write sBIT chunk
	if (image.format == if_rgb555 || image.format == if_rgb565) {
		write32be(out, 3); // sbit length
		crc = crc32(0L, Z_NULL, 0);
		chunk_write("sBIT", 4);
		chunk_write8(5);
		chunk_write8(image.format == if_rgb565 ? 6 : 5);
		chunk_write8(5);
		write32be(out, crc);
	}

	// generate filtered and compressed data
	auto newimage = image.format == if_index8 ? image : ImageUtils::ToRGB24(image);
	int num_channels = image.format == if_index8 ? 1 : 3;
	std::vector<uint8_t> filtered_data(newimage.data.size() + newimage.height);
	for (int y = 0; y < newimage.height; ++y) {
		auto out = &filtered_data[y * (newimage.width * num_channels + 1)];
		*out = 0; // no filtering
		std::memcpy(out + 1, &newimage.data[y * newimage.width * num_channels], newimage.width * num_channels);
	}
	uLongf compressed_size = compressBound(filtered_data.size());
	std::vector<char> compressed_data(compressed_size);
	int status = compress((Bytef*)compressed_data.data(), &compressed_size, (const Bytef*)filtered_data.data(), filtered_data.size());
	if (status != Z_OK) {
		throw Exception("Failure in compress()");
	}
	compressed_data.resize(compressed_size);

	// write IDAT chunk
	write32be(out, compressed_data.size()); // idat length
	crc = crc32(0L, Z_NULL, 0);
	chunk_write("IDAT", 4);
	chunk_write(compressed_data.data(), compressed_data.size());
	write32be(out, crc);

	// write IEND chunk
	write32be(out, 0);
	crc = crc32(0L, Z_NULL, 0);
	chunk_write("IEND", 4);
	write32be(out, crc);
}