#include "encoding.h"

#include "endianlove.h"

#include <array>
#include <assert.h>
#include <stdexcept>
#include <utf8proc.h>

std::string codepoint_to_utf8(char32_t c) {
	std::array<uint8_t, 5> s = {0, 0, 0, 0, 0};
	if (utf8proc_encode_char(c, s.data()) <= 0) {
		throw std::domain_error("Can't convert " + std::to_string(c) + " into UTF-8");
	};
	return std::string((char*)s.data());
}

static char32_t utf8_to_codepoint(const std::string& s, size_t& pos) {
	int32_t codepoint;
	utf8proc_ssize_t bytes_read = utf8proc_iterate((unsigned char*)s.c_str() + pos, s.size() - pos, &codepoint);
	if (bytes_read < 0 || codepoint == -1) {
		throw std::domain_error("Invalid UTF-8 codepoint starting with " + std::to_string(s[pos]));
	}
	pos += bytes_read;
	return codepoint;
}

static bool is_valid_utf8(const std::string& str) {
	size_t pos = 0;
	while (pos < str.size()) {
		int32_t codepoint;
		utf8proc_ssize_t bytes_read = utf8proc_iterate((unsigned char*)str.c_str() + pos, str.size() - pos, &codepoint);
		if (bytes_read < 0 || codepoint == -1) {
			return false;
		}
		pos += bytes_read;
	}
	return true;
}

std::string cp1252_to_utf8(const std::string& cp1252_str) {
	std::string utf8_str;
	for (unsigned char c : cp1252_str) {
		utf8_str += codepoint_to_utf8(cp1252_to_codepoint(c));
	}
	return utf8_str;
}

std::string utf8_to_cp1252(const std::string& utf8_str) {
	uint8_t* nfc_char_p = utf8proc_NFC(reinterpret_cast<const unsigned char*>(utf8_str.c_str()));
	std::string nfc_str;
	if (nfc_char_p) {
		nfc_str = std::string(reinterpret_cast<char*>(nfc_char_p));
		free(nfc_char_p);
	} else {
		// TODO: error?
		nfc_str = utf8_str;
	}

	std::string cp1252_str;
	size_t pos = 0;
	while (pos < nfc_str.size()) {
		cp1252_str += unicode_to_cp1252(utf8_to_codepoint(nfc_str, pos));
	}
	return cp1252_str;
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

char32_t cp1252_to_codepoint(unsigned char cp1252_char) {
	if (cp1252_char <= 0x7f) {
		return cp1252_char;
	}
	if (cp1252_char >= 0xa0 && cp1252_char <= 0xff) {
		return cp1252_char;
	}
	switch (cp1252_char) {
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

unsigned char unicode_to_cp1252(char32_t codepoint) {
	if (codepoint <= 0x007f) {
		return codepoint;
	}
	if (codepoint >= 0x00a0 && codepoint <= 0x00ff) {
		return codepoint;
	}
	switch (codepoint) {
		case 0x20ac: return 0x80;
		case 0x201a: return 0x82;
		case 0x0192: return 0x83;
		case 0x201e: return 0x84;
		case 0x2026: return 0x85;
		case 0x2020: return 0x86;
		case 0x2021: return 0x87;
		case 0x02c6: return 0x88;
		case 0x2030: return 0x89;
		case 0x0160: return 0x8a;
		case 0x2039: return 0x8b;
		case 0x0152: return 0x8c;
		case 0x017d: return 0x8e;
		case 0x2018: return 0x91;
		case 0x2019: return 0x92;
		case 0x201c: return 0x93;
		case 0x201d: return 0x94;
		case 0x2022: return 0x95;
		case 0x2013: return 0x96;
		case 0x2014: return 0x97;
		case 0x02dc: return 0x98;
		case 0x2122: return 0x99;
		case 0x0161: return 0x9a;
		case 0x203a: return 0x9b;
		case 0x0153: return 0x9c;
		case 0x017e: return 0x9e;
		case 0x0178: return 0x9f;
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
	uint16_t c1 = read16le(*p);
	*p += 2;
	if (c1 >= 0xd800 && c1 < 0xdc00) {
		uint16_t c2 = read16le(*p);
		*p += 2;
		return ((c1 & 0x3ff) << 10) + (c2 & 0x3ff) + 0x10000;
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