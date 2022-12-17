#include "catalogueFile.h"

#include "common/SimpleLexer.h"
#include "common/readfile.h"

#include <assert.h>
#include <fmt/core.h>
#include <fstream>

struct CatalogueParser {
	const char* catalogue_parse_p = nullptr;
	std::vector<SimpleToken> tokens;
	size_t token_p = 0;

	explicit CatalogueParser(const char* p)
		: catalogue_parse_p(p) {
		SimpleLexerConfiguration conf;
		conf.asterisk_comments = true;
		conf.number_sign_comments = true;
		tokens = simplelex(p, conf);
	}

	CatalogueFile parse() {
		// Parses the following grammar:
		// -> cgroup*
		// cgroup -> ctag | carray
		// ctag -> CTOK_TAG CTOK_OVERRIDE? CTOK_STR CTOK_STR+
		// carray -> CTOK_ARRAY CTOK_OVERRIDE? CTOK_STR CTOK_INT CTOK_STR+

		CatalogueFile result;
		SimpleToken token = next_token();

		while (true) {
			CatalogueTag tag;
			if (token.type == SIMPLE_EOI) {
				break;
			} else if (token.type == SIMPLE_BAREWORD && token.value() == "TAG") {
				tag.type = CatalogueTag::TYPE_TAG;
			} else if (token.type == SIMPLE_BAREWORD && token.value() == "ARRAY") {
				tag.type = CatalogueTag::TYPE_ARRAY;
			} else {
				throw catalogueException(fmt::format("Catalogue parse error at line {}: Expected TAG or ARRAY, got {}", token.lineno, token.c_str()));
			}

			token = next_token();
			if (token.type == SIMPLE_BAREWORD && token.value() == "OVERRIDE") {
				tag.override = true;
				token = next_token();
			}

			if (token.type != SIMPLE_STRING) {
				throw catalogueException(fmt::format("Catalogue parse error at line {}: Expected string, got {}", token.lineno, token.c_str()));
			}
			tag.name = token.value();
			token = next_token();

			int count = -1;
			if (tag.type == CatalogueTag::TYPE_ARRAY) {
				if (!token.is_integer()) {
					throw catalogueException(fmt::format("Catalogue parse error at line {}: Expected int, got {}", token.lineno, token.c_str()));
				}
				count = token.integer_value();
				token = next_token();
			}
			while (true) {
				if (token.type != SIMPLE_STRING) {
					break;
				}
				tag.values.push_back(token.value());
				token = next_token();
			}
			if (count != -1 && (size_t)count != tag.values.size()) {
				throw catalogueException(fmt::format("Catalogue parse error at line {}: Expected {} values but got {}", token.lineno, count, tag.values.size()));
			}

			result.tags.push_back(tag);
		}
		return result;
	}

	const SimpleToken& next_token() {
		// skip whitespace and comments
		while (tokens[token_p].is_one_of({SIMPLE_WHITESPACE, SIMPLE_COMMENT, SIMPLE_NEWLINE})) {
			token_p++;
		}
		return tokens[token_p++];
	}
};

CatalogueFile readCatalogueFile(ghc::filesystem::path p) {
	std::ifstream in(p, std::ios_base::binary);
	return readCatalogueFile(in);
}

CatalogueFile readCatalogueFile(std::istream& in) {
	std::string data = readfile(in);
	CatalogueParser parser(data.c_str());
	return parser.parse();
}