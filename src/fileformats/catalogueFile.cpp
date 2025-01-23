#include "catalogueFile.h"

#include "common/Ascii.h"
#include "common/io/FileReader.h"
#include "common/readfile.h"

#include <assert.h>
#include <fmt/core.h>

enum cataloguetokentype {
	TOK_BAREWORD_TAG,
	TOK_BAREWORD_ARRAY,
	TOK_BAREWORD_OVERRIDE,
	TOK_STRING,
	TOK_INTEGER,
	TOK_WHITESPACE,
	TOK_NEWLINE,
	TOK_COMMENT,
	TOK_EOI,
	TOK_ERROR
};

static char unescape(char c) {
	switch (c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		default: return c;
	}
}

struct CatalogueToken {
  public:
	cataloguetokentype type;
	std::string raw;

	CatalogueToken(cataloguetokentype type_, std::string raw_)
		: type(type_), raw(std::move(raw_)) {}

	const char* c_str() const {
		return raw.c_str();
	}

	std::string value() const {
		if (type == TOK_STRING) {
			// remove double quotes and unescape characters
			std::string newvalue;
			for (size_t i = 1; i < raw.size() - 1; ++i) {
				if (raw[i] == '\\') {
					++i;
					newvalue += unescape(raw[i]);
				} else {
					newvalue += raw[i];
				}
			}
			return newvalue;
		}
		throw Exception("CatalogueToken::value() not a string");
	}

	int32_t integer_value() const {
		if (type == TOK_INTEGER) {
			return std::stoi(raw);
		}
		throw Exception("CatalogueToken::integer_value() not an integer");
	}
};

std::vector<CatalogueToken> cataloguelex(const char* p) {
	std::vector<CatalogueToken> v;
	const char* basep;
	assert(p);

start:
	basep = p;

#define push_value(type) \
	v.emplace_back(type, std::string(basep, p - basep)); \
	goto start;
#define match_word_literal(string_value, token_type) \
	if (strncmp(string_value, p, strlen(string_value)) == 0 && !is_ascii_alnum(p[strlen(string_value)])) { \
		p += strlen(string_value); \
		push_value(token_type); \
	}

	if (p[0] == '\0') {
		goto eoi;
	}
	if (p[0] == '\r' && p[1] == '\n') {
		p += 2;
		push_value(TOK_NEWLINE);
	}
	if (p[0] == '\n') {
		p += 1;
		push_value(TOK_NEWLINE);
	}
	if (p[0] == ' ' || p[0] == '\t' || p[0] == '\r') {
		while (p[0] == ' ' || p[0] == '\t' || p[0] == '\r') {
			p++;
		}
		push_value(TOK_WHITESPACE);
	}

	match_word_literal("TAG", TOK_BAREWORD_TAG);
	match_word_literal("ARRAY", TOK_BAREWORD_ARRAY);
	match_word_literal("OVERRIDE", TOK_BAREWORD_OVERRIDE);

	if (p[0] == '#' || p[0] == '*') {
		while (!(p[0] == '\r' || p[0] == '\n' || p[0] == '\0')) {
			p++;
		}
		push_value(TOK_COMMENT);
	}
	if (is_ascii_digit(p[0])) {
		p++;
		while (is_ascii_digit(p[0])) {
			p++;
		}
		push_value(TOK_INTEGER);
	}
	if (p[0] == '"') {
		p++;
		goto str;
	}
	p++;
	push_value(TOK_ERROR);

str:
	if (p[0] == '\0' || p[0] == '\r' || p[0] == '\n') {
		p++;
		push_value(TOK_ERROR);
	}
	if (p[0] == '\\') {
		p += 2;
		goto str;
	}
	if (p[0] == '"') {
		p++;
		push_value(TOK_STRING);
	}
	p++;
	goto str;

eoi:
	v.emplace_back(TOK_EOI, "");
	return v;
}

CatalogueFile catalogueparse(const std::vector<CatalogueToken>& tokens) {
	// Parses the following grammar:
	// -> cgroup*
	// cgroup -> ctag | carray
	// ctag -> CTOK_TAG CTOK_OVERRIDE? CTOK_STR CTOK_STR+
	// carray -> CTOK_ARRAY CTOK_OVERRIDE? CTOK_STR CTOK_INT CTOK_STR+

	size_t token_p = 0;
	int32_t lineno = 1;

	auto next_token = [&] {
		// skip whitespace and comments
		while (tokens[token_p].type == TOK_WHITESPACE || tokens[token_p].type == TOK_COMMENT || tokens[token_p].type == TOK_NEWLINE) {
			if (tokens[token_p].type == TOK_NEWLINE) {
				lineno++;
			}
			token_p++;
		}
		return tokens[token_p++];
	};

	CatalogueFile result;
	CatalogueToken token = next_token();

	while (true) {
		CatalogueTag tag;
		if (token.type == TOK_EOI) {
			break;
		} else if (token.type == TOK_BAREWORD_TAG) {
			tag.type = CatalogueTag::TYPE_TAG;
		} else if (token.type == TOK_BAREWORD_ARRAY) {
			tag.type = CatalogueTag::TYPE_ARRAY;
		} else {
			throw catalogueException(fmt::format("Catalogue parse error at line {}: Expected TAG or ARRAY, got {}", lineno, token.c_str()));
		}

		token = next_token();
		if (token.type == TOK_BAREWORD_OVERRIDE) {
			tag.override = true;
			token = next_token();
		}

		if (token.type != TOK_STRING) {
			throw catalogueException(fmt::format("Catalogue parse error at line {}: Expected string, got {}", lineno, token.c_str()));
		}
		tag.name = token.value();
		token = next_token();

		int count = -1;
		if (tag.type == CatalogueTag::TYPE_ARRAY) {
			if (token.type != TOK_INTEGER) {
				throw catalogueException(fmt::format("Catalogue parse error at line {}: Expected int, got {}", lineno, token.c_str()));
			}
			count = token.integer_value();
			token = next_token();
		}
		while (true) {
			if (token.type != TOK_STRING) {
				break;
			}
			tag.values.push_back(token.value());
			token = next_token();
		}
		if (count != -1 && (size_t)count != tag.values.size()) {
			throw catalogueException(fmt::format("Catalogue parse error at line {}: Expected {} values but got {}", lineno, count, tag.values.size()));
		}

		result.tags.push_back(tag);
	}
	return result;
}

CatalogueFile readCatalogueFile(ghc::filesystem::path p) {
	FileReader in(p);
	return readCatalogueFile(in);
}

CatalogueFile readCatalogueFile(Reader& in) {
	std::string data = readfile(in);
	return catalogueparse(cataloguelex(data.c_str()));
}