#include "creaturesException.h"
#include "endianlove.h"
#include "fileformats/blkImage.h"
#include "fileformats/c16Image.h"
#include "fileformats/sprImage.h"
#include "scope_guard.h"

#include <ghc/filesystem.hpp>
#include <fmt/format.h>
#include <fstream>
#include <png.h>
#include <regex>
#include <vector>

namespace fs = ghc::filesystem;

static const uint8_t CREATURES_PALETTE[] = {
    0x00,0x00,0x00, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F,
    0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x04,0x02,0x02,
    0x05,0x06,0x0A, 0x06,0x0A,0x04, 0x06,0x09,0x0C, 0x0B,0x04,0x02, 0x0A,0x06,0x09, 0x0D,0x0A,0x04,
    0x0C,0x0B,0x0C, 0x06,0x07,0x11, 0x05,0x0D,0x15, 0x06,0x0F,0x18, 0x09,0x07,0x11, 0x0B,0x0D,0x12,
    0x0B,0x0E,0x1A, 0x07,0x10,0x07, 0x07,0x10,0x0A, 0x0D,0x12,0x06, 0x0D,0x12,0x0B, 0x0F,0x18,0x06,
    0x0F,0x18,0x0A, 0x06,0x10,0x17, 0x07,0x10,0x19, 0x0D,0x11,0x14, 0x0B,0x13,0x1A, 0x0E,0x18,0x13,
    0x0F,0x18,0x1C, 0x12,0x06,0x02, 0x12,0x07,0x09, 0x14,0x0B,0x04, 0x12,0x0D,0x0B, 0x1A,0x06,0x03,
    0x1B,0x07,0x09, 0x1B,0x0C,0x04, 0x1A,0x0D,0x09, 0x12,0x0E,0x12, 0x12,0x0E,0x1A, 0x1A,0x0D,0x12,
    0x1D,0x0D,0x1A, 0x14,0x12,0x05, 0x14,0x12,0x0C, 0x14,0x19,0x06, 0x13,0x1A,0x0B, 0x1C,0x12,0x05,
    0x1B,0x13,0x0B, 0x1C,0x19,0x05, 0x1D,0x19,0x0C, 0x13,0x13,0x13, 0x13,0x15,0x1B, 0x15,0x19,0x14,
    0x15,0x19,0x1C, 0x1A,0x15,0x13, 0x1A,0x16,0x1A, 0x1C,0x1A,0x14, 0x1B,0x1B,0x1B, 0x0C,0x0F,0x21,
    0x0E,0x17,0x24, 0x10,0x0F,0x21, 0x13,0x16,0x23, 0x12,0x16,0x2C, 0x14,0x1A,0x23, 0x12,0x1B,0x2B,
    0x19,0x16,0x22, 0x19,0x17,0x2B, 0x1B,0x1C,0x23, 0x1B,0x1D,0x2A, 0x13,0x17,0x31, 0x14,0x1D,0x32,
    0x17,0x1C,0x3B, 0x1A,0x1E,0x33, 0x19,0x1E,0x3D, 0x1A,0x23,0x0D, 0x17,0x21,0x13, 0x17,0x20,0x1A,
    0x1B,0x23,0x13, 0x1D,0x22,0x1C, 0x1E,0x29,0x13, 0x1E,0x29,0x1A, 0x16,0x20,0x23, 0x17,0x20,0x2E,
    0x1C,0x21,0x25, 0x1D,0x22,0x2B, 0x1F,0x29,0x23, 0x1E,0x29,0x2C, 0x16,0x21,0x33, 0x16,0x24,0x39,
    0x16,0x29,0x3C, 0x1C,0x22,0x33, 0x1D,0x22,0x3F, 0x1E,0x28,0x36, 0x1C,0x29,0x3B, 0x23,0x06,0x04,
    0x24,0x07,0x09, 0x22,0x0D,0x04, 0x23,0x0D,0x0A, 0x2B,0x06,0x04, 0x2B,0x07,0x08, 0x2A,0x0C,0x04,
    0x2B,0x0C,0x0A, 0x26,0x0D,0x12, 0x23,0x13,0x05, 0x23,0x14,0x0A, 0x24,0x1A,0x05, 0x24,0x1A,0x0C,
    0x2B,0x14,0x05, 0x2A,0x15,0x0A, 0x2C,0x1A,0x05, 0x2B,0x1B,0x0B, 0x22,0x15,0x12, 0x22,0x16,0x1B,
    0x23,0x1B,0x13, 0x22,0x1D,0x1B, 0x2B,0x14,0x12, 0x2C,0x15,0x19, 0x2A,0x1D,0x12, 0x2B,0x1D,0x1A,
    0x34,0x0B,0x07, 0x35,0x0D,0x12, 0x32,0x15,0x05, 0x32,0x15,0x0A, 0x33,0x1A,0x05, 0x33,0x1C,0x0B,
    0x3A,0x14,0x05, 0x3A,0x14,0x0B, 0x3A,0x1D,0x05, 0x3A,0x1D,0x0A, 0x33,0x14,0x12, 0x33,0x15,0x19,
    0x33,0x1D,0x12, 0x32,0x1D,0x1A, 0x3A,0x14,0x14, 0x3B,0x16,0x18, 0x3C,0x1C,0x12, 0x3B,0x1C,0x1C,
    0x24,0x0F,0x21, 0x23,0x14,0x21, 0x21,0x1E,0x24, 0x21,0x1E,0x2A, 0x2A,0x1E,0x22, 0x29,0x1F,0x29,
    0x20,0x1F,0x31, 0x34,0x0C,0x20, 0x36,0x1C,0x22, 0x3B,0x1D,0x33, 0x29,0x22,0x0B, 0x25,0x21,0x14,
    0x24,0x22,0x1C, 0x22,0x2B,0x14, 0x23,0x2B,0x1B, 0x2C,0x22,0x14, 0x2B,0x23,0x1B, 0x2D,0x29,0x14,
    0x2D,0x2A,0x1C, 0x27,0x31,0x0F, 0x29,0x34,0x17, 0x34,0x22,0x06, 0x34,0x22,0x0C, 0x35,0x2A,0x05,
    0x34,0x2A,0x0B, 0x3C,0x23,0x05, 0x3B,0x23,0x0B, 0x3D,0x2B,0x05, 0x3D,0x2B,0x0C, 0x33,0x23,0x13,
    0x32,0x25,0x1A, 0x34,0x2A,0x14, 0x34,0x2A,0x1C, 0x3B,0x24,0x12, 0x3B,0x24,0x19, 0x3C,0x2B,0x13,
    0x3B,0x2C,0x1B, 0x34,0x31,0x0E, 0x3D,0x33,0x03, 0x3E,0x33,0x0C, 0x3F,0x3C,0x03, 0x3F,0x3B,0x0B,
    0x35,0x31,0x14, 0x35,0x31,0x1C, 0x32,0x3D,0x14, 0x33,0x3D,0x1B, 0x3E,0x32,0x13, 0x3D,0x33,0x1B,
    0x3E,0x3B,0x13, 0x3F,0x3A,0x1C, 0x23,0x22,0x24, 0x23,0x24,0x2B, 0x24,0x2A,0x24, 0x25,0x2A,0x2D,
    0x2A,0x24,0x23, 0x29,0x26,0x2C, 0x2C,0x2A,0x24, 0x2B,0x2A,0x2D, 0x22,0x25,0x33, 0x21,0x26,0x3E,
    0x25,0x29,0x34, 0x24,0x2A,0x3F, 0x28,0x27,0x31, 0x2B,0x2B,0x33, 0x29,0x2E,0x3D, 0x2A,0x32,0x2A,
    0x26,0x31,0x31, 0x2C,0x30,0x34, 0x2A,0x31,0x3F, 0x2C,0x3A,0x31, 0x2E,0x39,0x3A, 0x33,0x24,0x24,
    0x32,0x26,0x29, 0x33,0x2C,0x23, 0x32,0x2C,0x2C, 0x3B,0x24,0x23, 0x3B,0x24,0x29, 0x3A,0x2D,0x22,
    0x3A,0x2D,0x2A, 0x31,0x2E,0x32, 0x31,0x2F,0x38, 0x3D,0x2B,0x33, 0x35,0x32,0x24, 0x34,0x32,0x2C,
    0x33,0x3C,0x22, 0x33,0x39,0x2C, 0x3C,0x33,0x24, 0x3B,0x34,0x2B, 0x3E,0x3A,0x24, 0x3E,0x3B,0x2C,
    0x35,0x32,0x33, 0x32,0x32,0x3A, 0x35,0x39,0x33, 0x36,0x3A,0x39, 0x39,0x35,0x34, 0x38,0x34,0x38,
    0x3C,0x3A,0x34, 0x3D,0x3D,0x3B, 0x3F,0x3F,0x3F, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
    0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F,
    0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F, 0x3F,0x3F,0x3F
};

