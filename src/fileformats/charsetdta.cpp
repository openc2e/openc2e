#include "fileformats/charsetdta.h"

#include "creaturesException.h"
#include "utils/endianlove.h"

#include <string.h>
#include <string>

/*
  CHARSET.DTA and EuroCharset.dta character data is a form of indexed data: each
  byte is either 0 (for transparent), or an integer. The game engine will pass
  in what colors it wants, but we define a simple default palette here just for
  debugging and ease of use.
*/
static shared_array<Color> getDefaultCharsetPalette() {
	static shared_array<Color> s_palette;
	if (!s_palette) {
		s_palette = shared_array<Color>(256);

		s_palette[0].r = 0;
		s_palette[0].g = 0;
		s_palette[0].b = 0;
		s_palette[0].a = 255;

		s_palette[1].r = 255;
		s_palette[1].g = 255;
		s_palette[1].b = 255;
		s_palette[1].a = 255;

		s_palette[2].r = 128;
		s_palette[2].g = 128;
		s_palette[2].b = 128;
		s_palette[2].a = 255;

		for (int i = 3; i < 256; ++i) {
			s_palette[i].r = 0;
			s_palette[i].g = 0;
			s_palette[i].b = 128;
			s_palette[i].a = 255;
		}

		// nice background color used by spritedumper
		s_palette[100].r = 112;
		s_palette[100].g = 164;
		s_palette[100].b = 236;
	}
	return s_palette;
}

/*
 Creatures 1 and 2 define their fonts in a custom file format, used by the
 CHARSET.DTA and EuroCharset.dta files. CHARSET.DTA covers 128 ASCII
 or Windows-932 characters (for Japanese), and EuroCharset.dta covers the 256
 Windows-1252 characters.
 
 The file format starts with either 72-byte blocks of character data for each
 character (132-byte blocks for the Japanese version). Within the character data,
 each row is padded out to 6 bytes (11 bytes for the Japanese Version), and each
 character is 12 pixels high.
 
 After the character data, there are either 128 or 256 2-byte integers that
 define the character widths.
*/
MultiImage ReadCharsetDtaFile(std::istream& in) {
	std::vector<uint8_t> filedata{std::istreambuf_iterator<char>(in), {}};

	if (!(filedata.size() == 9472 || filedata.size() == 17152 || filedata.size() == 18944)) {
		throw creaturesException("Expected size of charset.dta file to be 9472, 17152, 18944 - got " + std::to_string(filedata.size()));
	}

	const unsigned int char_width = filedata.size() == 17152 ? 11 : 6;
	const unsigned int char_height = 12;
	const unsigned int char_length = char_width * char_height;
	const unsigned int num_characters = filedata.size() / (char_length + 2);

	MultiImage images(num_characters);
	for (unsigned int i = 0; i < num_characters; i++) {
		const unsigned int actual_width = read16le(filedata.data() + num_characters * char_length + i * 2);

		images[i].width = actual_width;
		images[i].height = char_height;
		images[i].format = if_index8;
		images[i].palette = getDefaultCharsetPalette();
		images[i].data = shared_array<uint8_t>(char_height * actual_width);
		for (unsigned int row = 0; row < char_height; ++row) {
			memcpy(
				images[i].data.data() + row * actual_width,
				filedata.data() + i * char_length + row * char_width,
				actual_width);
		}
	}
	return images;
}
