#include "cfgFile.h"
#include "common/readfile.h"

#include <assert.h>
#include <fstream>

enum tokentype {
	CFG_BARE_STRING,
	CFG_QUOTED_STRING,
	CFG_WHITESPACE,
	CFG_NEWLINE,
	CFG_EOI,
	CFG_ERROR
};

struct cfgtoken {
	cfgtoken(tokentype type_, std::string value_)
		: type(type_), value(value_) {}
	tokentype type;
	std::string value;
};

static std::vector<cfgtoken> cfglex(const char* p) {
	std::vector<cfgtoken> v;
	const char* basep;
	assert(p);

start:
	basep = p;

#define push_token(type) \
	v.push_back(cfgtoken(type)); \
	goto start;
#define push_value(type) \
	v.push_back(cfgtoken(type, std::string(basep, p - basep))); \
	goto start;

	/*!re2c
	re2c:define:YYCTYPE = "unsigned char";
	re2c:define:YYCURSOR = p;
	re2c:yyfill:enable = 0;
	re2c:yych:conversion = 1;
	re2c:indent:top = 1;

    [\000] { goto eoi; }
    ("\r\n" | "\n") { push_value(CFG_NEWLINE); }
    [ \t\r]+    { push_value(CFG_WHITESPACE); }
    ([A-Za-z0-9\./\\_])+ { push_value(CFG_BARE_STRING); }
    "\"" { goto str; }
    . { push_value(CFG_ERROR); }
*/

str:
	/*!re2c
	[\000\r\n] { push_value(CFG_ERROR); }
	"\\" . { goto str; }
	"\"" { push_value(CFG_QUOTED_STRING);  }
	. { goto str; }
*/

eoi:
	v.push_back(cfgtoken(CFG_EOI, "\0"));
	return v;
}

const char* tokentype_to_charp(tokentype type) {
	switch (type) {
		case CFG_BARE_STRING: return "string";
		case CFG_QUOTED_STRING: return "string";
		case CFG_WHITESPACE: return "whitespace";
		case CFG_NEWLINE: return "newline";
		case CFG_ERROR: return "lexer error";
		case CFG_EOI: return "<eoi>";
	}
}

std::map<std::string, std::string> readcfgfile(std::istream& in) {
	std::string data = readfile(in);
	std::map<std::string, std::string> result;

	auto tokens = cfglex(data.c_str());
	auto p = tokens.begin();
	while (true) {
		while (p->type == CFG_WHITESPACE || p->type == CFG_NEWLINE) {
			p++;
		}
		if (p->type == CFG_EOI) {
			break;
		}
		if (p->type != CFG_BARE_STRING && p->type != CFG_QUOTED_STRING) {
			printf("error parsing CFG file, expected string but got: %s\n", tokentype_to_charp(p->type));
			return {};
		}

		std::string key;
		if (p->type == CFG_BARE_STRING) {
			key = p->value;
		} else if (p->type == CFG_QUOTED_STRING) {
			key = p->value.substr(1, p->value.size() - 2);
		}
		p++;

		if (p->type != CFG_WHITESPACE) {
			printf("error parsing CFG file, expected whitespace after string but got: %s\n", tokentype_to_charp(p->type));
			return {};
		}
		p++;

		std::string value;
		if (p->type == CFG_BARE_STRING) {
			value = p->value;
		} else if (p->type == CFG_QUOTED_STRING) {
			value = p->value.substr(1, p->value.size() - 2);
		}
		p++;

		if (p->type == CFG_WHITESPACE) {
			p++;
		}
		if (p->type != CFG_NEWLINE) {
			printf("error parsing CFG file, expected newline after string but got: %s\n", tokentype_to_charp(p->type));
			return {};
		}
		p++;

		if (result.count(key)) {
			printf("error: duplicate key %s\n", key.c_str());
		}

		result[key] = value;
	}
	return result;
}

std::map<std::string, std::string> readcfgfile(ghc::filesystem::path p) {
	std::ifstream in(p, std::ios_base::binary);
	return readcfgfile(in);
}
