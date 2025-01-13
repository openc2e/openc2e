#include "caoslexer.h"

#include "caostoken.h"
#include "common/Ascii.h"
#include "common/NumericCast.h"

#include <assert.h>
#include <string>
#include <vector>

void lexcaos(std::vector<caostoken>& v, const std::string& p) {
	return lexcaos(v, p.c_str());
}

void lexcaos(std::vector<caostoken>& v, const char* p) {
	const char* basep;
	assert(p);
	int yylineno = 1;
	int baseyylineno;

#define push_value(type) \
	v.push_back(caostoken(type, std::string(basep, numeric_cast<std::string::size_type>(p - basep)), baseyylineno)); \
	goto start;

start:
	basep = p;
	baseyylineno = yylineno;

	// end-of-input
	if (p[0] == '\0') {
		goto eoi;
	}
	// single-line comment
	else if (p[0] == '*') {
		p++;
		while (!(p[0] == '\0' || p[0] == '\n' || (p[0] == '\r' && p[1] == '\n'))) {
			p++;
		}
		push_value(caostoken::TOK_COMMENT);
	}
	// unix newline
	else if (p[0] == '\n') {
		p++;
		yylineno++;
		push_value(caostoken::TOK_NEWLINE);
	}
	// windows newline
	else if (p[0] == '\r' && p[1] == '\n') {
		p += 2;
		yylineno++;
		push_value(caostoken::TOK_NEWLINE);
	}
	// whitespace
	else if (p[0] == ' ' || p[0] == '\t' || (p[0] == '\r' && p[1] != '\n')) {
		p++;
		while (p[0] == ' ' || p[0] == '\t' || (p[0] == '\r' && p[1] != '\n')) {
			p++;
		}
		push_value(caostoken::TOK_WHITESPACE);
	}
	// decimal with no leading digit, e.g. `.275`
	else if (p[0] == '.' && is_ascii_digit(p[1])) {
		p += 2;
		while (is_ascii_digit(p[0])) {
			p++;
		}
		push_value(caostoken::TOK_FLOAT);
	}
	// negative decimal with no leading digit, e.g. `-.15`
	else if (p[0] == '-' && p[1] == '.' && is_ascii_digit(p[2])) {
		p += 3;
		while (is_ascii_digit(p[0])) {
			p++;
		}
		push_value(caostoken::TOK_FLOAT);
	}
	// integers and decimals with leading digits
	else if ((is_ascii_digit(p[0]) && !is_ascii_alpha(p[1])) || (p[0] == '-' && is_ascii_digit(p[1]))) {
		p++;
		while (is_ascii_digit(p[0])) {
			p++;
		}
		if (p[0] != '.') {
			push_value(caostoken::TOK_INT);
		}
		p++;
		while (is_ascii_digit(p[0])) {
			p++;
		}
		push_value(caostoken::TOK_FLOAT);
	}
	// binary strings e.g. %01011010
	else if (p[0] == '%' && (p[1] == '0' || p[1] == '1')) {
		p++;
		while (p[0] == '0' || p[0] == '1') {
			p++;
		}
		push_value(caostoken::TOK_BINARY);
	}
	// quoted strings
	else if (p[0] == '"') {
		p++;
		goto str;
	}
	// bytestrings e.g. [034R]
	else if (p[0] == '[') {
		p++;
		goto bytestr;
	}
	// character literal
	else if (p[0] == '\'') {
		p++;
		if (p[0] == '\\') {
			p++;
		}
		if (p[0] == '\0') {
			push_value(caostoken::TOK_ERROR);
		}
		p++; // the actual char
		if (p[0] == '\0') {
			push_value(caostoken::TOK_ERROR);
		} else if (p[0] != '\'') {
			p++;
			push_value(caostoken::TOK_ERROR);
		}
		p++;
		push_value(caostoken::TOK_CHAR);
	}
	// comma
	else if (p[0] == ',') {
		p++;
		push_value(caostoken::TOK_COMMA);
	}
	// comparison operators
	else if (p[0] == '<' && p[1] == '>') {
		p += 2;
		push_value(caostoken::TOK_WORD);
	} else if (p[0] == '<' && p[1] == '=') {
		p += 2;
		push_value(caostoken::TOK_WORD);
	} else if (p[0] == '>' && p[1] == '=') {
		p += 2;
		push_value(caostoken::TOK_WORD);
	} else if (p[0] == '<') {
		p++;
		push_value(caostoken::TOK_WORD);
	} else if (p[0] == '>') {
		p++;
		push_value(caostoken::TOK_WORD);
	} else if (p[0] == '=') {
		p++;
		push_value(caostoken::TOK_WORD);
	}
	// bareword. this seems a little over-permissive, which characters exactly should we allow?
	else if (is_ascii_alpha(p[0]) || (p[0] == '_' && is_ascii_alpha(p[1]))) {
		p++;
		while (is_ascii_alnum(p[0]) || p[0] == '_' || p[0] == '-' || p[0] == ':' || p[0] == '+' || p[0] == '#' || p[0] == '!' || p[0] == '*') {
			p++;
		}
		push_value(caostoken::TOK_WORD);
	}
	// that should be exhaustive, anything else is an error
	else {
		p++;
		push_value(caostoken::TOK_ERROR);
	}

str:
	if (p[0] == '\0' || p[0] == '\r' || p[0] == '\n') {
		push_value(caostoken::TOK_ERROR);
	} else if (p[0] == '\\') {
		p += 2;
		goto str;
	} else if (p[0] == '"') {
		p++;
		push_value(caostoken::TOK_STRING);
	} else {
		p++;
		goto str;
	}

bytestr:
	if (p[0] == '\0' || p[0] == '\r' || p[0] == '\n') {
		p++;
		push_value(caostoken::TOK_ERROR);
	} else if (p[0] == ']') {
		p++;
		push_value(caostoken::TOK_BYTESTR);
	} else {
		p++;
		goto bytestr;
	}

eoi:
	v.push_back(caostoken(caostoken::TOK_EOI, "\0", baseyylineno));
	for (size_t i = 0; i < v.size(); i++)
		v[i].index = i;
}
