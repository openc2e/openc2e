#include "encoding.h"

#include "endianlove.h"

#include <array>
#include <assert.h>
#include <stdexcept>

std::string codepoint_to_utf8(char32_t c) {
	std::array<uint8_t, 5> s = {0, 0, 0, 0, 0};
	if (c <= 0x7f) {
		s[0] = static_cast<uint8_t>(c);
	} else if (c <= 0x7ff) {
		s[0] = static_cast<uint8_t>(0xc0 | (c >> 6));
		s[1] = static_cast<uint8_t>(0x80 | (c & 0x3f));
	} else if (c <= 0xffff) {
		s[0] = static_cast<uint8_t>(0xe0 | (c >> 12));
		s[1] = static_cast<uint8_t>(0x80 | ((c >> 6) & 0x3f));
		s[2] = static_cast<uint8_t>(0x80 | (c & 0x3f));
	} else if (c <= 0x10ffff) {
		s[0] = static_cast<uint8_t>(0xf0 | (c >> 18));
		s[1] = static_cast<uint8_t>(0x80 | ((c >> 12) & 0x3f));
		s[2] = static_cast<uint8_t>(0x80 | ((c >> 6) & 0x3f));
		s[3] = static_cast<uint8_t>(0x80 | (c & 0x3f));
	} else {
		throw std::domain_error("Can't convert " + std::to_string(c) + " into UTF-8");
	}
	return std::string((char*)s.data());
}

static size_t utf8decode(const unsigned char* s, char32_t* c) {
	size_t n = 0;
	if (s[0] <= 0x7f) {
		n = 1;
		*c = (char32_t)s[0];
	} else if (
		s[0] >= 0xc0 && s[0] <= 0xdf &&
		s[1] >= 0x80 && s[1] <= 0xbf) {
		n = 2;
		*c = ((char32_t)(s[0] & 0x1f) << 6) |
			 ((char32_t)(s[1] & 0x3f));
	} else if (
		s[0] >= 0xe0 && s[0] <= 0xef &&
		s[1] >= 0x80 && s[1] <= 0xbf &&
		s[2] >= 0x80 && s[2] <= 0xbf) {
		n = 3;
		*c = ((char32_t)(s[0] & 0x0f) << 12) |
			 ((char32_t)(s[1] & 0x3f) << 6) |
			 ((char32_t)(s[2] & 0x3f));
	} else if (
		s[0] >= 0xf0 && s[0] <= 0xf7 &&
		s[1] >= 0x80 && s[1] <= 0xbf &&
		s[2] >= 0x80 && s[2] <= 0xbf &&
		s[3] >= 0x80 && s[3] <= 0xbf) {
		n = 4;
		*c = ((char32_t)(s[0] & 0x07) << 18) |
			 ((char32_t)(s[1] & 0x3f) << 12) |
			 ((char32_t)(s[2] & 0x3f) << 6) |
			 ((char32_t)(s[3] & 0x3f));
	} else {
		// TODO: 5- and 6-byte sequences
		return 0;
	}
	// no surrogate halves allowed
	if (*c >= 0xd800 && *c <= 0xdfff) {
		return 0;
	}
	// no overlongs
	if (*c <= 0x7f) {
		if (n != 1)
			return 0;
	} else if (*c <= 0x7ff) {
		if (n != 2)
			return 0;
	} else if (*c <= 0xffff) {
		if (n != 3)
			return 0;
	} else if (*c <= 0x1fffff) {
		if (n != 4)
			return 0;
	}
	return n;
}

static bool utf8_is_combining_diacritical(char32_t codepoint) {
	return codepoint >= 0x300 && codepoint <= 0x36f;
}

