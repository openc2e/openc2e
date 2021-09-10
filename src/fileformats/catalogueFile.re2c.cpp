#include "fileformats/catalogueFile.h"
#include "utils/readfile.h"

#include <assert.h>
#include <fmt/core.h>
#include <fstream>

enum CatalogueParserTokenType {
	CTOK_EOI = 0,
	CTOK_TAG,
	CTOK_ARRAY,
	CTOK_OVERRIDE,
	CTOK_STR,
	CTOK_INT,
};

struct CatalogueParserToken {
	CatalogueParserToken(CatalogueParserTokenType type_)
		: type(type_) {}
	CatalogueParserToken(CatalogueParserTokenType type_, std::string value_)
		: type(type_), value(value_) {}

	CatalogueParserTokenType type;
	std::string value;
};

struct CatalogueParser {
	const char* catalogue_parse_p = nullptr;
	int yylineno = -1;

	explicit CatalogueParser(const char* p)
		: catalogue_parse_p(p), yylineno(1) {}

	void catalogueParseError(const std::string& err) {
		throw catalogueException(fmt::format("Catalogue parse error at line {}: {}", yylineno, err));
	}

	CatalogueFile parse() {
		// Parses the following grammar:
		// -> cgroup*
		// cgroup -> ctag | carray
		// ctag -> CTOK_TAG CTOK_OVERRIDE? CTOK_STR CTOK_STR+
		// carray -> CTOK_ARRAY CTOK_OVERRIDE? CTOK_STR CTOK_INT CTOK_STR+

		CatalogueFile result;
		CatalogueParserToken token = next_token();

		while (true) {
			CatalogueTag tag;
			if (token.type == CTOK_EOI) {
				break;
			} else if (token.type == CTOK_TAG) {
				tag.type = CatalogueTag::TYPE_TAG;
			} else if (token.type == CTOK_ARRAY) {
				tag.type = CatalogueTag::TYPE_ARRAY;
			} else {
				catalogueParseError(fmt::format("Expected TAG or ARRAY, got {}", token.type));
			}

			token = next_token();
			if (token.type == CTOK_OVERRIDE) {
				tag.override = true;
				token = next_token();
			}

			if (token.type != CTOK_STR) {
				catalogueParseError(fmt::format("Expected string, got {}", token.type));
			}
			tag.name = token.value;
			token = next_token();

			int count = -1;
			if (tag.type == CatalogueTag::TYPE_ARRAY) {
				if (token.type != CTOK_INT) {
					catalogueParseError(fmt::format("Expected int, got {}", token.type));
				}
				count = std::stoi(token.value);
				token = next_token();
			}
			while (true) {
				if (token.type != CTOK_STR) {
					break;
				}
				tag.values.push_back(token.value);
				token = next_token();
			}
			if (count != -1 && (size_t)count != tag.values.size()) {
				catalogueParseError(fmt::format("Expected {} values but got {}", count, tag.values.size()));
			}

			result.tags.push_back(tag);
		}
		return result;
	}

	CatalogueParserToken next_token() {
		const char* basep;
		const char* YYMARKER;
		assert(catalogue_parse_p);
	start:
		basep = catalogue_parse_p;

		/*!re2c
        re2c:define:YYCTYPE = "unsigned char";
        re2c:define:YYCURSOR = catalogue_parse_p;
        re2c:yyfill:enable = 0;
        re2c:yych:conversion = 1;
        re2c:indent:top = 1;

        any			= [\001-\377];
        eoi			= [\000];
        comment		= [#*][^\r\n\000]*;
        whitespace  = ( comment | [ \t\r] ) *;
        newline		= [\n];
        tag			= "TAG";
        array		= "ARRAY";
        override	= "OVERRIDE";
        number		= [0-9]+;
        quote		= ["];
        backslash   = [\\];

        eoi { goto eoi; }
        whitespace { goto start; }
        newline { yylineno++; goto start; }
        tag { return {CTOK_TAG}; }
        array { return {CTOK_ARRAY}; }
        override { return {CTOK_OVERRIDE}; }
        number { return {CTOK_INT, std::string(basep, catalogue_parse_p)}; }
        quote { goto str; }
        any { catalogueParseError(fmt::format("Unexpected character '{}' ({:#x})", (char)basep[0], basep[0])); }
        */
	str : {
		std::string temp;
	strloop:
		basep = catalogue_parse_p;

		/*!re2c
        "\\n" { temp += '\n'; goto strloop; }
        "\\t" { temp += '\t'; goto strloop; }
        "\\\\" { temp += '\\'; goto strloop; }
        "\\\"" { temp += '"'; goto strloop; }
        "\\'" { temp += '\''; goto strloop; }
        backslash any { catalogueParseError(fmt::format("Unknown substitution \\{}", basep[1])); }
        quote { return {CTOK_STR, temp}; }
        backslash ? eoi { catalogueParseError("premature end of input"); }
        newline { temp += basep[0]; yylineno++; goto strloop; }
        any { temp += basep[0]; goto strloop; }
    */
	}
	eoi : {
		catalogue_parse_p = NULL;
		return {CTOK_EOI};
	}
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