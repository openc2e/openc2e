#include "PraySourceParser.h"
#include "common/encoding.h"

#include <cassert>
#include <fmt/format.h>
#include <iostream>
#include <map>
#include <string>

using namespace PraySourceParser;

static char unescape(char c) {
	switch (c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		default: return c;
	}
}

class PraySourceParserImpl {
  public:
	PraySourceParserImpl(const char* buf);
	std::vector<PraySourceParser::Event> run();
	std::vector<PraySourceParser::Event> next();

  private:
	enum TokenType {
		PS_EOI,
		PS_ERROR,
		PS_NEWLINE,
		PS_WS,
		PS_BAREWORD,
		PS_STRING,
		PS_INTEGER,
		PS_AT,
		PS_COMMENT,
		PS_EN_GB,
	};

	struct Token {
		TokenType type;
		std::string stringval;
		int intval;
	};

	static std::string TokenToString(Token& token);
	void next_token();
	PraySourceParser::Event parse_line();
	bool parse_some_ws();

	const char* p = nullptr;
	int yylineno = -1;
	Token token;
	bool in_group_block = false;
	std::string last_group_type;
	std::string last_group_name;
	bool seen_en_gb = false;
};

void PraySourceParserImpl::next_token() {
	const char* basep;
	const char* YYMARKER;
	assert(p);
	basep = p;
	token.stringval = "";

	/*!re2c
  re2c:define:YYCTYPE = "unsigned char";
  re2c:define:YYCURSOR = p;
  re2c:yyfill:enable = 0;
  re2c:yych:conversion = 1;
  re2c:indent:top = 1;

  [\000] { p = nullptr; token.type = PS_EOI; return; }
  [\n] { yylineno++; token.type = PS_NEWLINE; return; }
  [ \t\r]+ { token.type = PS_WS; return; }
  [A-Za-z]+ {
    token.type = PS_BAREWORD;
    token.stringval = std::string(basep, p - basep);
    return;
  }
  [0-9]+ {
    token.type = PS_INTEGER;
    token.intval = std::atoi(basep);
    return;
  }
  "\"en-GB\"" { token.type = PS_EN_GB; return; }
  [@] { token.type = PS_AT; return; }
  ["] { goto str; }
  "(-" { goto comment; }
  . {
    token.type = PS_ERROR;
    token.stringval = "Unknown char '" + std::string(basep, 1) + "'";
    return;
  }
  */

str:
	/*!re2c
  [\\] ? [\000] {
    token.type = PS_ERROR;
    token.stringval = "Unexpected end of input inside string";
    return;
  }
  [\\] . { token.stringval.push_back(unescape(p[-1])); goto str; }
  ["] {
    token.type = PS_STRING;
    return;
  }
  [\n] { yylineno++; token.stringval.push_back(p[-1]); goto str; }
  . { token.stringval.push_back(p[-1]); goto str; }
  */

comment:
	/*!re2c
  [\000] {
    token.type = PS_ERROR;
    token.stringval = "Unexpected end of input inside comment";
    return;
  }
  "-)" {
    token.type = PS_COMMENT;
    token.stringval = std::string(basep, p - basep);
    return;
  }
  [\n] { yylineno++; goto comment; }
  . { goto comment; }
  */
}

std::string PraySourceParserImpl::TokenToString(PraySourceParserImpl::Token& token) {
	switch (token.type) {
		case PS_EOI:
			return "PS_EOI";
		case PS_ERROR:
			return "PS_ERROR: " + token.stringval;
		case PS_NEWLINE:
			return "PS_NEWLINE";
		case PS_WS:
			return "PS_WS";
		case PS_BAREWORD:
			return "PS_BAREWORD: " + token.stringval;
		case PS_STRING:
			return "string: " + token.stringval;
		case PS_INTEGER:
			return "PS_INTEGER: " + std::to_string(token.intval);
		case PS_AT:
			return "'@'";
		case PS_COMMENT:
			return "PS_COMMENT: " + token.stringval;
		case PS_EN_GB:
			return "PS_EN_GB";
	}
}

PraySourceParserImpl::PraySourceParserImpl(const char* buf)
	: p(buf), yylineno(1) {
	next_token();
}

std::vector<PraySourceParser::Event> PraySourceParserImpl::run() {
	std::vector<PraySourceParser::Event> events;
	while (true) {
		std::vector<PraySourceParser::Event> newevents = next();
		for (auto e : newevents) {
			if (e.has<Error>()) {
				return {e};
			}
		}
		if (newevents.size() == 0) {
			return events;
		}
		events.insert(events.end(), newevents.begin(), newevents.end());
	}
}

