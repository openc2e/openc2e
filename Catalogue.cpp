#include "Catalogue.h"
#include "catalogue.lex.h"
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>
#include <iostream>

#include <cctype>

#include "openc2e.h"

using boost::str;

namespace fs = boost::filesystem;

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

catalogueFlexLexer *catalexer = NULL;
Catalogue *parsing_cat = NULL;

void Catalogue::addVals(std::string &title, bool override, int count,
		const std::list<std::string> &vals)
{
	if (data.find(title) != data.end() && !override)
		return; // XXX: ?
	data[title].clear();
//	copy(vals.begin(), vals.end(), data[title].begin());
	std::list<std::string>::const_iterator i = vals.begin();
	while(i != vals.end()) {
		data[title].push_back(*i++);
	}
}


extern int cataparse();

void cataerror(const char *err) {
	// XXX: we may leak here - memory pools?
	throw catalogueException(err);
}
void catalogueParseError(int lineno) {
	std::ostringstream oss;
	oss << "Catalogue parse error at line " << lineno << std::endl;
	
	throw catalogueException(oss.str());
}

std::istream &operator >> (std::istream &s, Catalogue &c) {
	catalogueFlexLexer lex(&s);
	catalexer = &lex;
	parsing_cat = &c;
			
	cataparse();

	catalexer = NULL;
	parsing_cat = NULL;

	return s;
}

void Catalogue::reset() {
	data.clear();
}

void Catalogue::initFrom(fs::path path) {
	assert(fs::exists(path));
	assert(fs::is_directory(path));
	
	std::cout << "Catalogue is reading " << path.native_directory_string() << std::endl;

	fs::directory_iterator end;
	std::string file;
	for (fs::directory_iterator i(path); i != end; ++i) {
		try {
			if ((!fs::is_directory(*i)) && (fs::extension(*i) == ".catalogue")) {
				file = (*i).string();
				std::string x = fs::basename(*i);
				// TODO: '-en-GB' exists too, this doesn't work for that
				if ((x.size() > 3) && (x[x.size() - 3] == '-')) {
					// TODO: this is NOT how we should do it
					continue; // skip all localised files
				}
				std::cout << "Catalogue file '" << x << "' being read" << std::endl;
				fs::ifstream f(*i);
				f >> *this;
			}
		}
		catch (const catalogueException &ex) {
			std::cerr << "Error reading catalogue file " << file << ":" << std::endl << '\t' << ex.what() << std::endl;
		}
		catch (const std::exception &ex) {
			std::cerr << "directory_iterator died on '" << i->leaf() << "' with " << ex.what() << std::endl;
		}
	}	
}

std::string stringFromInt(int i) {
	// TODO: hacky? also, put somewhere more appropriate
	
	char buffer[20]; // more than enough
	snprintf(buffer, 20, "%i", i);

	return std::string(buffer);
}

const std::string Catalogue::getAgentName(unsigned char family, unsigned char genus, unsigned short species) const {
	std::string buf;
	buf = str(boost::format("Agent Help %d %d %d") % (int)family % (int)genus % species);
	std::cerr << "cat " << buf << std::endl;
	if (hasTag(buf)) {
		return getTag(buf)[0];
	} else {
		return "";
	}
}

std::string Catalogue::calculateWildcardTag(std::string tag, unsigned char family, unsigned char genus, unsigned short species) const {
	std::string searchstring = tag + " " + stringFromInt(family) + " " + stringFromInt(genus) + " " + stringFromInt(species);
	if (hasTag(searchstring)) return searchstring;
	searchstring = tag + " " + stringFromInt(family) + " " + stringFromInt(genus) + " 0";
	if (hasTag(searchstring)) return searchstring;
	searchstring = tag + " " + stringFromInt(family) + " 0 0";
	if (hasTag(searchstring)) return searchstring;
	searchstring = tag + "0 0 0";
	if (hasTag(searchstring)) return searchstring;
	return "";
}

/* vim: set noet: */
