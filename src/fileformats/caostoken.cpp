#include "caostoken.h"

#include "common/Exception.h"

static char char_unescape(char c) {
	switch (c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case '\\': return '\\';
		case '"': return '"';
		default: return c;
	}
}

std::string caostoken::stringval() const {
	if (!(type == TOK_STRING || type == TOK_BYTESTR))
		abort();
	if (data[0] == '[') {
		return data.substr(1, data.size() - 2);
	}
	if (data[0] == '"') {
		std::string unescaped;
		for (size_t i = 1; i < data.size() - 1; ++i) {
			if (data[i] == '\\' && i < data.size() - 2) {
				unescaped += char_unescape(data[i + 1]);
				++i;
			} else {
				unescaped += data[i];
			}
		}
		return unescaped;
	}
	abort();
}

std::vector<unsigned char> caostoken::bytestr() const {
	if (type != TOK_BYTESTR)
		abort();
	std::vector<unsigned char> result;
	for (size_t i = 1; i < data.size() - 1; ++i) {
		if (std::isdigit(data[i])) {
			result.push_back(std::atoi(data.c_str() + i));
			++i;
			while (std::isdigit(data[i])) {
				++i;
			}
		}
	}
	return result;
}

int caostoken::intval() const {
	if (type == TOK_INT) {
		try {
			return std::stoi(data);
		} catch (std::out_of_range&) {
			// Creatures Village has some 2147483700 literals, which don't fit in int32_t
			throw Exception("Integer literal " + data + " is out of range");
		}
	} else if (type == TOK_CHAR) {
		return data[1];
	} else if (type == TOK_BINARY) {
		int accum = 0;
		for (size_t i = 1; i < data.size(); ++i) {
			accum <<= 1;
			accum += (data[i] == '1');
		}
		return accum;
	} else {
		abort();
	}
}

float caostoken::floatval() const {
	if (type != TOK_FLOAT) {
		abort();
	}
	return std::stof(data);
}

std::string caostoken::typeAsString() const {
	switch (type) {
		case TOK_WORD: return "word";
		case TOK_BYTESTR: return "bytestr";
		case TOK_STRING: return "string";
		case TOK_CHAR: return "char";
		case TOK_BINARY: return "binary";
		case TOK_INT: return "integer";
		case TOK_FLOAT: return "float";
		case TOK_COMMENT: return "comment";
		case TOK_WHITESPACE: return "whitespace";
		case TOK_NEWLINE: return "newline";
		case TOK_COMMA: return "comma";
		case TOK_EOI: return "eoi";
		case TOK_ERROR: return "lexer error";
		default: abort();
	}
}
