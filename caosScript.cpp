/*
 *  caosScript.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed May 26 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "caosVM.h"
#include "openc2e.h"
#include "World.h"
#include <iostream>
#include <sstream>
#include <algorithm>

std::string token::dump() {
	std::string src;
	if (type == CAOSVAR) return var.dump();
	else if (type == BYTESTRING) return "[" + var.stringValue + "] "; // TODO
	else if (type == POSSIBLEFUNC) src = data + " ";
	else if (type == LABEL) return data + " ";
	else if (type == FUNCTION) src = func->name + " "; // TODO
	else if (type == COMPARISON) {
		switch (comparison) {
			case EQ: return "eq ";
			case NE: return "ne ";
			case GT: return "gt ";
			case GE: return "ge ";
			case LT: return "lt ";
			case LE: return "le ";
			case AND: return "and ";
			case OR: return "or ";
			default: return "[bad comparison!] ";
		}
	} else {
		return "[bad token!] ";
	}
	std::transform(src.begin(), src.end(), src.begin(), tolower);	
	return src;
}

std::string cmdinfo::dump() {
	return std::string("[command: ") + name + "] ";
}

std::string script::dumpLine(unsigned int i) {
	std::string out;
	for (std::list<token>::iterator j = lines[i].begin(); j != lines[i].end(); j++) {
		out += j->dump();
	}
	return out;
}	

std::string script::dump() {
	std::string out;
	for (unsigned int i = 0; i < lines.size(); i++) {
		out = out + dumpLine(i) + "\n";
	}
	return out;
}

std::string caosScript::dump() {
	std::string out = "installation script:\n" + installer.dump();
	out += "\nremoval script:\n" + removal.dump();
	for (std::vector<script>::iterator i = scripts.begin(); i != scripts.end(); i++) {
		out += "\nagent script:\n" + i->dump();
	}
	return out;
}

// TODO: debug use only?
std::map<std::string, bool> seenbadsymbols;

token makeToken(std::string &src) {
	static std::string t;
	token r;

	// check for labels; easy enough, they only appear in certain places
	if (!t.empty())
		if ((t == "GSUB") || (t == "SUBR")) {
			t.clear();
			// first character *must* be a letter
			if (!isalpha(src[0])) throw tokeniseFailure("wanted a label, but first character isn't alpha");
			// handle a label
			r.type = token::LABEL;
			r.data = src;
			return r;
		}
	t.clear();
	
	// handle strings
	if (src[0] == '"') {
		r.type = token::CAOSVAR;
		src.erase(src.begin());
		src.erase(src.end() - 1);
		r.var.setString(src);
	// handle bytestrings
	} else if (src[0] == '[') {
		r.type = token::BYTESTRING;
		// TODO: handle this!
		src.erase(src.begin());
		src.erase(src.end() - 1);
		r.var.setString(src);
	// handle numeric types
	} else if ((isdigit(src[0])) || (src[0] == '.') || (src[0] == '-')) {
		r.type = token::CAOSVAR;
		if (src.find(".") != std::string::npos) {
			float f = atof(src.c_str());
			r.var.setFloat(f);
		} else {
			int v = atoi(src.c_str());
			r.var.setInt(v);
		}
		// TODO: check for zero return value from atoi (and atof?) and then check for EINVAL
	// handle possible functions
	} else if (src.size() == 4) {
		r.type = token::POSSIBLEFUNC;
		std::transform(src.begin(), src.end(), src.begin(), toupper);
		r.data = src;
		t = src;
	// handle comparisons (our last possibility)
	} else {
		std::transform(src.begin(), src.end(), src.begin(), toupper);
		r.type = token::COMPARISON;
		// todo: make this a hash table?
		if (src == "NE") r.comparison = NE;
		else if (src == "EQ") r.comparison = EQ;
		else if (src == "GE") r.comparison = GE;
		else if (src == "GT") r.comparison = GT;
		else if (src == "LE") r.comparison = LE;
		else if (src == "LT") r.comparison = LT;
		else if (src == "<>") r.comparison = NE;
		else if (src == "=") r.comparison = EQ;
		else if (src == ">=") r.comparison = GE;
		else if (src == ">") r.comparison = GT;
		else if (src == "<=") r.comparison = LE;
		else if (src == "<") r.comparison = LT;
		else if (src == "AND") r.comparison = AND;
		else if (src == "OR") r.comparison = OR;
		else throw tokeniseFailure("couldn't parse this token");
	}
			
	return r;
}

void tokeniseLine(std::string s, std::vector<token> &t) {
	std::string currtoken;
	token lasttoken;
	for (std::string::iterator i = s.begin(); i != s.end(); i++) {
		if ((*i == ' ') || (*i == '\t') || (*i == '\r')) {
			if (!currtoken.empty()) {
				t.push_back(makeToken(currtoken));
				currtoken.clear();
			}
		} else if (*i == '"') {
			if (!currtoken.empty()) throw tokeniseFailure("can't have \" in the middle of a token");
			currtoken += *i;
			i++;
			currtoken += *i;
			while (*i != '"') {
				i++;
				currtoken += *i;
				if (i == s.end()) throw tokeniseFailure("string parsing reached EOL");
			}
			t.push_back(makeToken(currtoken));
			currtoken.clear();
		} else if (*i == '[') {
			if (!currtoken.empty()) throw tokeniseFailure("can't have [ in the middle of a token");
			currtoken += *i;
			while (*i != ']') {
				i++;
				currtoken += *i;
				if (i == s.end()) throw tokeniseFailure("bytestring parsing reached EOL");
			}
			t.push_back(makeToken(currtoken));
			currtoken.clear();
		} else if (*i == '*') {
			// start of a comment. forget the rest of the line.
			i = s.end() - 1;
		} else {
			currtoken += *i;
		}
	}
	if (!currtoken.empty()) {
		t.push_back(makeToken(currtoken));
	}
}

// TODO: we just blindly increase loc here, without checking if we have enough tokens to cope
void slurpTokens(unsigned int &loc, std::vector<token> &tokens, std::list<token> &destline, bool func = false) {
	if (tokens[loc].type != token::POSSIBLEFUNC) throw tokeniseFailure("Expected a command or function");
	
	cmdinfo *c = getCmdInfo(tokens[loc].data, !func);
	if (!c) {
		if (func) throw tokeniseFailure("Expected a function");
		else throw tokeniseFailure("Expected a command");
	}

	// TODO: nicer error handling here (ie, work out functions/commands)
	if (c->twotokens) {
		loc++;
		if (tokens[loc].type != token::POSSIBLEFUNC) {
			loc--;
			throw tokeniseFailure("Expected a command or function");
		}
		c = getSecondCmd(c, tokens[loc].data, !func);
		if (!c) {
			loc--;
			// this makes the error output 'nice'
			tokens[loc].data = tokens[loc].data + " " + tokens[loc + 1].data;
			tokens.erase(tokens.begin() + loc + 1);
			throw tokeniseFailure("Expected a command or function");
		}
	}

	token us;
	us.type = token::FUNCTION;
	us.func = c;
	us.varnumber = varnumber; // global hack for VAxx/OVxx etc (-1 if none)!
	destline.push_back(us);

	if (c->needscondition) {
		bool looping = true;
		while (looping) {
			loc++;
			if (tokens[loc].type == token::POSSIBLEFUNC)
				slurpTokens(loc, tokens, destline, true);
			else
				destline.push_back(tokens[loc]);
			loc++;
			if (tokens[loc].type != token::COMPARISON)
				throw tokeniseFailure("Expected a comparison operator");
			destline.push_back(tokens[loc]);
			loc++;
			if (tokens[loc].type == token::POSSIBLEFUNC)
				slurpTokens(loc, tokens, destline, true);
			else
				destline.push_back(tokens[loc]);
			if (tokens[loc + 1].type == token::COMPARISON) {
				loc++;
				destline.push_back(tokens[loc]);
			} else
				looping = false;
		}
	}

	for (int i = 0; i < c->notokens; i++) {
		loc++;
		// TODO: handle flow control
		if (tokens[loc].type == token::POSSIBLEFUNC)
			slurpTokens(loc, tokens, destline, true);
		else
			destline.push_back(tokens[loc]);
	}
}
	
caosScript::caosScript(std::istream &in) {
	std::vector<token> tokens;

	int lineno = 0;
	while (!in.fail()) {
		lineno++;
		std::string s;
		std::getline(in, s);
		if (s[s.size() - 1] == '\r')
			s.erase(s.end() - 1);
		try {
			tokeniseLine(s, tokens);
		} catch (tokeniseFailure f) {
			std::cerr << "failed to tokenise line #" << lineno << "(" << s << ") because: " << f.what() << "\n";
		}
	}

	unsigned int loc = 0;
	script *currscrip = &installer;
	for (unsigned int i = 0; loc != tokens.size(); i++) {
		std::list<token> destline;
		try {
			slurpTokens(loc, tokens, destline);
			if (destline.begin()->func->name == "SCRP") {
				assert(destline.size() == 5);
				int one, two, three, four;
				std::list<token>::iterator i = destline.begin();
				// TODO: shouldn't add scripts here, should store them for optional addition
				i++; assert(i->type == token::CAOSVAR); assert(i->var.hasInt()); one = i->var.intValue;
				i++; assert(i->type == token::CAOSVAR); assert(i->var.hasInt()); two = i->var.intValue;
				i++; assert(i->type == token::CAOSVAR); assert(i->var.hasInt()); three = i->var.intValue;
				i++; assert(i->type == token::CAOSVAR); assert(i->var.hasInt()); four = i->var.intValue;
				//std::cout << "caosScript: script " << one << " " << two << " " << three << " " << four
				//	<< " being added to scriptorium.\n";
				currscrip = &(world.scriptorium.getScript(one, two, three, four));
				// todo: verify event script doesn't already exist, maybe? don't know
				// what real engine does
			} else if (destline.begin()->func->name == "RSCR") {
				currscrip = &removal;
			} else if (destline.begin()->func->name == "ISCR") {
				currscrip = &installer;
			} else if (destline.begin()->func->name == "ENDM") {
				currscrip->lines.push_back(destline);
				currscrip = &installer;
			} else {
				currscrip->lines.push_back(destline);
			}
		} catch (tokeniseFailure f) {
			std::string firsttok = tokens[loc].dump();
			firsttok.erase(firsttok.end() - 1);
			std::cerr << f.what() << " at token '" << firsttok << "'\n [";
			if (loc == 1)
				std::cerr << tokens[0].dump();
			else if (loc > 1)
				std::cerr << tokens[loc - 2].dump() << tokens[loc - 1].dump();
			std::cerr << "{@}" << tokens[loc].dump();
			if (tokens.size() > (loc + 1))
				std::cerr << tokens[loc + 1].dump();
			std::cerr << "]\n";

			// make sure to zap the install script
			installer.lines.clear();
			return; // parse failure -> byebye
		}
		/*for (std::list<token>::iterator i = destline.begin(); i != destline.end(); i++) {
			std::cout << (*i).dump();
		}
		std::cout << "\n";*/

		loc++;
	}
}
