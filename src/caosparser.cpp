#include "caosparser.h"
#include "dialect.h"
#include "noncopyable.h"
#include "stringutil.h"
#include <ctype.h>
#include <fmt/core.h>

struct CAOSParserState : noncopyable {
    CAOSParserState(const std::vector<token>& tokens_, Dialect *dialect_) : tokens(tokens_), dialect(dialect_) {}
    const std::vector<token>& tokens;
    Dialect *dialect;
    size_t p = 0;
};

static bool current_token_is(const CAOSParserState& state, std::initializer_list<token::toktype> types) {
    for (auto t : types) {
        if (state.tokens[state.p].type == t) {
            return true;
        }
    }
    return false;
}

static void assert_current_token_is(const CAOSParserState& state, std::initializer_list<token::toktype> types) {
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
    throw creaturesException(fmt::format("Expected tokentype {}, got {}", expected, state.tokens[state.p].type));
}

static bool maybe_eat_whitespace(CAOSParserState& state) {
    bool ate_whitespace = false;
    while (current_token_is(state, {token::TOK_WHITESPACE})) {
        ate_whitespace = true;
        state.p += 1;
    }
    return ate_whitespace;
}

static void eat_whitespace(CAOSParserState& state) {
    if (!maybe_eat_whitespace(state)) {
        throw creaturesException(fmt::format("Expected whitespace, got {} {}", state.tokens[state.p].typeAsString(), state.tokens[state.p].format()));
    }
}

static CAOSNodePtr parse_command(CAOSParserState& state, bool is_toplevel);

static CAOSNodePtr parse_value(CAOSParserState& state) {
    maybe_eat_whitespace(state);
    
    switch (state.tokens[state.p].type) {
        case token::TOK_WORD:
            return parse_command(state, false);
        case token::TOK_BYTESTR:
        case token::TOK_STRING:
        case token::TOK_CHAR:
        case token::TOK_BINARY:
        case token::TOK_INT:
        case token::TOK_FLOAT:
            return CAOSNodePtr{new CAOSLiteralValueNode{state.tokens[state.p++]}};
        case token::TOK_COMMENT:
        case token::TOK_WHITESPACE:
        case token::TOK_NEWLINE:
        case token::TOK_COMMA:
        case token::TOK_EOI:
        case token::TOK_ERROR:
            throw creaturesException(fmt::format("Expected value, got {} {}", state.tokens[state.p].typeAsString(), state.tokens[state.p].format()));
    }
}

