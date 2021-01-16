#include "fileformats/Caos2PrayParser.h"
#include "fileformats/caoslexer.h"
#include "fileformats/caostoken.h"
#include "utils/encoding.h"
#include "utils/overload.h"

#include <cassert>
#include <regex>

static char unescape(char c) {
	switch (c) {
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		default: return c;
	}
}

static bool iequals(const std::string& a, const std::string& b) {
	return std::equal(a.begin(), a.end(), b.begin(), b.end(),
		[](char a, char b) { return std::tolower(a) == std::tolower(b); });
}

static bool imatch(const std::string& s, std::string regex) {
	return std::regex_search(s, std::regex(regex, std::regex::icase));
}

struct c2ptoken {
	enum commandtype {
		CMD_PRAY_FILE,
		CMD_DS_NAME,
		CMD_C3_NAME,
		CMD_ARBITRARY_NAME,
		CMD_DEPEND,
		CMD_INLINE,
		CMD_ATTACH,
		CMD_LINK,
		CMD_RSCR,
	};

	enum tokentype {
		TOK_EOI,
		TOK_ERROR,
		TOK_WS,
		TOK_BAREWORD,
		TOK_STRING,
		TOK_NEWLINE,
		TOK_ASSIGNMENT,
		TOK_COMMAND,
		TOK_INTEGER,
	};

	c2ptoken(tokentype type_)
		: type(type_) {}
	c2ptoken(tokentype type_, std::string val)
		: type(type_), stringval(val) {}
	c2ptoken(commandtype command_, std::string val)
		: type(TOK_COMMAND), command(command_), stringval(val) {}

	std::string asString() const {
		switch (type) {
			case TOK_EOI:
				return "TOK_EOI";
			case TOK_ERROR:
				return "TOK_ERROR";
			case TOK_WS:
				return "TOK_WS";
			case TOK_BAREWORD:
				return "TOK_BAREWORD: " + stringval;
			case TOK_STRING:
				return "TOK_STRING: '" + stringval + "'";
			case TOK_NEWLINE:
				return "TOK_NEWLINE";
			case TOK_ASSIGNMENT:
				return "TOK_ASSIGNMENT";
			case TOK_COMMAND:
				return "TOK_COMMAND: " + stringval;
			case TOK_INTEGER:
				return "TOK_INTEGER: " + stringval;
		}
	}

	tokentype type;
	commandtype command;
	std::string stringval;
};

class Caos2PrayLexer {
  public:
	Caos2PrayLexer(const char* p_)
		: p(p_) {}

	c2ptoken next_token() {
		assert(p);
		const char* basep = p;
		const char *YYMARKER, *YYCTXMARKER;
		std::string tempstr;

		/*!re2c
        re2c:define:YYCTYPE = "unsigned char";
        re2c:define:YYCURSOR = p;
        re2c:yyfill:enable = 0;
        re2c:yych:conversion = 1;

        [\000] { return c2ptoken(c2ptoken::TOK_EOI); }
        [\n] { return c2ptoken(c2ptoken::TOK_NEWLINE); }
        [ \t\r]+ { return c2ptoken(c2ptoken::TOK_WS); }
        'Pray-File' {
            return c2ptoken(c2ptoken::CMD_PRAY_FILE, std::string(basep, p -
        basep));
        }
        'DS-Name' {
            return c2ptoken(c2ptoken::CMD_DS_NAME, std::string(basep, p -
        basep));
        }
        'C3-Name' {
            return c2ptoken(c2ptoken::CMD_C3_NAME, std::string(basep, p -
        basep));
        }
        [A-Z]{4} '-Name' {
            return c2ptoken(c2ptoken::CMD_ARBITRARY_NAME, std::string(basep, p -
        basep));
        }
        'Depend' {
            return c2ptoken(c2ptoken::CMD_DEPEND, std::string(basep, p -
        basep));
        }
        'Inline' {
            return c2ptoken(c2ptoken::CMD_INLINE, std::string(basep, p -
        basep));
        }
        'Attach' {
            return c2ptoken(c2ptoken::CMD_ATTACH, std::string(basep, p -
        basep));
        }
        'Link' {
            return c2ptoken(c2ptoken::CMD_LINK, std::string(basep, p - basep));
        }
        'rscr' {
            return c2ptoken(c2ptoken::CMD_RSCR, std::string(basep, p - basep));
        }
        [0-9]+ {
            return c2ptoken(c2ptoken::TOK_INTEGER, std::string(basep, p -
        basep));
        }
        [A-Za-z0-9_\-\.] [A-Za-z0-9_\-\. ]* [A-Za-z0-9_\-\.] / ([ ]* "=") {
            return c2ptoken(c2ptoken::TOK_BAREWORD, std::string(basep, p -
        basep));
        }
        [A-Za-z0-9_\-\.]+ {
            return c2ptoken(c2ptoken::TOK_BAREWORD, std::string(basep, p -
        basep));
        }
        [=] { return c2ptoken(c2ptoken::TOK_ASSIGNMENT); }
        ["] { goto str; }
        . { return c2ptoken(c2ptoken::TOK_ERROR); }
        */
	str:
		/*!re2c
        [\\] . { tempstr.push_back(unescape(p[-1])); goto str; }
        ["] { return c2ptoken(c2ptoken::TOK_STRING, tempstr); }
        [\n] { yylineno++; tempstr.push_back(p[-1]); goto str; }
        [\000] { return c2ptoken(c2ptoken::TOK_ERROR);  }
        . { tempstr.push_back(p[-1]); goto str; }
        */
	}

