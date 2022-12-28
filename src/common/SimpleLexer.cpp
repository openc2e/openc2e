#include "SimpleLexer.h"

#include "common/Ascii.h"
#include "common/NumericCast.h"

#include <assert.h>
#include <string>
#include <vector>


static char unescape(char c) {
	switch (c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		default: return c;
	}
}

static bool is_bareword_body(char c) {
	return is_ascii_alnum(c) || c == '.' || c == '_' || c == '-' || c == ':' || c == '+' || c == '#' || c == '!' || c == '*';
}

std::string SimpleToken::value() const {
	if (type == SIMPLE_STRING) {
		// remove double quotes and unescape characters
		std::string newvalue;
		for (size_t i = 1; i < raw_value.size() - 1; ++i) {
			if (raw_value[i] == '\\') {
				++i;
				newvalue += unescape(raw_value[i]);
			} else {
				newvalue += raw_value[i];
			}
		}
		return newvalue;
	}
	return raw_value;
}

int SimpleToken::integer_value() const {
	assert(is_integer());
	return std::stoi(raw_value);
}

bool SimpleToken::is_one_of(std::initializer_list<SimpleTokenType> list) const {
	for (auto t : list) {
		if (type == t) {
			return true;
		}
	}
	return false;
}

bool SimpleToken::is_integer() const {
	return type == SIMPLE_NUMBER && raw_value[0] != '-' && raw_value.find(".") == raw_value.npos;
}

bool SimpleToken::is_symbol(char c) const {
	return type == SIMPLE_ERROR && raw_value[0] == c;
}

std::string SimpleToken::repr() const {
	// TODO: escape characters
	return std::string(c_str()) + " " + raw_value;
}

const char* SimpleToken::c_str() const {
	switch (type) {
		case SIMPLE_BAREWORD: return "string";
		case SIMPLE_STRING: return "string";
		case SIMPLE_CHAR: return "char";
		case SIMPLE_NUMBER: return "number";
		case SIMPLE_BYTESTR: return "bytestr";
		case SIMPLE_BINARY: return "binary";
		case SIMPLE_COMMENT: return "comment";
		case SIMPLE_WHITESPACE: return "whitespace";
		case SIMPLE_NEWLINE: return "newline";
		case SIMPLE_ERROR: return "lexer error";
		case SIMPLE_EOI: return "<eoi>";
	}
}

std::vector<SimpleToken> simplelex(const char* p, SimpleLexerConfiguration conf) {
	std::vector<SimpleToken> v;
	const char* basep;
	assert(p);
	int yylineno = 1;
	int baseyylineno;

#define push_value(type) \
	v.push_back(SimpleToken(type, baseyylineno, std::string(basep, numeric_cast<std::string::size_type>(p - basep)))); \
	goto start;

#define push_value_and_error(type) \
	v.push_back(SimpleToken(type, baseyylineno, std::string(basep, numeric_cast<std::string::size_type>(p - basep)))); \
	v.push_back(SimpleToken(SIMPLE_ERROR, baseyylineno, "")); \
	goto start;

start:
	basep = p;
	baseyylineno = yylineno;

	if (p[0] == '\0') {
		goto eoi;
	} else if ((conf.asterisk_comments && p[0] == '*') || (conf.number_sign_comments && p[0] == '#')) {
		p++;
		while (!(p[0] == '\0' || p[0] == '\n' || (p[0] == '\r' && p[1] == '\n'))) {
			p++;
		}
		push_value(SIMPLE_COMMENT);
	} else if (conf.paren_asterisk_comments && p[0] == '(' && p[1] == '*') {
		p += 2;
		// TODO: do these comments allow embedded newlines?
		while (!(p[0] == '\0' || (p[0] == '*' && p[1] == ')'))) {
			p++;
		}
		if (p[0] == '\0') {
			push_value_and_error(SIMPLE_COMMENT);
		}
		p += 2;
		push_value(SIMPLE_COMMENT);
	} else if (p[0] == '\n') {
		p++;
		yylineno++;
		push_value(SIMPLE_NEWLINE);
	} else if (p[0] == '\r' && p[1] == '\n') {
		p += 2;
		yylineno++;
		push_value(SIMPLE_NEWLINE);
	} else if (p[0] == ' ' || p[0] == '\t' || (p[0] == '\r' && p[1] != '\n')) {
		p++;
		while (p[0] == ' ' || p[0] == '\t' || (p[0] == '\r' && p[1] != '\n')) {
			p++;
		}
		push_value(SIMPLE_WHITESPACE);
	} else if (p[0] == '.' && is_ascii_digit(p[1])) {
		// decimal with no leading digit, e.g. `.275`
		p += 2;
		while (is_ascii_digit(p[0])) {
			p++;
		}
		push_value(SIMPLE_NUMBER);
	} else if (p[0] == '-' && p[1] == '.' && is_ascii_digit(p[2])) {
		// negative decimal with no leading digit, e.g. `-.15`
		p += 3;
		while (is_ascii_digit(p[0])) {
			p++;
		}
		push_value(SIMPLE_NUMBER);
	} else if ((is_ascii_digit(p[0]) && !is_ascii_alpha(p[1])) || (p[0] == '-' && is_ascii_digit(p[1]))) {
		// normal numbers
		p++;
		while (is_ascii_digit(p[0])) {
			p++;
		}
		if (p[0] == '.' && is_ascii_digit(p[1])) {
			p++;
		}
		while (is_ascii_digit(p[0])) {
			p++;
		}
		push_value(SIMPLE_NUMBER);
	} else if (p[0] == '%' && (p[1] == '0' || p[1] == '1')) {
		p++;
		while (p[0] == '0' || p[0] == '1') {
			p++;
		}
		push_value(SIMPLE_BINARY);
	} else if (p[0] == '"') {
		p++;
		goto str;
	} else if (p[0] == '[') {
		p++;
		goto bytestr;
	} else if (p[0] == '\'') {
		p++;
		if (p[0] == '\\') {
			p++;
		}
		if (p[0] == '\0') {
			push_value(SIMPLE_ERROR);
		}
		p++; // the actual char
		if (p[0] != '\'') {
			p++;
			push_value(SIMPLE_ERROR);
		}
		p++;
		push_value(SIMPLE_CHAR);

	} else if (is_bareword_body(p[0])) {
		p++;
		while (is_bareword_body(p[0])) {
			p++;
		}
		if (p[0] == ':') {
			printf("ERROR\n");
		}
		push_value(SIMPLE_BAREWORD);
	} else {
		p++;
		push_value(SIMPLE_ERROR);
	}

str:
	if (p[0] == '\0' || p[0] == '\r' || p[0] == '\n') {
		p++;
		push_value(SIMPLE_ERROR);
	} else if (p[0] == '\\') {
		p += 2;
		goto str;
	} else if (p[0] == '"') {
		p++;
		push_value(SIMPLE_STRING);
	} else {
		p++;
		goto str;
	}

bytestr:
	if (p[0] == '\0' || p[0] == '\r' || p[0] == '\n') {
		p++;
		push_value(SIMPLE_ERROR);
	} else if (p[0] == ']') {
		p++;
		push_value(SIMPLE_BYTESTR);
	} else {
		p++;
		goto bytestr;
	}

eoi:
	v.push_back(SimpleToken(SIMPLE_EOI, yylineno, "\0"));
	return v;
}