#include "common/Exception.h"
#include "fileformats/ImageUtils.h"
#include "fileformats/c1defaultpalette.h"
#include "fileformats/paletteFile.h"
#include "fileformats/pngImage.h"

#include <fmt/format.h>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

void stitch_to_sheet(MultiImage& image) {
	if (image.size() <= 1) {
		return;
	}

	const int depth = [&] {
		switch (image[0].format) {
			case if_rgb555:
			case if_rgb565:
				return 2;
			case if_index8:
				return 1;
			default:
				throw Exception("Don't know how to stitch format into sheet");
		}
	}();

	// find good sheet size
	// TODO: if it's a creature sprite, use the specific layout so poses line up
	// TODO: better packing algorithm that lets sprites take up multiple rows?
	const int SHEET_PADDING = 5; // TODO: make customizable?
	size_t max_width = 800; // TODO: make customizable?
	size_t total_width = 0;
	size_t total_height = 0;
	{
		size_t current_row_width = SHEET_PADDING;
		size_t current_row_height = 0;
		for (auto& i : image) {
			if (current_row_width + i.width + SHEET_PADDING > max_width) {
				total_height += current_row_height + SHEET_PADDING;
				current_row_width = SHEET_PADDING;
				current_row_height = 0;
			}
			current_row_width += i.width + SHEET_PADDING;
			current_row_height = std::max<size_t>(current_row_height, i.height);
			total_width = std::max<size_t>(total_width, current_row_width);
		}
		total_height += SHEET_PADDING + current_row_height + SHEET_PADDING;
	}

	// set up storage, set to default background color
	shared_array<uint8_t> data(total_width * total_height * depth);
	for (size_t i = 0; i < total_width * total_height; ++i) {
		// TODO: avoid existing colors on image borders, or at least alert
		// TODO: make customizable
		if (depth == 2) {
			// rgb(112, 164, 236) in RGB565LE
			data[2 * i] = 0x3D;
			data[2 * i + 1] = 0x75;
		} else if (depth == 1) {
			// 0x1C,0x29,0x3B in palette.dta -> rgb(112, 164, 236)
			data[i] = 100;
		}
	}

	// copy sprites over
	size_t current_x = SHEET_PADDING;
	size_t current_y = SHEET_PADDING;
	size_t current_row_height = 0;
	for (auto& i : image) {
		// check row
		if (current_x + i.width + SHEET_PADDING > total_width) {
			current_y += current_row_height + SHEET_PADDING;
			current_x = SHEET_PADDING;
			current_row_height = 0;
		}
		// copy
		for (int y = 0; y < i.height; y++) {
			auto rowstart = ((uint8_t*)i.data.data()) + y * i.width * depth;
			auto rowend = rowstart + i.width * depth;
			auto insertpos = &data[(current_y + y) * total_width * depth + current_x * depth];
			std::copy(rowstart, rowend, insertpos);
		}
		current_x += i.width + SHEET_PADDING;
		current_row_height = std::max<size_t>(current_row_height, i.height);
	}

	// set image
	image.resize(1);
	image[0].width = total_width;
	image[0].height = total_height;
	image[0].data = data;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fmt::print(stderr, "syntax: spritedumper filenames...\n");
		exit(1);
	}

	for (int i = 1; i < argc; ++i) {
		fs::path input_path(argv[i]);
		std::string stem = input_path.stem();

		MultiImage image = [&] {
			try {
				return ImageUtils::ReadImage(input_path);
			} catch (Exception& e) {
				fmt::print(stderr, "Exception: {}\n", e.prettyPrint());
				exit(1);
			}
		}();
		if (ImageUtils::IsBackground(image)) {
			image = {ImageUtils::StitchBackground(image)};
		}

		stitch_to_sheet(image);

		shared_array<Color> palette;

		for (size_t i = 0; i < image.size(); ++i) {
			std::string frame_filename = [&]() {
				if (image.size() == 1) {
					return stem + ".png";
				} else {
					return stem + fmt::format("_{:03}.png", i);
				}
			}();
			fmt::print("{}\n", frame_filename);

			WritePngFile(image[i], frame_filename);
		}
	}
}