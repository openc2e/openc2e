#include "cfgFile.h"

#include "common/Exception.h"
#include "common/readfile.h"

#include <assert.h>
#include <fstream>

static char unescape(char c) {
	switch (c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		default: return c;
	}
}

enum cfgtokentype {
	CFG_STRING,
	CFG_BAREWORD,
	CFG_NEWLINE,
	CFG_WHITESPACE,
	CFG_EOI,
	CFG_ERROR
};

struct cfgtoken {
  public:
	cfgtokentype type;
	std::string raw;

	cfgtoken(cfgtokentype type_, std::string raw_)
		: type(type_), raw(std::move(raw_)) {}

	const char* c_str() const {
		return raw.c_str();
	}

	std::string value() const {
		if (type == CFG_BAREWORD) {
			return raw;
		}
		if (type == CFG_STRING) {
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
		throw Exception("cfgtoken::value() not a string");
	}
};

std::vector<cfgtoken> cfglex(const char* p) {
	std::vector<cfgtoken> v;
	const char* basep;
	assert(p);

start:
	basep = p;

#define push_value(type) \
	v.emplace_back(type, std::string(basep, p - basep)); \
	goto start;

	if (p[0] == '\0') {
		goto eoi;
	}
	if ((p[0] == '\r' && p[1] == '\n') || p[0] == '\n') {
		if (p[0] == '\r')
			p++;
		p++;
		push_value(CFG_NEWLINE);
	}
	if (p[0] == ' ' || p[0] == '\t' || p[0] == '\r') {
		while (p[0] == ' ' || p[0] == '\t' || p[0] == '\r') {
			p++;
		}
		push_value(CFG_WHITESPACE);
	}
	if (p[0] == '"') {
		p++;
		goto str;
	}
	if (!(p[0] == '\0' || p[0] == ' ' || p[0] == '\r' || p[0] == '\n' || p[0] == '\t' || p[0] == '"')) {
		while (!(p[0] == '\0' || p[0] == ' ' || p[0] == '\r' || p[0] == '\n' || p[0] == '\t' || p[0] == '"')) {
			p++;
		}
		push_value(CFG_BAREWORD);
	}
	p++;
	push_value(CFG_ERROR);

str:
	if (p[0] == '\0' || p[0] == '\r' || p[0] == '\n') {
		p++;
		push_value(CFG_ERROR);
	}
	if (p[0] == '\\') {
		p += 2;
		goto str;
	}
	if (p[0] == '"') {
		p++;
		push_value(CFG_STRING);
	}
	p++;
	goto str;

eoi:
	v.emplace_back(CFG_EOI, "");
	return v;
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

		std::string key;
		if (p->type == CFG_BAREWORD || p->type == CFG_STRING) {
			key = p->value();
		} else {
			throw Exception(fmt::format("error parsing CFG file, expected key but got: {}\n", p->raw));
		}
		p++;

		if (p->type != CFG_WHITESPACE) {
			throw Exception(fmt::format("error parsing CFG file, expected whitespace after string but got: {}", p->raw));
		}
		p++;

		std::string value;
		bool got_one = false;
		while (true) {
			if (p->type == CFG_STRING) {
				value += p->value();
				p++;
				break;
			} else if (p->type == CFG_BAREWORD) {
				value += p->value();
			} else {
				if (!got_one) {
					throw Exception(fmt::format("error parsing CFG file, expected value but got: {}", p->raw));
				}
				break;
			}
			got_one = true;
			p++;
		}

		if (p->type == CFG_WHITESPACE) {
			p++;
		}
		if (p->type != CFG_NEWLINE) {
			throw Exception(fmt::format("error parsing CFG file, expected newline after string but got: {}", p->raw));
		}
		p++;

		if (result.count(key)) {
			throw Exception(fmt::format("error: duplicate key {}", key));
		}

		result[key] = value;
	}
	return result;
}

std::map<std::string, std::string> readcfgfile(ghc::filesystem::path p) {
	std::ifstream in(p, std::ios_base::binary);
	return readcfgfile(in);
}
