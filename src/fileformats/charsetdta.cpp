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
	static shared_array<Color> s_default_charset_palette;
	if (!s_default_charset_palette) {
		s_default_charset_palette = shared_array<Color>(256);
		s_default_charset_palette[0].r = 0;
		s_default_charset_palette[0].g = 0;
		s_default_charset_palette[0].b = 0;
		s_default_charset_palette[0].a = 0xff;
		for (int i = 1; i < 256; i++) {
			s_default_charset_palette[i].r = 0xff;
			s_default_charset_palette[i].g = 0xff;
			s_default_charset_palette[i].b = 0xff;
			s_default_charset_palette[i].a = 0xff;
		}
		// nice background color used by spritedumper
		s_default_charset_palette[100].r = 112;
		s_default_charset_palette[100].g = 164;
		s_default_charset_palette[100].b = 236;
	}
	return s_default_charset_palette;
}

/*
 Creatures 1 and 2 define their fonts in a custom file format, used by the
 CHARSET.DTA and EuroCharset.dta files. CHARSET.DTA covers the 128 ASCII
 characters, and EuroCharset.dta covers the 256 Windows-1252 characters.
 
 The file format starts with either 128 or 256 72-byte blocks of character data.
 Within the character data, each row is padded out to 6 bytes, and each character
 is 12 pixels high.
 
 After the character data, there are either 128 or 256 2-byte integers that
 define the character widths.
*/
MultiImage ReadCharsetDtaFile(std::istream& in) {
	std::vector<uint8_t> filedata{std::istreambuf_iterator<char>(in), {}};

	if (!(filedata.size() == 9472 || filedata.size() == 18944)) {
		throw creaturesException("Expected size of charset.dta file to be 9472 or 18944 - got " + std::to_string(filedata.size()));
	}
	const unsigned int num_characters = filedata.size() / 74;

	MultiImage images(num_characters);
	for (unsigned int i = 0; i < num_characters; i++) {
		const unsigned int char_width = read16le(filedata.data() + num_characters * 72 + i * 2);
		const unsigned int char_height = 12;

		images[i].width = char_width;
		images[i].height = char_height;
		images[i].format = if_index8;
		images[i].palette = getDefaultCharsetPalette();
		images[i].data = shared_array<uint8_t>(char_height * char_width);
		for (unsigned int row = 0; row < char_height; ++row) {
			memcpy(
				images[i].data.data() + row * char_width,
				filedata.data() + i * 72 + row * 6,
				char_width);
		}
	}
	return images;
}