static char32_t utf8_combine_diacriticals(char cp1252_char, char32_t combining) {
	unsigned char c = static_cast<unsigned char>(cp1252_char);
	// basic NFC normalization for characters that exist in both Unicode and CP-1252
	// clang-format off
	if (combining == 0x300 && c == 0x41) return 0xc0;
	if (combining == 0x300 && c == 0x45) return 0xc8;
	if (combining == 0x300 && c == 0x49) return 0xcc;
	if (combining == 0x300 && c == 0x4f) return 0xd2;
	if (combining == 0x300 && c == 0x55) return 0xd9;
	if (combining == 0x300 && c == 0x61) return 0xe0;
	if (combining == 0x300 && c == 0x65) return 0xe8;
	if (combining == 0x300 && c == 0x69) return 0xec;
	if (combining == 0x300 && c == 0x6f) return 0xf2;
	if (combining == 0x300 && c == 0x75) return 0xf9;
	if (combining == 0x301 && c == 0x41) return 0xc1;
	if (combining == 0x301 && c == 0x45) return 0xc9;
	if (combining == 0x301 && c == 0x49) return 0xcd;
	if (combining == 0x301 && c == 0x4f) return 0xd3;
	if (combining == 0x301 && c == 0x55) return 0xda;
	if (combining == 0x301 && c == 0x59) return 0xdd;
	if (combining == 0x301 && c == 0x61) return 0xe1;
	if (combining == 0x301 && c == 0x65) return 0xe9;
	if (combining == 0x301 && c == 0x69) return 0xed;
	if (combining == 0x301 && c == 0x6f) return 0xf3;
	if (combining == 0x301 && c == 0x75) return 0xfa;
	if (combining == 0x301 && c == 0x79) return 0xfd;
	if (combining == 0x302 && c == 0x41) return 0xc2;
	if (combining == 0x302 && c == 0x45) return 0xca;
	if (combining == 0x302 && c == 0x49) return 0xce;
	if (combining == 0x302 && c == 0x4f) return 0xd4;
	if (combining == 0x302 && c == 0x55) return 0xdb;
	if (combining == 0x302 && c == 0x61) return 0xe2;
	if (combining == 0x302 && c == 0x65) return 0xea;
	if (combining == 0x302 && c == 0x69) return 0xee;
	if (combining == 0x302 && c == 0x6f) return 0xf4;
	if (combining == 0x302 && c == 0x75) return 0xfb;
	if (combining == 0x303 && c == 0x41) return 0xc3;
	if (combining == 0x303 && c == 0x4e) return 0xd1;
	if (combining == 0x303 && c == 0x4f) return 0xd5;
	if (combining == 0x303 && c == 0x61) return 0xe3;
	if (combining == 0x303 && c == 0x6e) return 0xf1;
	if (combining == 0x303 && c == 0x6f) return 0xf5;
	if (combining == 0x308 && c == 0x41) return 0xc4;
	if (combining == 0x308 && c == 0x45) return 0xcb;
	if (combining == 0x308 && c == 0x49) return 0xcf;
	if (combining == 0x308 && c == 0x4f) return 0xd6;
	if (combining == 0x308 && c == 0x55) return 0xdc;
	if (combining == 0x308 && c == 0x59) return 0x178;
	if (combining == 0x308 && c == 0x61) return 0xe4;
	if (combining == 0x308 && c == 0x65) return 0xeb;
	if (combining == 0x308 && c == 0x69) return 0xef;
	if (combining == 0x308 && c == 0x6f) return 0xf6;
	if (combining == 0x308 && c == 0x75) return 0xfc;
	if (combining == 0x308 && c == 0x79) return 0xff;
	if (combining == 0x30a && c == 0x41) return 0xc5;
	if (combining == 0x30a && c == 0x61) return 0xe5;
	if (combining == 0x30c && c == 0x53) return 0x160;
	if (combining == 0x30c && c == 0x5a) return 0x17d;
	if (combining == 0x30c && c == 0x73) return 0x161;
	if (combining == 0x30c && c == 0x7a) return 0x17e;
	if (combining == 0x327 && c == 0x43) return 0xc7;
	if (combining == 0x327 && c == 0x63) return 0xe7;
	// clang-format on
	return 0;
}

static char32_t utf8_to_codepoint(const std::string& s, size_t& pos) {
	char32_t c;
	size_t bytes_read = utf8decode((unsigned char*)s.data() + pos, &c);
	if (bytes_read == 0) {
		throw std::domain_error("Invalid UTF-8 codepoint starting with " + std::to_string(s[pos]));
	}
	pos += bytes_read;
	return c;
}

bool is_valid_utf8(const std::string& str) {
	size_t pos = 0;
	while (pos < str.size()) {
		char32_t codepoint;
		size_t bytes_read = utf8decode((unsigned char*)str.c_str() + pos, &codepoint);
		if (bytes_read == 0) {
			return false;
		}
		pos += bytes_read;
	}
	return true;
}

std::string cp1252_to_utf8(const std::string& cp1252_str) {
	std::string utf8_str;
	for (char c : cp1252_str) {
		utf8_str += codepoint_to_utf8(cp1252_to_codepoint(c));
	}
	return utf8_str;
}

