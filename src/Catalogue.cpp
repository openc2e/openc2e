/*
 *  Catalogue.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat 13 Nov 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005-2006 Bryan Donlan. All rights reserved.
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
#include "Catalogue.h"

// make sure we have the header imports that bison's horrible .h file needs
#include <cassert>
#include <string>
#include <list>

#include "catalogparser.h"
#include <ghc/filesystem.hpp>
#include <fmt/printf.h>
#include <iostream>
#include <sstream>

#include <cctype>

#include "utils/readfile.h"


namespace fs = ghc::filesystem;

Catalogue catalogue;

struct quote_subst {
	char escape;
	char subst;
};

static struct quote_subst subst_table[] = {
	{ 'n', '\n' },
	{ '\\', '\\' },
	{ '\"', '\"' },
	{ 't', '\t' },
	{ 0, 0 }
};

char catalogue_descape(char c) {
	struct quote_subst *qs = subst_table;
	while (qs->escape) {
		if (qs->escape == c)
			return qs->subst;
		qs++;
	}
	std::cerr << "Unmatched substitution: \\" << c << std::endl;
	return c;
}

static const char *tok_str[] = { "EOF", "word", "string", "integer" };

std::string cat_str;
int cat_int = -1;

Catalogue *parsing_cat = NULL;

void Catalogue::addVals(std::string &title, bool override, int count,
		const std::list<std::string> &vals)
{
	// TODO: how the heck does override work? DS has an "Option Text" tag which has to overwrite the C3 one, so commenting this out for now..
	/*if (data.find(title) != data.end() && !override)
		return; // XXX: ?*/
	data[title].clear();
//	copy(vals.begin(), vals.end(), data[title].begin());
	std::list<std::string>::const_iterator i = vals.begin();
	while(i != vals.end()) {
		data[title].push_back(*i++);
	}
}


extern int cataparse();

void Catalogue::catalogueParseError(const char *err) {
	std::ostringstream oss;
	oss << "Catalogue parse error at line " << yylineno;
	if (err)
		oss << ": " << err;
	
	throw catalogueException(oss.str());
}

std::istream &operator >> (std::istream &s, Catalogue &c) {
	std::string buf = readfile(s);
	Catalogue::yyinit(buf.c_str());
	parsing_cat = &c;
			
	cataparse();

	return s;
}

void Catalogue::reset() {
	data.clear();
}

void Catalogue::addFile(fs::path path) {
	assert(fs::exists(path));
	assert(!fs::is_directory(path));

	try {
		fs::ifstream f(path);
		f >> *this;
	} catch (const catalogueException &ex) {
		std::cerr << "Error reading catalogue file " << path.string() << ":" << std::endl << '\t' << ex.what() << std::endl;
	}
}

void Catalogue::initFrom(fs::path path, std::string language) {
	assert(fs::exists(path));
	assert(fs::is_directory(path));
	
	//std::cout << "Catalogue is reading " << path.string() << std::endl;

	fs::directory_iterator end;
	std::string file;
	for (fs::directory_iterator i(path); i != end; ++i) {
		try {
			if ((!fs::is_directory(*i)) && (i->path().extension().string() == ".catalogue")) {

				std::string x = i->path().stem().string();
				bool has_language_suffix = x.size() > 3 && x[x.size() - 3] == '-';

				if (has_language_suffix) {
					std::string language_suffix = x.substr(x.size() - 2);
					// TODO: case sensitive?
					if (language_suffix == language) {
						addFile(*i);
					}
				} else {
					auto suffixed_version = i->path().parent_path() / (i->path().stem().string() + "-" + language + ".catalogue");
					if (!fs::exists(suffixed_version)) {
						addFile(*i);
					}
				}
			}
		}
		catch (const std::exception &ex) {
			std::cerr << "directory_iterator died on '" << i->path().filename() << "' with " << ex.what() << std::endl;
		}
	}	
}

std::string stringFromInt(int i) {
	// TODO: hacky? also, put somewhere more appropriate
	return fmt::sprintf("%d", i);
}

const std::string Catalogue::getAgentName(unsigned char family, unsigned char genus, unsigned short species) const {
	std::string buf;
	buf = fmt::sprintf("Agent Help %d %d %d", (int)family, (int)genus, species);
	if (hasTag(buf)) {
		return getTag(buf)[0];
	} else {
		return "";
	}
}

std::string Catalogue::calculateWildcardTag(std::string tag, unsigned char family, unsigned char genus, unsigned short species) const {
	std::string searchstring = tag + " " + stringFromInt(family) + " " + stringFromInt(genus) + " " + stringFromInt(species);
	if (hasTag(searchstring)) return searchstring;
	if (species != 0) return calculateWildcardTag (tag, family, genus, 0);
	if (genus != 0) return calculateWildcardTag (tag, family, 0, 0);
	if (family != 0) return calculateWildcardTag (tag, 0, 0, 0);
	return "";
}

/* vim: set noet: */
