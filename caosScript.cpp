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
#include <iostream>
#include <sstream>

std::string token::dump() {
	if (comparison == NONE) return (isvar ? var.dump() : ((cmd == 0) ? ((func == 0) ? "[bad command!] " : func->dump()) : cmd->dump()));
	else {
		switch (comparison) {
			case EQ: return "EQ ";
			case NE: return "NE ";
			case GT: return "GT ";
			case GE: return "GE ";
			case LT: return "LT ";
			case LE: return "LE ";
			case AND: return "AND ";
			case OR: return "OR ";
			default: return "[bad comparison!] ";
		}
	}
}

inline std::string stringify(double x) {
	std::ostringstream o;
	if (!(o << x)) throw "stringify() failed";
	return o.str();
}

std::string caosVar::dump() {
	std::string out = "[caosVar: ";
	if (hasString()) {
		out += std::string("\"") + stringValue + "\"";
	} else if (hasInt()) {
		out += stringify(intValue);
		out += "(i)";
	} else if (hasFloat()) {
		out += stringify(floatValue);
		out += "(f)";
	} else {
		out += "unknown flags: ";
		out += flags;
	}
	out += "] ";
	return out;
}

std::string cmdinfo::dump() {
	return std::string("[command: ") + name + "] ";
}

#define _SHOW_RAWLINES
std::string caosScript::dump() {
	std::string out;
	for (unsigned int i = 0; i < lines.size(); i++) {
#ifdef _SHOW_RAWLINES
		out += std::string("[rawline: ") + rawlines[i] + "]\n";
#endif
		for (std::list<token>::iterator j = lines[i].begin(); j != lines[i].end(); j++) {
			out += j->dump();
		}
#ifdef _SHOW_RAWLINES
		out += "\n\n";
#else
		out += "\n";
#endif
	}
	return out;
}

token makeToken(std::string &src, bool str, token &lasttok) {
	token r;
	if (str) { // handle strings (and arrays, at present)
		r.var.setString(src);
	} else if ((isdigit(src[0])) || (src[0] == '.') || (src[0] == '-')) { // handle digits
		if (src.find(".") == std::string::npos) r.var.setInt(atoi(src.c_str()));
		else r.var.setFloat(atof(src.c_str()));
//		if (errno == EINVAL) throw tokeniseFailure();
		// TODO: check for zero return value from atoi (and atof?) and then check for EINVAL
	} else if (src.size() == 4) { // handle commands
		r.isvar = false;
		cmdinfo *lastcmd = (lasttok.cmd ? lasttok.cmd : lasttok.func);
		if ((!lasttok.isvar) && (lastcmd->twotokens)) {
			if (lasttok.cmd) r.cmd = getSecondCmd(lastcmd, src, true);
			else r.func = getSecondCmd(lastcmd, src, false);
		} else {
			r.cmd = getCmdInfo(src, true);
			r.func = getCmdInfo(src, false);
		}
		if (!r.cmd && !r.func) throw tokeniseFailure();
	} else { // presumably we have a comparison
		transform(src.begin(), src.end(), src.begin(), toupper);
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
		else throw tokeniseFailure();
	}
	return r;
}

void tokenise(std::string s, std::list<token> &t) {
	std::string currtoken;
	token lasttoken;
	for (std::string::iterator i = s.begin(); i != s.end(); i++) {
		if ((*i == ' ') || (*i == '\t') || (*i == '\r')) {
			if (!currtoken.empty()) {
				lasttoken = makeToken(currtoken, false, lasttoken);
				if ((lasttoken.isvar) || (lasttoken.cmd ? !lasttoken.cmd->twotokens : !lasttoken.func->twotokens))
					t.push_back(lasttoken);
				currtoken.clear();
			}
		} else if (*i == '"') {
			if (!currtoken.empty()) throw tokeniseFailure();
			i++; // skip the "
			while (*i != '"') {
				currtoken += *i;
				i++;
				if (i == s.end()) throw tokeniseFailure();
			}
			t.push_back(makeToken(currtoken, true, lasttoken));
			currtoken.clear();
		} else if (*i == '[') {
			if (!currtoken.empty()) throw tokeniseFailure();
			i++; // skip the [
			while (*i != ']') {
				currtoken += *i;
				i++;
				if (i == s.end()) throw tokeniseFailure();
			}
			t.push_back(makeToken(currtoken, true, lasttoken));
			currtoken.clear();
		} else {
			currtoken += *i;
		}
	}
	if (!currtoken.empty()) {
		t.push_back(makeToken(currtoken, false, lasttoken));
	}
}

caosScript::caosScript(std::istream &in) {
	std::vector<std::list<token> > lines;
	std::vector<std::string> rawlines;

	while (!in.fail()) {
		std::list<token> t;
		std::string s;
		std::getline(in, s);
		try {
			tokenise(s, t);
			if (!t.empty()) {
				lines.push_back(t);
				rawlines.push_back(s);
			}
		} catch (tokeniseFailure f) {
			// we should throw out an error here, but for the purposes of testing..
			rawlines.push_back("BROKEN: " + s);
			lines.push_back(t);
		}
	}

	installer.lines = lines;
	installer.rawlines = rawlines;
	/*
		todo: go through the lines, find scrp / rscr tokens, rip them out
	*/
}
