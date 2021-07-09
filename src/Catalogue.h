/*
 *  Catalogue.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat 13 Nov 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
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
#ifndef _CATALOGUE_H
#define _CATALOGUE_H

#include "creaturesException.h"

#include <ghc/filesystem.hpp>
#include <iostream>
#include <istream>
#include <map>
#include <string>
#include <vector>

class catalogueException : public creaturesException {
  public:
	catalogueException(const char* s) throw()
		: creaturesException(s) {}
	catalogueException(const std::string& s) throw()
		: creaturesException(s) {}
};

class Catalogue {
  protected:
	static int yylineno;
	static const char* catalogue_parse_p;
	static void yyinit(const char* buf);
	static int catalex();
	friend int cataparse();
	static void catalogueParseError(const std::string& err);

  public:
	std::map<std::string, std::vector<std::string> > data;

  public:
	friend std::istream& operator>>(std::istream&, Catalogue&);

	const std::vector<std::string>& getTag(std::string t) const { return (*data.find(t)).second; }
	bool hasTag(std::string t) const { return (data.find(t) != data.end()); }
	const std::string getAgentName(unsigned char family, unsigned char genus, unsigned short species) const;
	std::string calculateWildcardTag(std::string tag, unsigned char family, unsigned char genus, unsigned short species) const;

	void reset();
	void addFile(ghc::filesystem::path path);
	void initFrom(ghc::filesystem::path path, std::string language);

	void addVals(std::string& title, bool override, const std::vector<std::string>& vals);
};

extern Catalogue catalogue;

#endif
/* vim: set noet: */
