#include "PraySourceParser.h"

#include "common/Ascii.h"
#include "common/NumericCast.h"
#include "common/encoding.h"

#include <cassert>
#include <fmt/format.h>
#include <string>
#include <vector>

using namespace PraySourceParser;

enum PrayTokenType {
	PRAY_BAREWORD,
	PRAY_STRING,
	PRAY_INTEGER,
	PRAY_ATSIGN,
	PRAY_COMMENT,
	PRAY_WHITESPACE,
	PRAY_NEWLINE,
	PRAY_ERROR,
	PRAY_EOI
};

struct PrayToken {
	PrayToken()
		: type(PRAY_ERROR), lineno(-1) {}
	PrayToken(PrayTokenType type_, int lineno_, std::string raw_value_)
		: type(type_), lineno(lineno_), raw_value(raw_value_) {}
	PrayTokenType type;
	int lineno;
	std::string raw_value;

	std::string value() const;
	int integer_value() const;

	bool is_one_of(std::initializer_list<PrayTokenType>) const;
	bool is_integer() const;
	bool is_atsign() const;
};

static char unescape(char c) {
	switch (c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		default: return c;
	}
}

std::string PrayToken::value() const {
	if (type == PRAY_STRING) {
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

int PrayToken::integer_value() const {
	assert(is_integer());
	return std::stoi(raw_value);
}

bool PrayToken::is_one_of(std::initializer_list<PrayTokenType> list) const {
	for (auto t : list) {
		if (type == t) {
			return true;
		}
	}
	return false;
}

bool PrayToken::is_integer() const {
	return type == PRAY_INTEGER;
}

bool PrayToken::is_atsign() const {
	return type == PRAY_ATSIGN;
}

std::string format_as(const PrayToken& t) {
	return fmt::format("{} {:?}", t.type, t.raw_value);
}

auto format_as(PrayTokenType type) {
	switch (type) {
		case PRAY_BAREWORD: return "string";
		case PRAY_STRING: return "string";
		case PRAY_INTEGER: return "integer";
		case PRAY_ATSIGN: return "atsign";
		case PRAY_COMMENT: return "comment";
		case PRAY_WHITESPACE: return "whitespace";
		case PRAY_NEWLINE: return "newline";
		case PRAY_ERROR: return "lexer error";
		case PRAY_EOI: return "<eoi>";
	}
}

std::vector<PrayToken> praylex(const char* p) {
	std::vector<PrayToken> v;
	const char* basep;
	assert(p);
	int yylineno = 1;
	int baseyylineno;

#define push_value(type) \
	v.push_back(PrayToken(type, baseyylineno, std::string(basep, numeric_cast<std::string::size_type>(p - basep)))); \
	goto start;

#define push_value_and_error(type) \
	v.push_back(PrayToken(type, baseyylineno, std::string(basep, numeric_cast<std::string::size_type>(p - basep)))); \
	v.push_back(PrayToken(PRAY_ERROR, baseyylineno, "")); \
	goto start;

start:
	basep = p;
	baseyylineno = yylineno;

	if (p[0] == '\0') {
		goto eoi;
	} else if (p[0] == '(' && p[1] == '-') {
		p += 2;
		// TODO: do these comments allow embedded newlines?
		while (!(p[0] == '\0' || (p[0] == '-' && p[1] == ')'))) {
			p++;
		}
		if (p[0] == '\0') {
			push_value_and_error(PRAY_COMMENT);
		}
		p += 2;
		push_value(PRAY_COMMENT);
	} else if (p[0] == '\n') {
		p++;
		yylineno++;
		push_value(PRAY_NEWLINE);
	} else if (p[0] == '\r' && p[1] == '\n') {
		p += 2;
		yylineno++;
		push_value(PRAY_NEWLINE);
	} else if (p[0] == ' ' || p[0] == '\t' || (p[0] == '\r' && p[1] != '\n')) {
		p++;
		while (p[0] == ' ' || p[0] == '\t' || (p[0] == '\r' && p[1] != '\n')) {
			p++;
		}
		push_value(PRAY_WHITESPACE);
	} else if (is_ascii_digit(p[0]) || (p[0] == '-' && is_ascii_digit(p[1]))) {
		// normal numbers
		p++;
		while (is_ascii_digit(p[0])) {
			p++;
		}
		push_value(PRAY_INTEGER);
	} else if (p[0] == '"') {
		p++;
		goto str;
	} else if (p[0] == '@') {
		p++;
		push_value(PRAY_ATSIGN);
	} else if (is_ascii_alpha(p[0])) {
		p++;
		while (is_ascii_alpha(p[0])) {
			p++;
		}
		push_value(PRAY_BAREWORD);
	} else {
		p++;
		push_value(PRAY_ERROR);
	}

str:
	if (p[0] == '\0' || p[0] == '\r' || p[0] == '\n') {
		p++;
		push_value(PRAY_ERROR);
	} else if (p[0] == '\\') {
		p += 2;
		goto str;
	} else if (p[0] == '"') {
		p++;
		push_value(PRAY_STRING);
	} else {
		p++;
		goto str;
	}

eoi:
	v.push_back(PrayToken(PRAY_EOI, yylineno, "\0"));
	return v;
}

class PraySourceParserImpl {
  public:
	PraySourceParserImpl(const char* buf);
	std::vector<PraySourceParser::Event> run();
	std::vector<PraySourceParser::Event> next();

  private:
	std::vector<PrayToken> tokens;

	PrayToken peek_token() const;
	void next_token();
	PraySourceParser::Event parse_line();
	bool parse_some_ws();

	int yylineno = 1;
	size_t token_p = 0;
	PrayToken token;
	bool in_group_block = false;
	std::string last_group_type;
	std::string last_group_name;
	bool seen_en_gb = false;
};

PrayToken PraySourceParserImpl::peek_token() const {
	return tokens[token_p];
}

void PraySourceParserImpl::next_token() {
	token = tokens[token_p++];
	if (token.type == PRAY_NEWLINE) {
		yylineno++;
	}
}

PraySourceParserImpl::PraySourceParserImpl(const char* buf)
	: tokens(praylex(buf)), yylineno(1) {
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
		if (!seen_en_gb && token.type == PRAY_STRING && token.value() == "en-GB") {
			seen_en_gb = true;
			next_token();
		}
		if (token.type == PRAY_NEWLINE) {
			next_token();
		} else {
			break;
		}
	}
	if (!seen_en_gb) {
		return {Error{fmt::format("Expected \"en-GB\", got {}", token)}};
	}

	// generate synthetic "GroupBlock" lines
	if (in_group_block && (token.type == PRAY_BAREWORD || token.type == PRAY_EOI)) {
		in_group_block = false;
		return {GroupBlockEnd{last_group_type, last_group_name}};
	}

	// parse the actual line
	if (token.type == PRAY_EOI) {
		return {};
	}
	PraySourceParser::Event result = parse_line();
	if (result.has<Error>()) {
		return {result};
	}

	// parse ending whitespace/newline
	parse_some_ws();
	if (token.type == PRAY_EOI) {
		return {result};
	}
	if (token.type != PRAY_NEWLINE) {
		return {Error{fmt::format("Expected newline, got {}", token)}};
	}
	next_token();

	return {result};
}

PraySourceParser::Event PraySourceParserImpl::parse_line() {
	if (token.type == PRAY_ERROR) {
		return Error{format_as(token)};
	}

	if (token.type == PRAY_STRING) {
		std::string key = token.value();
		next_token();

		if (!parse_some_ws()) {
			return Error{fmt::format("Expected whitespace or newline after string, got {}", token)};
		}

		if (token.type == PRAY_STRING) {
			std::string value = token.value();
			next_token();
			return StringTag{key, value};
		} else if (token.is_atsign()) {
			next_token();
			if (!parse_some_ws()) {
				return Error{fmt::format("Expected whitespace after '@', got {}",
					token)};
			}
			if (token.type != PRAY_STRING) {
				return Error{fmt::format("Expected string after '@', got {}", token)};
			}
			std::string value = token.value();
			next_token();
			return StringTagFromFile{key, value};
		} else if (token.is_integer()) {
			int value = token.integer_value();
			next_token();
			return IntegerTag{key, value};
		} else {
			return Error{fmt::format("Expected string, integer, or '@', got {}", token)};
		}
	}

	if (token.type == PRAY_BAREWORD) {
		std::string directive = token.value();
		next_token();

		if (directive != "group" && directive != "inline") {
			return Error{fmt::format("Expected 'group' or 'inline', got {}", token)};
		}

		if (!parse_some_ws()) {
			return Error{fmt::format("Expected whitespace after '{}', got {}", directive, token)};
		}

		if (token.type != PRAY_BAREWORD) {
			return Error{fmt::format("Expected block type, got {}", token)};
		}
		std::string type = token.value();
		next_token();

		if (!parse_some_ws()) {
			return Error{"Expected whitespace after block type"};
		}

		if (directive == "group") {
			if (token.type != PRAY_STRING) {
				return Error{fmt::format("Expected block name, got {}", token)};
			}
			std::string name = token.value();
			next_token();
			// bookkeeping for generating "GroupBlockEnd"
			in_group_block = true;
			last_group_type = type;
			last_group_name = name;
			return GroupBlockStart{type, name};

		} else if (directive == "inline") {
			if (token.type != PRAY_STRING) {
				return Error{fmt::format("Expected block label, got {}", token)};
			}
			std::string label = token.value();
			next_token();

			if (!parse_some_ws())
				return Error{""};

			if (token.type != PRAY_STRING) {
				return Error{fmt::format("Expected quoted filename, got {}", token)};
			}
			std::string filename = token.value();
			next_token();

			return InlineBlock{type, label, filename};
		}
	}

	return Error{fmt::format("Expected string or bareword, got: {}", token)};
}

bool PraySourceParserImpl::parse_some_ws() {
	// parse whitespace or comment (but not newline)
	bool parsed_ws = false;
	while (true) {
		if (token.type == PRAY_WHITESPACE) {
			parsed_ws = true;
			next_token();
		} else if (token.type == PRAY_COMMENT) {
			next_token();
		} else {
			break;
		}
	}
	return parsed_ws;
}

std::vector<PraySourceParser::Event> PraySourceParser::parse(const std::string& str) {
	std::string s = ensure_utf8(str);
	return PraySourceParserImpl(s.c_str()).run();
}

std::string format_as(const Event& event) {
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
		return "???";
	}
}