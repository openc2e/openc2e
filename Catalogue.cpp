#include "Catalogue.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem/convenience.hpp"
#include "boost/tokenizer.hpp"

#include "openc2e.h"

namespace fs = boost::filesystem;

std::istream &operator >> (std::istream &s, Catalogue &c) {
	unsigned int arraysize = 0;
	std::vector<std::string> *x = 0;

	std::string i;
	while (std::getline(s, i)) {
		// std::cout << "Catalogue >> got " << i << std::endl;
		bool wasparsingstring = false, parsingstring = false, parsingtag = false, parsingarray = false;
		unsigned int stage = 0;
		std::string t;
		if (i[i.size() - 1] == '\r') i.erase(i.size() - 1);
		// TODO: \\ -> \, handle \n, anything else?

		boost::char_separator<char> sep(" \t");
		boost::tokenizer<boost::char_separator<char> > tok(i, sep);
		for (boost::tokenizer<boost::char_separator<char> >::iterator b = tok.begin(); b != tok.end(); b++) {
			std::string y = *b;

			// std::cout << "Catalogue tokenizer got " << y << std::endl;

			if ((y[0] == '#') || (y[0] == '*')) {
				break;
			}

			if (parsingarray) assert(stage < 3);
			if (parsingtag) assert(stage < 2);
			
			if (y == "OVERRIDE") continue; // TODO: handle this correctly, maybe (it's a DS thing) - fuzzie
			
			if (y == "TAG") {
				assert(!parsingtag);
				assert(!parsingarray);
				assert(stage == 0);
				parsingtag = true;
				stage = 1;
			} else if (y == "ARRAY") {
				assert(!parsingtag);
				assert(!parsingarray);
				assert(stage == 0);
				parsingarray = true;
				stage = 1;
			} else if (y[0] == '"') {
				if (y.size() == 1) {
					if (parsingstring) { parsingstring = false; wasparsingstring = true; }
					else parsingstring = true;
				} else {
					std::string r = y;
					r.erase(r.begin());
					if (parsingtag || parsingarray) {
						assert(stage == 1);
					}
					parsingstring = true;
					if (y[y.size() - 1] == '"') { r.erase(r.size() - 1); parsingstring = false; wasparsingstring = true; }
					t += r;
				}
			} else if (y[y.size() - 1] == '"') {
				if (y[y.size() - 2] == '\\') t += y;
				else {
					std::string r = y;
					r.erase(r.size() - 1);
					assert(parsingstring);
					parsingstring = false;
					wasparsingstring = true;
					t += " " + r;
				}
			} else if (stage == 2) {
				// check for a number, check it isn't zero, store it in arraysize..
				stage = 3;
			} else if (parsingstring) {
				t += " " + y;
			} else {
				std::cerr << "ERROR: didn't understand token '" << y << "' in: " << i << std::endl;
			}

			if (wasparsingstring && ((!parsingarray) && (!parsingtag))) {
//				std::cout << "catalogue appending value: " << t << std::endl;
				wasparsingstring = false;
				assert(x != 0);
				x->push_back(t);
				t.clear();
			} else if (wasparsingstring) {
				assert(stage == 1);
				stage = 2;
				// std::cout << "catalogue starting to parse tag/array: " << t << std::endl;
				x = &(c.data[t]);
				wasparsingstring = false;
			}
		}

		if (parsingstring) {
			std::cerr << "ERROR: incomplete string was found in: " << i << std::endl; 
			// TODO: spit error message (incomplete string)
		}

		// somewhere we need to check: if (arraysize != 0) assert(arraysize == x->size());
	}
	
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
	for (fs::directory_iterator i(path); i != end; ++i) {
		try {
			if ((!fs::is_directory(*i)) && (fs::extension(*i) == ".catalogue")) {
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
		} catch (const std::exception &ex) {
			std::cerr << "directory_iterator died on '" << i->leaf() << "' with " << ex.what() << std::endl;
		}
	}	
}

const std::string Catalogue::getAgentName(unsigned char family, unsigned char genus, unsigned short species) const {
	char buf[26]; // 11 + (3 + 1) + (3 + 1) + (5 + 1) + 1
	sprintf(buf, "Agent Help %i %i %i", family, genus, species);
	if (hasTag(buf)) {
		return getTag(buf)[0];
	} else {
		return "";
	}
}
/* vim: set noet: */