std::vector<PraySourceParser::Event> PraySourceParserImpl::next() {
	// parse the following grammar:
	// -> (ws* line ws* (newline|eoi))*
	// line ->
	//    ()
	//  | string ws+ string => StringTag
	//  | string ws+ at ws+ string => StringTagFromFile
	//  | string ws+ integer => IntegerTag
	//  | 'group' ws+ bareword ws+ string => GroupBlockStart
	//  | 'inline' ws+ bareword ws+ string ws+ string => InlineBlock

	// skip starting whitespace, empty lines, and initial en-GB
	while (true) {
		parse_some_ws();
		if (!seen_en_gb && token.type == PS_EN_GB) {
			seen_en_gb = true;
			next_token();
		}
		if (token.type == PS_NEWLINE) {
			next_token();
		} else {
			break;
		}
	}
	if (!seen_en_gb) {
		return {Error{"Expected \"en-GB\", got " + TokenToString(token)}};
	}

	// generate synthetic "GroupBlock" lines
	if (in_group_block && (token.type == PS_BAREWORD || token.type == PS_EOI)) {
		in_group_block = false;
		return {GroupBlockEnd{last_group_type, last_group_name}};
	}

	// parse the actual line
	if (token.type == PS_EOI) {
		return {};
	}
	PraySourceParser::Event result = parse_line();
	if (result.has<Error>()) {
		return {result};
	}

	// parse ending whitespace/newline
	parse_some_ws();
	if (token.type == PS_EOI) {
		return {result};
	}
	if (token.type != PS_NEWLINE) {
		return {Error{"Expected newline, got " + TokenToString(token)}};
	}
	next_token();

	return {result};
}

PraySourceParser::Event PraySourceParserImpl::parse_line() {
	if (token.type == PS_ERROR) {
		return Error{TokenToString(token)};
	}

	if (token.type == PS_STRING) {
		std::string key = token.stringval;
		next_token();

		if (!parse_some_ws()) {
			return Error{"Expected whitespace or newline after string, got " +
						 TokenToString(token)};
		}

		if (token.type == PS_STRING) {
			std::string value = token.stringval;
			next_token();
			return StringTag{key, value};
		} else if (token.type == PS_AT) {
			next_token();
			if (!parse_some_ws()) {
				return Error{"Expected whitespace after '@', got " +
							 TokenToString(token)};
			}
			if (token.type != PS_STRING) {
				return Error{"Expected string after '@', got " + TokenToString(token)};
			}
			std::string value = token.stringval;
			next_token();
			return StringTagFromFile{key, value};
		} else if (token.type == PS_INTEGER) {
			int value = token.intval;
			next_token();
			return IntegerTag{key, value};
		} else {
			return Error{"Expected string, integer, or '@', got " +
						 TokenToString(token)};
		}
	}

	if (token.type == PS_BAREWORD) {
		std::string directive = token.stringval;
		next_token();

		if (directive != "group" && directive != "inline") {
			return Error{"Expected 'group' or 'inline', got " + TokenToString(token)};
		}

		if (!parse_some_ws()) {
			return Error{"Expected whitespace after '" + directive + "', got " +
						 TokenToString(token)};
		}

		if (token.type != PS_BAREWORD) {
			return Error{"Expected block type, got " + TokenToString(token)};
		}
		std::string type = token.stringval;
		next_token();

		if (!parse_some_ws()) {
			return Error{"Expected whitespace after block type"};
		}

		if (directive == "group") {
			if (token.type != PS_STRING) {
				return Error{"Expected block name, got " + TokenToString(token)};
			}
			std::string name = token.stringval;
			next_token();
			// bookkeeping for generating "GroupBlockEnd"
			in_group_block = true;
			last_group_type = type;
			last_group_name = name;
			return GroupBlockStart{type, name};

		} else if (directive == "inline") {
			if (token.type != PS_STRING) {
				return Error{"Expected block label, got " + TokenToString(token)};
			}
			std::string label = token.stringval;
			next_token();

			if (!parse_some_ws())
				return Error{""};

			if (token.type != PS_STRING) {
				return Error{"Expected quoted filename, got " + TokenToString(token)};
			}
			std::string filename = token.stringval;
			next_token();

			return InlineBlock{type, label, filename};
		}
	}

	return Error{"Expected string or bareword, got: " + TokenToString(token)};
}

bool PraySourceParserImpl::parse_some_ws() {
	bool parsed_ws = false;
	while (token.type == PS_WS || token.type == PS_COMMENT) {
		if (token.type == PS_WS) {
			parsed_ws = true;
		}
		next_token();
	}
	return parsed_ws;
}

std::vector<PraySourceParser::Event> PraySourceParser::parse(const std::string& str) {
	std::string s = ensure_utf8(str);
	return PraySourceParserImpl(s.c_str()).run();
}

std::string PraySourceParser::eventToString(const Event& event) {
	if (auto* e = event.get_if<Error>()) {
		return fmt::format("Error({})", e->message);

	} else if (auto* e = event.get_if<GroupBlockStart>()) {
		return fmt::format("GroupBlockStart({}, {})", e->type, e->name);

	} else if (auto* e = event.get_if<GroupBlockEnd>()) {
		return fmt::format("GroupBlockEnd({}, {})", e->type, e->name);

	} else if (auto* e = event.get_if<InlineBlock>()) {
		return fmt::format("InlineBlock({}, {}, {})", e->type, e->name, e->filename);

	} else if (auto* e = event.get_if<StringTag>()) {
		return fmt::format("StringTag({}, {})", e->key, e->value);

	} else if (auto* e = event.get_if<StringTagFromFile>()) {
		return fmt::format("StringTagFromFile({}, {})", e->key, e->filename);

	} else if (auto* e = event.get_if<IntegerTag>()) {
		return fmt::format("IntegerTag({}, {})", e->key, e->value);

	} else {
		return fmt::format("???");
	}
}