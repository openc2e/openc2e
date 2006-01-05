#ifndef _CATALOGUE_H
#define _CATALOGUE_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <istream>
#include <boost/filesystem/path.hpp>
#include <list>
#include "exceptions.h"

class catalogueException : public creaturesException {
	public:
		catalogueException(const char *s) throw()
			: creaturesException(s) {}
		catalogueException(const std::string &s) throw()
			: creaturesException(s) {}
};	

class Catalogue {
//protected:
public:
	std::map<std::string, std::vector<std::string> > data;

public:
	friend std::istream &operator >> (std::istream &, Catalogue &);

	const std::vector<std::string> &getTag(std::string t) const { return (*data.find(t)).second; }
	bool hasTag(std::string t) const { return (data.find(t) != data.end()); }
	const std::string getAgentName(unsigned char family, unsigned char genus, unsigned short species) const;
	std::string calculateWildcardTag(std::string tag, unsigned char family, unsigned char genus, unsigned short species) const;
  
	void reset();
	void initFrom(boost::filesystem::path path);

	void addVals(std::string &title, bool override, int count, const std::list<std::string> &vals);
};

/* For lexer/parser, don't use for anything else. */

char catalogue_descape(char c);

#endif
/* vim: set noet: */
