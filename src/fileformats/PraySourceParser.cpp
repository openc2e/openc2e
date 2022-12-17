#include "PraySourceParser.h"

#include "common/SimpleLexer.h"
#include "common/encoding.h"

#include <cassert>
#include <fmt/format.h>
#include <iostream>
#include <map>
#include <string>

using namespace PraySourceParser;

class PraySourceParserImpl {
  public:
	PraySourceParserImpl(const char* buf);
	std::vector<PraySourceParser::Event> run();
	std::vector<PraySourceParser::Event> next();

  private:
	std::vector<SimpleToken> tokens;

	SimpleToken peek_token() const;
	void next_token();
	PraySourceParser::Event parse_line();
	bool parse_some_ws();

	int yylineno = 1;
	size_t token_p = 0;
	SimpleToken token;
	bool in_group_block = false;
	std::string last_group_type;
	std::string last_group_name;
	bool seen_en_gb = false;
};

SimpleToken PraySourceParserImpl::peek_token() const {
	return tokens[token_p];
}

void PraySourceParserImpl::next_token() {
	token = tokens[token_p++];
	if (token.type == SIMPLE_NEWLINE) {
		yylineno++;
	}
}

PraySourceParserImpl::PraySourceParserImpl(const char* buf)
	: tokens(simplelex(buf)), yylineno(1) {
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
		if (!seen_en_gb && token.type == SIMPLE_STRING && token.value() == "en-GB") {
			seen_en_gb = true;
			next_token();
		}
		if (token.type == SIMPLE_NEWLINE) {
			next_token();
		} else {
			break;
		}
	}
	if (!seen_en_gb) {
		return {Error{"Expected \"en-GB\", got " + token.repr()}};
	}

	// generate synthetic "GroupBlock" lines
	if (in_group_block && (token.type == SIMPLE_BAREWORD || token.type == SIMPLE_EOI)) {
		in_group_block = false;
		return {GroupBlockEnd{last_group_type, last_group_name}};
	}

	// parse the actual line
	if (token.type == SIMPLE_EOI) {
		return {};
	}
	PraySourceParser::Event result = parse_line();
	if (result.has<Error>()) {
		return {result};
	}

	// parse ending whitespace/newline
	parse_some_ws();
	if (token.type == SIMPLE_EOI) {
		return {result};
	}
	if (token.type != SIMPLE_NEWLINE) {
		return {Error{"Expected newline, got " + token.repr()}};
	}
	next_token();

	return {result};
}

PraySourceParser::Event PraySourceParserImpl::parse_line() {
	if (token.type == SIMPLE_ERROR) {
		return Error{token.repr()};
	}

	if (token.type == SIMPLE_STRING) {
		std::string key = token.value();
		next_token();

		if (!parse_some_ws()) {
			return Error{"Expected whitespace or newline after string, got " +
						 token.repr()};
		}

		if (token.type == SIMPLE_STRING) {
			std::string value = token.value();
			next_token();
			return StringTag{key, value};
		} else if (token.is_symbol('@')) {
			next_token();
			if (!parse_some_ws()) {
				return Error{"Expected whitespace after '@', got " +
							 token.repr()};
			}
			if (token.type != SIMPLE_STRING) {
				return Error{"Expected string after '@', got " + token.repr()};
			}
			std::string value = token.value();
			next_token();
			return StringTagFromFile{key, value};
		} else if (token.is_integer()) {
			int value = token.integer_value();
			next_token();
			return IntegerTag{key, value};
		} else {
			return Error{"Expected string, integer, or '@', got " +
						 token.repr()};
		}
	}

	if (token.type == SIMPLE_BAREWORD) {
		std::string directive = token.value();
		next_token();

		if (directive != "group" && directive != "inline") {
			return Error{"Expected 'group' or 'inline', got " + token.repr()};
		}

		if (!parse_some_ws()) {
			return Error{"Expected whitespace after '" + directive + "', got " +
						 token.repr()};
		}

		if (token.type != SIMPLE_BAREWORD) {
			return Error{"Expected block type, got " + token.repr()};
		}
		std::string type = token.value();
		next_token();

		if (!parse_some_ws()) {
			return Error{"Expected whitespace after block type"};
		}

		if (directive == "group") {
			if (token.type != SIMPLE_STRING) {
				return Error{"Expected block name, got " + token.repr()};
			}
			std::string name = token.value();
			next_token();
			// bookkeeping for generating "GroupBlockEnd"
			in_group_block = true;
			last_group_type = type;
			last_group_name = name;
			return GroupBlockStart{type, name};

		} else if (directive == "inline") {
			if (token.type != SIMPLE_STRING) {
				return Error{"Expected block label, got " + token.repr()};
			}
			std::string label = token.value();
			next_token();

			if (!parse_some_ws())
				return Error{""};

			if (token.type != SIMPLE_STRING) {
				return Error{"Expected quoted filename, got " + token.repr()};
			}
			std::string filename = token.value();
			next_token();

			return InlineBlock{type, label, filename};
		}
	}

	return Error{"Expected string or bareword, got: " + token.repr()};
}

bool PraySourceParserImpl::parse_some_ws() {
	// parse whitespace or comment (but not newline)
	bool parsed_ws = false;
	while (true) {
		if (token.type == SIMPLE_WHITESPACE) {
			parsed_ws = true;
			next_token();
		} else if (token.is_symbol('(') && peek_token().is_symbol('*')) {
			token_p += 2;
			while (true) {
				if (token.is_symbol('*') && peek_token().is_symbol('}')) {
					token_p += 2;
					break;
				} else if (token.type == SIMPLE_ERROR || token.type == SIMPLE_EOI) {
					break;
				} else {
					next_token();
				}
			}
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