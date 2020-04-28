#include "encoding.h"
#include "encoding_cp1252_codepoints.h"
#include <array>
#include <utf8proc.h>

static std::string codepoint_to_utf8(char32_t c) {
    std::array<uint8_t, 5> s = {0, 0, 0, 0, 0};
    if (utf8proc_encode_char(c, s.data()) <= 0) {
        throw std::domain_error("Can't convert " + std::to_string(c) + " into UTF-8");
    };
    return std::string((char*)s.data());
}

static char32_t utf8_to_codepoint(const std::string& s, size_t& pos)
{
    int32_t codepoint;
    ssize_t bytes_read = utf8proc_iterate((unsigned char*)s.c_str() + pos, s.size() - pos, &codepoint);
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
        ssize_t bytes_read = utf8proc_iterate((unsigned char*)str.c_str() + pos, str.size() - pos, &codepoint);
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
    uint8_t *nfc_char_p = utf8proc_NFC(reinterpret_cast<const unsigned char*>(utf8_str.c_str()));
    std::string nfc_str;
    if (nfc_char_p) {
        nfc_str = std::string(reinterpret_cast<char*>(nfc_char_p), strlen(reinterpret_cast<char*>(nfc_char_p)));
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
    } if (is_valid_utf8(str)) {
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