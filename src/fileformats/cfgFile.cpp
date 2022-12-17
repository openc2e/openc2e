#include "cfgFile.h"

#include "common/Ascii.h"
#include "common/SimpleLexer.h"
#include "common/readfile.h"

#include <assert.h>
#include <fstream>


std::map<std::string, std::string> readcfgfile(std::istream& in) {
	std::string data = readfile(in);
	std::map<std::string, std::string> result;

	auto tokens = simplelex(data.c_str());
	auto p = tokens.begin();
	while (true) {
		while (p->type == SIMPLE_WHITESPACE || p->type == SIMPLE_NEWLINE) {
			p++;
		}
		if (p->type == SIMPLE_EOI) {
			break;
		}

		std::string key;
		if (p->type == SIMPLE_BAREWORD || p->type == SIMPLE_STRING) {
			key = p->value();
		} else {
			printf("error parsing CFG file, expected key but got: %s\n", p->c_str());
			return {};
		}
		p++;

		if (p->type != SIMPLE_WHITESPACE) {
			printf("error parsing CFG file, expected whitespace after string but got: %s\n", p->c_str());
			return {};
		}
		p++;

		std::string value;
		bool got_one = false;
		while (true) {
			if (p->type == SIMPLE_STRING) {
				value += p->value();
				p++;
				break;
			} else if (p->type == SIMPLE_BAREWORD || p->type == SIMPLE_NUMBER) {
				value += p->value();
			} else {
				if (!got_one) {
					printf("error parsing CFG file, expected value but got: %s\n", p->c_str());
					return {};
				}
				break;
			}
			got_one = true;
			p++;
		}

		if (p->type == SIMPLE_WHITESPACE) {
			p++;
		}
		if (p->type != SIMPLE_NEWLINE) {
			printf("error parsing CFG file, expected newline after string but got: %s\n", p->c_str());
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