static bool imatch(const std::string &s, std::string regex) {
    return std::regex_search(s, std::regex(regex, std::regex::icase));
}

bool is_background_image(const std::unique_ptr<creaturesImage> &image) {
    if (typeid(*image.get()) == typeid(blkImage)) {
        return true;
    }
    if ((typeid(*image.get()) == typeid(sprImage) && image->numframes() == 464) ||
        (typeid(*image.get()) == typeid(s16Image) && image->numframes() == 928)) {
        for (size_t i = 0; i < image->numframes(); ++i) {
            if (image->width(i) != 144 && image->height(i) != 150) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void stitch_background_image(std::unique_ptr<creaturesImage> &image) {
    if (!is_background_image(image)) {
        throw creaturesException("can't stitch a non-background image");
    }

    if (typeid(*image.get()) == typeid(sprImage)) {
        const int sprwidth = 144;
        const int sprheight = 150;
        const int heightinsprites = 8;
        const int widthinsprites = 58;
        const int totalwidth = sprwidth * widthinsprites;
        const int totalheight = sprheight * heightinsprites;

        std::vector<uint8_t> buf(totalwidth * totalheight);

        unsigned int stride = totalwidth;
        for (unsigned int i = 0; i < heightinsprites; i++) {
            for (int j = 0; j < widthinsprites; j++) {
                const unsigned int whereweare = j * heightinsprites + i;
                const int destx = (j * sprwidth);
                const int desty = (i * sprheight);
                uint8_t* sprite = (uint8_t*) image->data(whereweare);
                for (int blocky = 0; blocky < sprheight; blocky++) {
                    uint8_t* start = &buf[(i * sprheight + blocky) * totalwidth + j * sprwidth];
                    std::copy(&sprite[blocky * sprwidth], &sprite[blocky * sprwidth] + sprwidth, start);
                }
            }
        }

        image.reset(new creaturesImage(image->getName(), image->format(), { buf }, { totalwidth }, { totalheight }));
        return;
    }

    if (typeid(*image.get()) == typeid(s16Image)) {
        const int sprwidth = 144;
        const int sprheight = 150;
        const int heightinsprites = 16;
        const int widthinsprites = 58;
        const int totalwidth = sprwidth * widthinsprites;
        const int totalheight = sprheight * heightinsprites;

        std::vector<uint8_t> buf(totalwidth * totalheight * 2);

        unsigned int stride = totalwidth;
        for (unsigned int i = 0; i < heightinsprites; i++) {
            for (int j = 0; j < widthinsprites; j++) {
                const unsigned int whereweare = j * heightinsprites + i;
                const int destx = (j * sprwidth);
                const int desty = (i * sprheight);
                uint8_t* sprite = (uint8_t*) image->data(whereweare);
                for (int blocky = 0; blocky < sprheight; blocky++) {
                    uint8_t* start = &buf[(i * sprheight + blocky) * totalwidth * 2 + j * sprwidth * 2];
                    std::copy(&sprite[blocky * sprwidth * 2], &sprite[blocky * sprwidth * 2] + sprwidth * 2, start);
                }
            }
        }

        image.reset(new creaturesImage(image->getName(), image->format(), { buf }, { totalwidth }, { totalheight }));
        return;
    }
}

std::vector<uint8_t> frame_as_rgb24(const std::unique_ptr<creaturesImage>& image, size_t i) {
    if (!(image->format() == if_16bit_565 || image->format() == if_16bit_555)) {
        throw creaturesException("Don't know how to convert non-RGB565-or-RGB555 format into RGB24");
    }

    std::vector<uint8_t> out(image->width(i) * image->height(i) * 3);
    for (size_t j = 0; j < image->width(i) * image->height(i); ++j) {
        uint16_t pixel = *(((uint16_t*)image->data(i)) + j);
        if (image->format() == if_16bit_565) {
            uint16_t r = (pixel & 0xF800) >> 11;
            uint16_t g = (pixel & 0x07E0) >> 5;
            uint16_t b = (pixel & 0x001F);
            r = r * 255 / 31;
            g = g * 255 / 63;
            b = b * 255 / 31;
            out[j * 3] = r;
            out[j * 3 + 1] = g;
            out[j * 3 + 2] = b;
        } else {
            uint16_t r = (pixel & 0x7C00) >> 10;
            uint16_t g = (pixel & 0x03E0) >> 5;
            uint16_t b = (pixel & 0x001F);
            r = r * 255 / 31;
            g = g * 255 / 31;
            b = b * 255 / 31;
            out[j * 3] = r;
            out[j * 3 + 1] = g;
            out[j * 3 + 2] = b;
        }
    }
    return out;
}

std::vector<uint8_t> frame_as_paletted(const std::unique_ptr<creaturesImage>& image, size_t i) {
    if (!(image->format() == if_paletted)) {
        throw creaturesException("Don't know how to convert non-paletted format into paletted");
    }
    return std::vector<uint8_t>((uint8_t*)image->data(i), ((uint8_t*)image->data(i)) + image->height(i) * image->width(i));
}

void stitch_to_sheet(std::unique_ptr<creaturesImage>& image) {
    if (image->numframes() <= 1) {
        return;
    }

    const int depth = [&]{
        switch (image->format()) {
            case if_16bit_555:
            case if_16bit_565:
                return 2;
            case if_paletted:
                return 1;
            default:
                throw creaturesException("Don't know how to stitch format into sheet");
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
        for (size_t i = 0; i < image->numframes(); ++i) {
            if (current_row_width + image->width(i) + SHEET_PADDING > max_width) {
                total_height += current_row_height + SHEET_PADDING;
                current_row_width = SHEET_PADDING;
                current_row_height = 0;
            }
            current_row_width += image->width(i) + SHEET_PADDING;
            current_row_height = std::max<size_t>(current_row_height, image->height(i));
            total_width = std::max<size_t>(total_width, current_row_width);
        }
        total_height += SHEET_PADDING + current_row_height + SHEET_PADDING;
    }

    // set up storage, set to default background color
    std::vector<uint8_t> data(total_width * total_height * depth);
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
    for (size_t i = 0; i < image->numframes(); ++i) {
        // check row
        if (current_x + image->width(i) + SHEET_PADDING > total_width) {
            current_y += current_row_height + SHEET_PADDING;
            current_x = SHEET_PADDING;
            current_row_height = 0;
        }
        // copy
        for (size_t y = 0; y < image->height(i); y++) {
            auto rowstart = ((uint8_t*)image->data(i)) + y * image->width(i) * depth;
            auto rowend = rowstart + image->width(i) * depth;
            auto insertpos = &data[(current_y + y) * total_width * depth + current_x * depth];
            std::copy(rowstart, rowend, insertpos);
        }
        current_x += image->width(i) + SHEET_PADDING;
        current_row_height = std::max<size_t>(current_row_height, image->height(i));
    }

    // set image
    image.reset(new creaturesImage(image->getName(), image->format(), { data }, { (unsigned short)total_width }, { (unsigned short)total_height }));
}

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "syntax: spritedumper filename" << std::endl;
		exit(1);
	}

	fs::path input_path(argv[1]);
	if (!fs::exists(input_path)) {
		std::cerr << "File " << input_path << " doesn't exist" << std::endl;
		exit(1);
	}
    std::string stem = input_path.stem();
    
    std::ifstream in(input_path);
    
    std::unique_ptr<creaturesImage> image;
    if (imatch(input_path.extension(), "\\.spr$")) {
        image.reset(new sprImage(in, stem));
    } else if (imatch(input_path.extension(), "\\.(s16|photo)$")) {
        image.reset(new s16Image(in, stem));
    } else if (imatch(input_path.extension(), "\\.c16$")) {
        image.reset(new c16Image(in, stem));
    } else if (imatch(input_path.extension(), "\\.blk$")) {
        image.reset(new blkImage(in, stem));
    } else {
        std::cerr << "File " << input_path << " doesn't have known sprite extension" << std::endl;
        exit(1);
    }

    if (is_background_image(image)) {
        stitch_background_image(image);
    }

    stitch_to_sheet(image);

    for (size_t i = 0; i < image->numframes(); ++i) {
        std::string frame_filename = [&](){
            if (image->numframes() == 1) {
                return stem + ".png";
            } else {
                return stem + fmt::format("_{:03}.png", i);
            }
        }();
        fmt::print("{}\n", frame_filename);
        
        FILE *fp = fopen(frame_filename.c_str(), "wb");
        auto fp_sg = make_scope_guard([&]{ fclose(fp); });
        
        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png) { abort(); }
        auto png_sg = make_scope_guard([&]{ png_destroy_write_struct(&png, nullptr); });
        
        png_infop info = png_create_info_struct(png);
        if (!info) { abort(); }
        auto info_sg = make_scope_guard([&]{ png_free_data(png, info, PNG_FREE_ALL, -1); });
        
        if (setjmp(png_jmpbuf(png))) {
            printf("error during png creation\n");
            abort();
        }

        png_init_io(png, fp);
        std::vector<uint8_t> out_buffer;

        if (image->format() == if_16bit_565 || image->format() == if_16bit_555) {
            png_set_IHDR(png, info, image->width(i), image->height(i),
              8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
              PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
            );

            png_color_8 sbit;
            sbit.red = 5;
            sbit.green = image->format() == if_16bit_565 ? 6 : 5;
            sbit.blue = 5;
            sbit.gray = 0;
            sbit.alpha = 0;
            png_set_sBIT(png, info, &sbit);

            png_write_info(png, info);

            out_buffer = frame_as_rgb24(image, i);

        } else if (image->format() == if_paletted) {
            png_set_IHDR(png, info, image->width(i), image->height(i),
              8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
              PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
            );

            std::vector<png_color_struct> palette(256);
            for (size_t i = 0; i < palette.size(); ++i) {
                png_color_struct color;
                color.red = CREATURES_PALETTE[i * 3] << 2;
                color.green = CREATURES_PALETTE[i * 3 + 1] << 2;
                color.blue = CREATURES_PALETTE[i * 3 + 2] << 2;
                palette[i] = color;
            }
            png_set_PLTE(png, info, palette.data(), palette.size());
            png_write_info(png, info);

            out_buffer = frame_as_paletted(image, i);

        } else {
            std::cerr << "creaturesImage: expected 8-bit or 16-bit" << std::endl;
            exit(1);
        }

        std::vector<png_bytep> row_pointers(image->height(i));
        for (size_t y = 0; y < image->height(i); ++y) {
            row_pointers[y] = out_buffer.data() + y * png_get_rowbytes(png, info);
        }
        png_write_image(png, row_pointers.data());
        png_write_end(png, info);
    }

}