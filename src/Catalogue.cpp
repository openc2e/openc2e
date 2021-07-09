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
#include "catalogparser.h"
#include "utils/readfile.h"

#include <cassert>
#include <cctype>
#include <fmt/core.h>
#include <ghc/filesystem.hpp>
#include <iostream>
#include <list>
#include <string>


namespace fs = ghc::filesystem;

Catalogue catalogue;

std::string cat_str;
int cat_int = -1;

Catalogue* parsing_cat = NULL;

void Catalogue::addVals(std::string& title, bool override, const std::vector<std::string>& vals) {
	// TODO: how the heck does override work?
	// There seem to be three groups of conflicting tags:
	// (1) Tags with the same value. Some are marked OVERRIDE, some aren't.
	// (2) Tags in Docking Station that have additional values on the end. None of these are marked OVERRIDE.
	// (3) Tags with OVERRIDE. These only appear in Docking Station.

	(void) override;
	/*if (data.find(title) != data.end() && !override)
		return; // XXX: ?*/
	data[title].clear();
	//	copy(vals.begin(), vals.end(), data[title].begin());
	data[title] = vals;
}


extern int cataparse();

void Catalogue::catalogueParseError(const std::string& err) {
	throw catalogueException(fmt::format("Catalogue parse error at line {}: {}", yylineno, err));
}

std::istream& operator>>(std::istream& s, Catalogue& c) {
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
		std::ifstream f(path);
		f >> *this;
	} catch (const catalogueException& ex) {
		std::cerr << "Error reading catalogue file " << path.string() << ":" << std::endl
				  << '\t' << ex.what() << std::endl;
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
		} catch (const std::exception& ex) {
			std::cerr << "directory_iterator died on '" << i->path().filename() << "' with " << ex.what() << std::endl;
		}
	}
}

const std::string Catalogue::getAgentName(unsigned char family, unsigned char genus, unsigned short species) const {
	std::string buf = fmt::format("Agent Help {} {} {}", (int)family, (int)genus, species);
	if (hasTag(buf)) {
		return getTag(buf)[0];
	} else {
		return "";
	}
}

std::string Catalogue::calculateWildcardTag(std::string tag, unsigned char family, unsigned char genus, unsigned short species) const {
	std::string searchstring = fmt::format("{} {} {} {}", tag, (int)family, (int)genus, species);
	if (hasTag(searchstring))
		return searchstring;
	if (species != 0)
		return calculateWildcardTag(tag, family, genus, 0);
	if (genus != 0)
		return calculateWildcardTag(tag, family, 0, 0);
	if (family != 0)
		return calculateWildcardTag(tag, 0, 0, 0);
	return "";
}

/* vim: set noet: */
