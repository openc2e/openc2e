#include "caosparser.h"

#include "common/Ascii.h"
#include "common/Exception.h"
#include "common/Ranges.h"
#include "dialect.h"

#include <ctype.h>
#include <fmt/format.h>

struct CAOSParserState {
	CAOSParserState(const std::vector<caostoken>& tokens_, Dialect* dialect_)
		: tokens(tokens_), dialect(dialect_) {}
	CAOSParserState(const CAOSParserState&) = delete;
	CAOSParserState& operator=(const CAOSParserState&) = delete;
	const std::vector<caostoken>& tokens;
	Dialect* dialect;
	size_t p = 0;
};

static bool current_token_is(const CAOSParserState& state, std::initializer_list<caostoken::toktype> types) {
	for (auto t : types) {
		if (state.tokens[state.p].type == t) {
			return true;
		}
	}
	return false;
}

static void assert_current_token_is(const CAOSParserState& state, std::initializer_list<caostoken::toktype> types) {
	for (auto t : types) {
		if (state.tokens[state.p].type == t) {
			return;
		}
	}
	std::string expected;
	for (unsigned int i = 0; i < types.size() - 1; ++i) {
		expected += fmt::format("{}, ", *(types.begin() + i));
	}
	if (expected.size() > 0) {
		expected += "or ";
	}
	expected += fmt::format("{}", *(types.end() - 1));
	throw Exception(fmt::format("Expected tokentype {}, got {}", expected, state.tokens[state.p].type));
}

static bool maybe_eat_whitespace(CAOSParserState& state) {
	bool ate_whitespace = false;
	while (current_token_is(state, {caostoken::TOK_WHITESPACE})) {
		ate_whitespace = true;
		state.p += 1;
	}
	return ate_whitespace;
}

static void eat_whitespace(CAOSParserState& state) {
	if (!maybe_eat_whitespace(state)) {
		throw Exception(fmt::format("Expected whitespace, got {} {:?}", state.tokens[state.p].typeAsString(), state.tokens[state.p].data));
	}
}

static CAOSNodePtr parse_command(CAOSParserState& state, bool is_toplevel);

static CAOSNodePtr parse_value(CAOSParserState& state) {
	maybe_eat_whitespace(state);

	switch (state.tokens[state.p].type) {
		case caostoken::TOK_WORD:
			return parse_command(state, false);
		case caostoken::TOK_BYTESTR:
		case caostoken::TOK_STRING:
		case caostoken::TOK_CHAR:
		case caostoken::TOK_BINARY:
		case caostoken::TOK_INT:
		case caostoken::TOK_FLOAT:
			return CAOSNodePtr{new CAOSLiteralValueNode{state.tokens[state.p++]}};
		case caostoken::TOK_COMMENT:
		case caostoken::TOK_WHITESPACE:
		case caostoken::TOK_NEWLINE:
		case caostoken::TOK_COMMA:
		case caostoken::TOK_EOI:
		case caostoken::TOK_ERROR:
			throw Exception(fmt::format("Expected value, got {} {:?}", state.tokens[state.p].typeAsString(), state.tokens[state.p].data));
	}
}

static CAOSNodePtr parse_condition(CAOSParserState& state) {
	auto left = parse_value(state);
	eat_whitespace(state);

	assert_current_token_is(state, {caostoken::TOK_WORD});
	auto comparison = state.tokens[state.p].data;
	state.p += 1;

	// TODO: are all of these allowed in C1/C2 ?
	if (!contains({
					  "eq",
					  "ne",
					  "gt",
					  "ge",
					  "lt",
					  "le",
					  "=",
					  "<>",
					  ">",
					  ">=",
					  "<",
					  "<=",
				  },
			to_ascii_lowercase(comparison))) {
		throw Exception(fmt::format("Unknown comparison operator '{}'", comparison));
	}
	eat_whitespace(state);

	auto right = parse_value(state);

	std::vector<CAOSNodePtr> args;
	args.push_back(left);
	args.push_back(CAOSNodePtr(new CAOSLiteralWordNode(comparison)));
	args.push_back(right);

	bool ate_whitespace = maybe_eat_whitespace(state);
	if (
		ate_whitespace && contains({"and", "or"}, to_ascii_lowercase(state.tokens[state.p].data))) {
		auto combiner = state.tokens[state.p].data;
		state.p += 1;
		auto remainder = parse_condition(state);

		args.push_back(CAOSNodePtr(new CAOSLiteralWordNode(combiner)));
		for (auto r : ((CAOSConditionNode*)remainder.get())->args) {
			args.push_back(r);
		}
	}
	return CAOSNodePtr(new CAOSConditionNode(args));
}