std::string utf8_to_cp1252(const std::string& utf8_str) {
	// TODO: assert valid utf8?
	std::string cp1252_str;
	size_t pos = 0;
	while (pos < utf8_str.size()) {
		char32_t codepoint = utf8_to_codepoint(utf8_str, pos);
		if (utf8_is_combining_diacritical(codepoint) && cp1252_str.size() > 0) {
			char32_t combined = utf8_combine_diacriticals(cp1252_str.back(), codepoint);
			if (combined != 0) {
				cp1252_str.back() = unicode_to_cp1252(combined);
				continue;
			}
		}
		cp1252_str += unicode_to_cp1252(codepoint);
	}
	return cp1252_str;
}

static std::string cp1252_to_ascii_lossy(char cp1252_char) {
	unsigned char c = static_cast<unsigned char>(cp1252_char);
	if (c < 128) {
		return std::string(1, cp1252_char);
	}
	switch (c) {
		case 0x82: return ",";
		case 0x84: return ",,";
		case 0x85: return "...";
		case 0x88: return "^";
		case 0x8A: return "S";
		case 0x8B: return "<";
		case 0x8C: return "OE";
		case 0x8E: return "Z";
		case 0x91: return "'";
		case 0x92: return "'";
		case 0x93: return "\"";
		case 0x94: return "\"";
		case 0x96: return "-";
		case 0x97: return "-";
		case 0x98: return "~";
		case 0x9A: return "s";
		case 0x9B: return ">";
		case 0x9C: return "oe";
		case 0x9E: return "z";
		case 0xA0: return " ";
		case 0xA6: return "|";
		case 0xAB: return "<<";
		case 0xBB: return ">>";
		case 0xC0: return "A";
		case 0xC1: return "A";
		case 0xC2: return "A";
		case 0xC3: return "A";
		case 0xC4: return "A";
		case 0xC5: return "A";
		case 0xC6: return "AE";
		case 0xC7: return "C";
		case 0xC8: return "E";
		case 0xC9: return "E";
		case 0xCA: return "E";
		case 0xCB: return "E";
		case 0xCC: return "I";
		case 0xCD: return "I";
		case 0xCE: return "I";
		case 0xCF: return "I";
		case 0xD0: return "D";
		case 0xD1: return "N";
		case 0xD2: return "O";
		case 0xD3: return "O";
		case 0xD4: return "O";
		case 0xD5: return "O";
		case 0xD6: return "O";
		case 0xD8: return "O";
		case 0xD9: return "U";
		case 0xDA: return "U";
		case 0xDB: return "U";
		case 0xDC: return "U";
		case 0xDD: return "Y";
		case 0xDF: return "ss";
		case 0xE0: return "a";
		case 0xE1: return "a";
		case 0xE2: return "a";
		case 0xE3: return "a";
		case 0xE4: return "a";
		case 0xE5: return "a";
		case 0xE6: return "ae";
		case 0xE7: return "c";
		case 0xE8: return "e";
		case 0xE9: return "e";
		case 0xEA: return "e";
		case 0xEB: return "e";
		case 0xEC: return "i";
		case 0xED: return "i";
		case 0xEE: return "i";
		case 0xEF: return "i";
		case 0xF0: return "d";
		case 0xF1: return "n";
		case 0xF2: return "o";
		case 0xF3: return "o";
		case 0xF4: return "o";
		case 0xF5: return "o";
		case 0xF6: return "o";
		case 0xF8: return "o";
		case 0xF9: return "u";
		case 0xFA: return "u";
		case 0xFB: return "u";
		case 0xFC: return "u";
		case 0xFD: return "y";
		case 0xFF: return "y";
	}
	return "?";
}

std::string cp1252_to_ascii_lossy(const std::string& cp1252_str) {
	// TODO: assert CP1252 (e.g. not UTF8)?
	std::string ascii_str;
	for (char c : cp1252_str) {
		ascii_str += cp1252_to_ascii_lossy(c);
	}
	return ascii_str;
}

std::string ensure_utf8(const std::string& str) {
	if (str.substr(0, 3) == "\xef\xbb\xbf") {
		return str.substr(3);
	}
	if (is_valid_utf8(str)) {
		return str;
	} else {
		return cp1252_to_utf8(str);
	}
}

std::string ensure_cp1252(const std::string& str) {
	if (is_valid_utf8(str)) {
		return utf8_to_cp1252(str);
	} else {
		return str.c_str();
	}
}

/*
mapping table available at
ftp://ftp.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
*/