	const char* p;
	int yylineno = 1;
};

using namespace PraySourceParser;

class Caos2PrayParserImpl {
  public:
	Caos2PrayParserImpl() {}

	int dependency_count = 0;
	int script_count = 1;
	bool in_rscr = false;
	std::string output_filename;
	std::string remove_script;

	void add_dependency_tags(std::vector<Event>* events,
		const std::string& a) {
		dependency_count++;
		events->push_back(StringTag{
			"Dependency " + std::to_string(dependency_count), a});
		std::string categorykey =
			"Dependency Category " + std::to_string(dependency_count);
		if (imatch(a, "\\.(mng|wav)$")) {
			events->push_back(IntegerTag{categorykey, 1});
		} else if (imatch(a, "\\.(c16|s16)$")) {
			events->push_back(IntegerTag{categorykey, 2});
		} else if (imatch(a, "\\.(gen|gno)$")) {
			events->push_back(IntegerTag{categorykey, 3});
		} else if (imatch(a, "\\.att$")) {
			events->push_back(IntegerTag{categorykey, 4});
		}
		// skip category 5, which is Overlay Data
		else if (imatch(a, "\\.blk$")) {
			events->push_back(IntegerTag{categorykey, 6});
		} else if (imatch(a, "\\.catalogue$")) {
			events->push_back(IntegerTag{categorykey, 7});
		} else {
			*events = {Error{"Unknown dependency category for '" + a}};
		}
	}

	std::vector<Event>
	handle_caos2pray_command(c2ptoken::commandtype type,
		const std::string& command,
		const std::vector<std::string>& args) {
		switch (type) {
			case c2ptoken::CMD_PRAY_FILE:
				if (args.size() != 1) {
					return {Error{"Command 'Pray-File' takes 1 argument, got " +
								  std::to_string(args.size())}};
				}
				if (output_filename.size() > 0) {
					return {Error{"Command 'Pray-File' used more than once"}};
				}
				output_filename = args[0];
				return {};
			case c2ptoken::CMD_DS_NAME:
				if (args.size() != 1) {
					return {Error{"Command 'DS-Name' takes 1 argument, got " +
								  std::to_string(args.size())}};
				}
				return {GroupBlockStart{"DSAG", args[0]}};
			case c2ptoken::CMD_C3_NAME:
				if (args.size() > 1) {
					return {Error{"Command 'C3-Name' takes 0 or 1 arguments, got " +
								  std::to_string(args.size())}};
				}
				if (args.size() == 1) {
					return {GroupBlockStart{"AGNT", args[0]}};
				}
				// TODO
				return {Error{"c3-name no name not implemented\n"}};
			case c2ptoken::CMD_ARBITRARY_NAME:
				if (args.size() != 1) {
					return {Error{"Command '" + command +
								  "' takes 1 argument, got " +
								  std::to_string(args.size())}};
				}
				return {GroupBlockStart{command.substr(0, 4), args[0]}};
			case c2ptoken::CMD_DEPEND: {
				std::vector<Event> events;
				for (auto a : args) {
					add_dependency_tags(&events, a);
				}
				return events;
			}
			case c2ptoken::CMD_INLINE:
				// TODO
				return {Error{"Command 'inline' not implemented\n"}};
			case c2ptoken::CMD_ATTACH: {
				std::vector<Event> events;
				for (auto a : args) {
					add_dependency_tags(&events, a);
					events.push_back(InlineBlock{"FILE", a, a});
				}
				return events;
			}
			case c2ptoken::CMD_LINK: {
				std::vector<Event> events;
				for (auto a : args) {
					script_count++;
					events.push_back(StringTagFromFile{
						"Script " + std::to_string(script_count), a});
				}
				return events;
			}
			case c2ptoken::CMD_RSCR:
				// TODO:
				return {Error{"Command 'rscr' not implemented\n"}};
		}
	}