static CAOSNodePtr parse_command(CAOSParserState& state, bool is_toplevel) {
	assert_current_token_is(state, {caostoken::TOK_WORD});

	// find a command from the first token
	std::string command = to_ascii_lowercase(state.tokens[state.p].data);
	std::string commandnormalized = command;
	if (command.size() == 4 && command[0] == 'o' && command[1] == 'b' && command[2] == 'v' && isdigit(command[3])) {
		commandnormalized = "obvx";
	} else if (command.size() == 4 && command[0] == 'v' && command[1] == 'a' && command[2] == 'r' && isdigit(command[3])) {
		commandnormalized = "varx";
	} else if (command.size() == 4 && command[0] == 'v' && command[1] == 'a' && isdigit(command[2]) && isdigit(command[3])) {
		commandnormalized = "vaxx";
	} else if (command.size() == 4 && command[0] == 'o' && command[1] == 'v' && isdigit(command[2]) && isdigit(command[3])) {
		commandnormalized = "ovxx";
	} else if (command.size() == 4 && command[0] == 'm' && command[1] == 'v' && isdigit(command[2]) && isdigit(command[3])) {
		commandnormalized = "mvxx";
	}
	std::string lookup_key = (is_toplevel ? "cmd " : "expr ") + commandnormalized;
	auto commandinfo = state.dialect->find_command(lookup_key);
	state.p += 1;

	// some commands are namespace placeholders
	if (commandinfo && commandinfo->argc == 1 && commandinfo->argtypes[0] == CI_SUBCOMMAND) {
		eat_whitespace(state);
		command = command + " " + to_ascii_lowercase(state.tokens[state.p].data);
		std::string lookup_key = (is_toplevel ? "cmd " : "expr ") + command;
		commandinfo = state.dialect->find_command(lookup_key);
		if (commandinfo == nullptr) {
			throw Exception(fmt::format("No such {} command '{}'", is_toplevel ? "toplevel" : "expression", command));
		}
		state.p += 1;
	}
	// some commands are two words but aren't namespaces, and may even overlap
	// with single-word commands!
	// C1 has a bunch of commands like SETV PUHL (integer) (integer) (integer)
	else {
		auto set_p = state.p;
		if (maybe_eat_whitespace(state) && current_token_is(state, {caostoken::TOK_WORD})) {
			std::string newcommand = command + " " + to_ascii_lowercase(state.tokens[state.p].data);
			std::string lookup_key = (is_toplevel ? "cmd " : "expr ") + newcommand;
			auto newcommandinfo = state.dialect->find_command(lookup_key);
			if (newcommandinfo) {
				command = newcommand;
				commandinfo = newcommandinfo;
				set_p = state.p + 1;
			}
		}
		state.p = set_p;
	}

	// whoops, didn't find a single-word command or double-word command
	if (commandinfo == nullptr) {
		throw Exception(fmt::format("No such {} command '{}'", is_toplevel ? "toplevel" : "expression", commandnormalized));
	}

	// parse the arguments
	std::vector<CAOSNodePtr> args;
	for (int i = 0; i < commandinfo->argc; ++i) {
		if (i == 0 && commandnormalized == "anim" && state.dialect->name == "c1") {
			// The learning computer and cloud butterflies have scripts that go
			// `anim[0123]` with no whitespace separating the ANIM command and
			// the string argument. If we're followed directly by the string
			// bracket, skip trying to parse a separator.
			maybe_eat_whitespace(state);
		} else {
			eat_whitespace(state);
		}
		switch (commandinfo->argtypes[i]) {
			case CI_OTHER:
			case CI_COMMAND:
			case CI_SUBCOMMAND:
				throw Exception(fmt::format("unhandled argument type {}", commandinfo->argtypes[i]));
			case CI_VARIABLE:
				args.push_back(parse_command(state, false));
				break;
			case CI_STRING:
			case CI_FACEVALUE:
			case CI_NUMERIC:
			case CI_AGENT:
			case CI_BYTESTR:
			case CI_VECTOR:
			case CI_ANYVALUE:
				args.push_back(parse_value(state));
				break;
			case CI_CONDITION:
				args.push_back(parse_condition(state));
				break;
			case CI_BAREWORD:
				assert_current_token_is(state, {caostoken::TOK_WORD});
				args.push_back(CAOSNodePtr(new CAOSLiteralWordNode(state.tokens[state.p].data)));
				state.p += 1;
				break;
		}
	}

	return CAOSNodePtr{new CAOSCommandNode{
		command,
		commandinfo->rettype,
		args}};
}