char32_t cp1252_to_codepoint(char cp1252_char) {
	unsigned char c = static_cast<unsigned char>(cp1252_char);
	if (c <= 0x7f) {
		return c;
	}
	if (c >= 0xa0 && c <= 0xff) {
		return c;
	}
	switch (c) {
		case 0x80: return 0x20ac;
		case 0x82: return 0x201a;
		case 0x83: return 0x0192;
		case 0x84: return 0x201e;
		case 0x85: return 0x2026;
		case 0x86: return 0x2020;
		case 0x87: return 0x2021;
		case 0x88: return 0x02c6;
		case 0x89: return 0x2030;
		case 0x8a: return 0x0160;
		case 0x8b: return 0x2039;
		case 0x8c: return 0x0152;
		case 0x8e: return 0x017d;
		case 0x91: return 0x2018;
		case 0x92: return 0x2019;
		case 0x93: return 0x201c;
		case 0x94: return 0x201d;
		case 0x95: return 0x2022;
		case 0x96: return 0x2013;
		case 0x97: return 0x2014;
		case 0x98: return 0x02dc;
		case 0x99: return 0x2122;
		case 0x9a: return 0x0161;
		case 0x9b: return 0x203a;
		case 0x9c: return 0x0153;
		case 0x9e: return 0x017e;
		case 0x9f: return 0x0178;
	}
	// only for 0x81, 0x8d, 0x8f, 0x90, and 0x9d
	throw std::domain_error("Unsupported CP-1252 character " + std::to_string(cp1252_char));
}

char unicode_to_cp1252(char32_t codepoint) {
	if (codepoint <= 0x7f) {
		return static_cast<char>(codepoint);
	}
	if (codepoint >= 0xa0 && codepoint <= 0xff) {
		return static_cast<char>(codepoint);
	}
	switch (codepoint) {
		case 0x20ac: return '\x80';
		case 0x201a: return '\x82';
		case 0x0192: return '\x83';
		case 0x201e: return '\x84';
		case 0x2026: return '\x85';
		case 0x2020: return '\x86';
		case 0x2021: return '\x87';
		case 0x02c6: return '\x88';
		case 0x2030: return '\x89';
		case 0x0160: return '\x8a';
		case 0x2039: return '\x8b';
		case 0x0152: return '\x8c';
		case 0x017d: return '\x8e';
		case 0x2018: return '\x91';
		case 0x2019: return '\x92';
		case 0x201c: return '\x93';
		case 0x201d: return '\x94';
		case 0x2022: return '\x95';
		case 0x2013: return '\x96';
		case 0x2014: return '\x97';
		case 0x02dc: return '\x98';
		case 0x2122: return '\x99';
		case 0x0161: return '\x9a';
		case 0x203a: return '\x9b';
		case 0x0153: return '\x9c';
		case 0x017e: return '\x9e';
		case 0x0178: return '\x9f';
	}
	throw std::domain_error("Code point " + std::to_string(codepoint) + " doesn't exist in CP-1252");
}

bool cp1252_isprint(unsigned char c) {
	// ASCII control characters
	if (c < 0x20) {
		return false;
	}
	// DEL
	if (c == 0x7f) {
		return false;
	}
	// characters that don't exist in CP-1252
	if (c == 0x81 || c == 0x8d || c == 0x8f || c == 0x90 || c == 0x9d) {
		return false;
	}
	return true;
}

static char32_t utf16le_to_codepoint(uint8_t** p) {
	assert(p);
	assert(*p);
	char32_t c1 = read16le(*p);
	*p += 2;
	if (c1 >= 0xd800 && c1 < 0xdc00) {
		char32_t c2 = read16le(*p);
		*p += 2;
		if (c2 >= 0xdc00 && c2 < 0xe000) {
			return ((c1 & 0x3ff) << 10) + (c2 & 0x3ff) + 0x10000;
		} else {
			throw std::domain_error("Encountered unpaired surrogate in UTF-16-LE data");
		}
	}
	return c1;
}

std::string utf16le_to_utf8(uint8_t* data, size_t num_bytes) {
	std::string s;
	uint8_t* p = data;
	while (p < data + num_bytes) {
		s += codepoint_to_utf8(utf16le_to_codepoint(&p));
	}
	return s;
}

bool is_valid_ascii(const uint8_t* s, size_t n) {
	if (s == nullptr) {
		return false;
	}
	size_t pos = 0;
	while (pos < n) {
		if (s[pos] > 127) {
			return false;
		}
		pos += 1;
	}
	return true;
}

bool is_valid_ascii(const std::vector<uint8_t>& data) {
	return is_valid_ascii(data.data(), data.size());
}