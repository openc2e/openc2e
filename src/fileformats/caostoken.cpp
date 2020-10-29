#include "fileformats/caostoken.h"
#include "creaturesException.h"

static char char_unescape(char c) {
	switch (c) {
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	case '\\': return '\\';
	case '"': return '"';
	default : return c;
	}
}

std::string caostoken::stringval() const {
    if (!(type == TOK_STRING || type == TOK_BYTESTR)) abort();
    if (value[0] == '[') {
        return value.substr(1, value.size() - 2);
    }
    if (value[0] == '"') {
        std::string unescaped;
        for (size_t i = 1; i < value.size() - 1; ++i) {
            if (value[i] == '\\' && i < value.size() - 2) {
                unescaped += char_unescape(value[i+1]);
                ++i;
            } else {
                unescaped += value[i];
            }
        }
        return unescaped;
    }
    abort();
}

std::vector<unsigned char> caostoken::bytestr() const {
    if (type != TOK_BYTESTR) abort();
    std::vector<unsigned char> result;
    for (size_t i = 1; i < value.size() - 1; ++i) {
        if (std::isdigit(value[i])) {
            result.push_back(std::atoi(value.c_str() + i));
            ++i;
            while (std::isdigit(value[i])) {
                ++i;
            }
        }
    }
    return result;
}

int caostoken::intval() const {
    if (type == TOK_INT) {
        try {
          return std::stoi(value);
        } catch (std::out_of_range&) {
          // Creatures Village has some 2147483700 literals, which don't fit in int32_t
          throw creaturesException("Integer literal " + value + " is out of range");
        }
    } else if (type == TOK_CHAR) {
        return value[1];
    } else if (type == TOK_BINARY) {
        int accum = 0;
        for (size_t i = 1; i < value.size(); ++i) {
            accum <<= 1;
            accum += (value[i] == '1');
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
    return std::stof(value);
}

std::string caostoken::format() const {
	switch(type) {
	case TOK_EOI: return "<EOI>";
	case TOK_ERROR: return "<ERROR>";
	case TOK_COMMA:
		return ",";
	case TOK_INT:
	case TOK_BINARY:
	case TOK_CHAR:
	case TOK_FLOAT:
	case TOK_STRING:
	case TOK_WORD:
	case TOK_COMMENT:
	case TOK_BYTESTR:
	case TOK_WHITESPACE:
	case TOK_NEWLINE:
		return value;
	}
}

std::string caostoken::typeAsString() const {
	switch(type) {
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