	std::vector<Event> parse_caos2pray(const char* buf) {
		Caos2PrayLexer lexer(buf);
		c2ptoken t = lexer.next_token();

		// skip whitespace
		while (t.type == c2ptoken::TOK_WS) {
			t = lexer.next_token();
		}

		// the meat
		if (t.type == c2ptoken::TOK_BAREWORD) {
			std::string key = t.stringval;
			if (key == "anim")
				key = "Agent Animation String";
			else if (key == "anim file")
				key = "Agent Animation File";
			else if (key == "desc")
				key = "Agent Description";
			else if (key == "anim start")
				key = "Agent Sprite First Image";
			else if (key == "anim img")
				key = "Agent Sprite First Image";
			else if (key == "anim image")
				key = "Agent Sprite First Image";
			else if (key == "first image")
				key = "Agent Sprite First Image";
			else if (key == "bioenergy")
				key = "Agent Bioenergy Value";
			t = lexer.next_token();
			while (t.type == c2ptoken::TOK_WS) {
				t = lexer.next_token();
			}
			if (t.type != c2ptoken::TOK_ASSIGNMENT) {
				return {Error{"Expected '=', got " + t.asString()}};
			}
			t = lexer.next_token();
			while (t.type == c2ptoken::TOK_WS) {
				t = lexer.next_token();
			}

			Event e;
			if (t.type == c2ptoken::TOK_STRING) {
				e = StringTag{key, t.stringval};
			} else if (t.type == c2ptoken::TOK_INTEGER) {
				e = IntegerTag{key, atoi(t.stringval.c_str())};
			} else {
				return {
					Error{"Expected string or integer, got " + t.asString()}};
			}
			t = lexer.next_token();
			while (t.type == c2ptoken::TOK_WS) {
				t = lexer.next_token();
			}
			if (t.type != c2ptoken::TOK_EOI) {
				return {Error{"Expected end of line, got " + t.asString()}};
			}
			return {e};

		} else if (t.type == c2ptoken::TOK_COMMAND) {
			c2ptoken::commandtype type = t.command;
			std::string stringval = t.stringval;
			t = lexer.next_token();

			std::vector<std::string> args;

			while (true) {
				while (t.type == c2ptoken::TOK_WS) {
					t = lexer.next_token();
				}
				if (t.type == c2ptoken::TOK_BAREWORD ||
					t.type == c2ptoken::TOK_STRING) {
					args.push_back(t.stringval);
					t = lexer.next_token();
				} else if (t.type == c2ptoken::TOK_EOI) {
					return handle_caos2pray_command(type, stringval, args);
				} else {
					return {Error{"Expected string or bareword, got " +
								  t.asString()}};
				}
			}
		} else {
			return {Error{"Expected word, got " + t.asString()}};
		}
	}

	std::vector<Event> run(std::string script, std::string* output_filename_) {
		std::vector<caostoken> tokens;
		size_t p = 0;
		lexcaos(tokens, script.c_str());

		std::vector<Event> events;
		events.push_back(StringTag{"Script 1", std::move(script)});

		while (true) {
			if (tokens[p].type == caostoken::TOK_ERROR) {
				return {Error{"CAOS lexer error on line " +
							  std::to_string(tokens[p].lineno)}};
			}
			if (in_rscr) {
				if (tokens[p].type == caostoken::TOK_EOI) {
					events.push_back(StringTag{"Remove Script", remove_script});
					break;
				}
				if (tokens[p].type != caostoken::TOK_COMMENT) {
					// TODO: include comments and original whitespace
					remove_script += " " + tokens[p].format();
				}
			} else if (tokens[p].type == caostoken::TOK_COMMENT) {
				if (tokens[p].value[0] == '*' && tokens[p].value[1] == '#') {
					// parse caos2pray comment
					std::vector<Event> newevents =
						parse_caos2pray(tokens[p].value.c_str() + 2);
					for (auto e : newevents) {
						if (mpark::holds_alternative<Error>(e)) {
							return {e};
						}
					}
					events.insert(events.end(), newevents.begin(),
						newevents.end());
				}
			} else if (tokens[p].type == caostoken::TOK_EOI) {
				break;
			} else if (tokens[p].type == caostoken::TOK_WORD) {
				if (iequals(tokens[p].value, "rscr")) {
					in_rscr = true;
				}
			}
			p++;
		}

		std::vector<GroupBlockStart> group_blocks;
		std::vector<Event> inline_blocks;
		std::vector<Event> tags;

		tags.push_back(IntegerTag{"Dependency Count", dependency_count});
		tags.push_back(IntegerTag{"Script Count", script_count});

		for (auto& e : events) {
			visit(overload(
					  [&](const GroupBlockStart& g) { group_blocks.push_back(g); },
					  [&](const InlineBlock&) { inline_blocks.push_back(e); },
					  [&](const StringTag&) { tags.push_back(e); },
					  [&](const StringTagFromFile&) { tags.push_back(e); },
					  [&](const IntegerTag&) { tags.push_back(e); },
					  [](const GroupBlockEnd&) { abort(); }, // not created yet
					  [](const Error&) { abort(); } // handled already
					  ),
				e);
		}

		if (group_blocks.size() == 0 && inline_blocks.size() == 0) {
			return {Error{"No PRAY blocks defined"}};
		}

		events = {};
		for (auto& g : group_blocks) {
			events.push_back(g);
			for (auto& t : tags) {
				events.push_back(t);
			}
			events.push_back(GroupBlockEnd{g.type, g.name});
		}
		events.insert(events.end(), inline_blocks.begin(), inline_blocks.end());

		if (output_filename.size() > 0 && output_filename_)
			*output_filename_ = output_filename;

		return events;
	}
};

std::vector<Event> Caos2PrayParser::parse(const std::string& script, std::string* output_filename) {
	return Caos2PrayParserImpl().run(ensure_utf8(script).c_str(), output_filename);
}