static CAOSNodePtr parse_condition(CAOSParserState &state) {
    auto left = parse_value(state);
    eat_whitespace(state);

    assert_current_token_is(state, {token::TOK_WORD});
    auto comparison = state.tokens[state.p].value;
    state.p += 1;

    // TODO: are all of these allowed in C1/C2 ?
    if (!string_in(lowerstring(comparison), {
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
    })) {
        throw creaturesException(fmt::format("Unknown comparison operator '{}'", comparison));
    }
    eat_whitespace(state);

    auto right = parse_value(state);

    std::vector<CAOSNodePtr> args;
    args.push_back(left);
    args.push_back(CAOSNodePtr(new CAOSLiteralWordNode(comparison)));
    args.push_back(right);

    bool ate_whitespace = maybe_eat_whitespace(state);
    if (
        ate_whitespace
        && string_in(lowerstring(state.tokens[state.p].value), {"and", "or"})
    ) {
        auto combiner = state.tokens[state.p].value;
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
        assert_current_token_is(state, { token::TOK_WORD });

        std::string command = lowerstring(state.tokens[state.p].value);
        std::string commandnormalized = command;
        // TODO: <regex> makes compile time slow. migrate to something like RE2?
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
        if (commandinfo == nullptr) {
            throw creaturesException(fmt::format("No such {} command '{}'", is_toplevel ? "toplevel" : "expression", command));
        }
        state.p += 1;

        // some commands are namespace placeholders
        if (commandinfo->argc == 1 && commandinfo->argtypes[0] == CI_SUBCOMMAND) {
            eat_whitespace(state);
            command = command + " " + lowerstring(state.tokens[state.p].value);
            std::string lookup_key = (is_toplevel ? "cmd " : "expr ") + command;
            commandinfo = state.dialect->find_command(lookup_key);
            if (commandinfo == nullptr) {
                throw creaturesException(fmt::format("No such {} command '{}'", is_toplevel ? "toplevel" : "expression", command));
            }
            state.p += 1;
        } else {
            // some commands are two words but aren't namespaces!
            // C1 has a bunch of commands like SETV PUHL (integer) (integer) (integer)
            auto set_p = state.p;
            if (maybe_eat_whitespace(state) && current_token_is(state, {token::TOK_WORD})) {
                std::string newcommand = command + " " + lowerstring(state.tokens[state.p].value);
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

        std::vector<CAOSNodePtr> args;
        for (int i = 0; i < commandinfo->argc; ++i) {
            eat_whitespace(state);
            switch (commandinfo->argtypes[i]) {
                case CI_OTHER:
                case CI_COMMAND:
                case CI_SUBCOMMAND:
                    throw creaturesException(fmt::format("unhandled argument type {}", commandinfo->argtypes[i]));
                case CI_VARIABLE:
                    args.push_back(parse_command(state, false));
                    break;
                case CI_NUMERIC:
                case CI_STRING:
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
                    assert_current_token_is(state, {token::TOK_WORD});
                    args.push_back(CAOSNodePtr(new CAOSLiteralWordNode(state.tokens[state.p].value)));
                    state.p += 1;
                    break;
            }
        }

        return CAOSNodePtr{new CAOSCommandNode{
            command,
            commandinfo->rettype,
            args
        } };
}

std::vector<CAOSNodePtr> parse(const std::vector<token>& tokens, Dialect* dialect) {
    CAOSParserState state{ tokens, dialect };
    std::vector<CAOSNodePtr> toplevel;
    while (true) {
        while (current_token_is(state, {token::TOK_WHITESPACE, token::TOK_NEWLINE, token::TOK_COMMENT, token::TOK_COMMA })) {
            state.p += 1;
        }
        if (state.tokens[state.p].type == token::TOK_EOI) {
            break;
        }
        toplevel.push_back(parse_command(state, true));
    }
    return toplevel;
}

template <>
struct fmt::formatter<ci_type> : public formatter<string_view> {
    template <typename FormatContext>
    auto format(const ci_type& ci, FormatContext& ctx) {
        std::string name;
        switch (ci) {
            case CI_OTHER: name = "CI_OTHER"; break;
            case CI_COMMAND: name = "CI_COMMAND"; break;
            case CI_NUMERIC: name = "CI_NUMERIC"; break;
            case CI_STRING: name = "CI_STRING"; break;
            case CI_AGENT: name = "CI_AGENT"; break;
            case CI_VARIABLE: name = "CI_VARIABLE"; break;
            case CI_BYTESTR: name = "CI_BYTESTR"; break;
            case CI_VECTOR: name = "CI_VECTOR"; break;
            case CI_BAREWORD: name = "CI_BAREWORD"; break;
            case CI_SUBCOMMAND: name = "CI_SUBCOMMAND"; break;
            case CI_ANYVALUE: name = "CI_ANYVALUE"; break;
            case CI_CONDITION: name = "CI_CONDITION"; break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<token::toktype> : public formatter<string_view> {
    template <typename FormatContext>
    auto format(const token::toktype& t, FormatContext& ctx) {
        std::string name;
        switch (t) {
            case token::TOK_WORD: name = "TOK_WORD"; break;
            case token::TOK_BYTESTR: name = "TOK_BYTESTR"; break;
            case token::TOK_STRING: name = "TOK_STRING"; break;
            case token::TOK_CHAR: name = "TOK_CHAR"; break;
            case token::TOK_BINARY: name = "TOK_BINARY"; break;
            case token::TOK_INT: name = "TOK_INT"; break;
            case token::TOK_FLOAT: name = "TOK_FLOAT"; break;
            case token::TOK_COMMENT: name = "TOK_COMMENT"; break;
            case token::TOK_WHITESPACE: name = "TOK_WHITESPACE"; break;
            case token::TOK_NEWLINE: name = "TOK_NEWLINE"; break;
            case token::TOK_COMMA: name = "TOK_COMMA"; break;
            case token::TOK_EOI: name = "TOK_EOI"; break;
            case token::TOK_ERROR: name = "TOK_ERROR"; break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};