std::vector<CAOSNodePtr> parse(const std::vector<caostoken>& tokens, Dialect* dialect) {
	CAOSParserState state{tokens, dialect};
	std::vector<CAOSNodePtr> toplevel;
	while (true) {
		while (current_token_is(state, {caostoken::TOK_WHITESPACE, caostoken::TOK_NEWLINE, caostoken::TOK_COMMENT, caostoken::TOK_COMMA})) {
			state.p += 1;
		}
		if (state.tokens[state.p].type == caostoken::TOK_EOI) {
			break;
		}
		toplevel.push_back(parse_command(state, true));
	}
	return toplevel;
}

auto format_as(const ci_type& ci) {
	switch (ci) {
		case CI_OTHER: return "CI_OTHER";
		case CI_COMMAND: return "CI_COMMAND";
		case CI_NUMERIC: return "CI_NUMERIC";
		case CI_STRING: return "CI_STRING";
		case CI_AGENT: return "CI_AGENT";
		case CI_VARIABLE: return "CI_VARIABLE";
		case CI_BYTESTR: return "CI_BYTESTR";
		case CI_FACEVALUE: return "CI_FACEVALUE";
		case CI_VECTOR: return "CI_VECTOR";
		case CI_BAREWORD: return "CI_BAREWORD";
		case CI_SUBCOMMAND: return "CI_SUBCOMMAND";
		case CI_ANYVALUE: return "CI_ANYVALUE";
		case CI_CONDITION: return "CI_CONDITION";
	}
}

auto format_as(const caostoken::toktype& t) {
	switch (t) {
		case caostoken::TOK_WORD: return "TOK_WORD";
		case caostoken::TOK_BYTESTR: return "TOK_BYTESTR";
		case caostoken::TOK_STRING: return "TOK_STRING";
		case caostoken::TOK_CHAR: return "TOK_CHAR";
		case caostoken::TOK_BINARY: return "TOK_BINARY";
		case caostoken::TOK_INT: return "TOK_INT";
		case caostoken::TOK_FLOAT: return "TOK_FLOAT";
		case caostoken::TOK_COMMENT: return "TOK_COMMENT";
		case caostoken::TOK_WHITESPACE: return "TOK_WHITESPACE";
		case caostoken::TOK_NEWLINE: return "TOK_NEWLINE";
		case caostoken::TOK_COMMA: return "TOK_COMMA";
		case caostoken::TOK_EOI: return "TOK_EOI";
		case caostoken::TOK_ERROR: return "TOK_ERROR";
	}
}