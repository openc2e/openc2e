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
	std::vector<std::string> &x = c.data["dummyblah TODO fix this crap"];

	std::string i;
	while (getline(s, i)) {
		// std::cout << "Catalogue >> got " << i << std::endl;
		bool wasparsingstring = false, parsingstring = false, parsingtag = false, parsingarray = false;
		unsigned int stage = 0;
		std::string t;
		if (i[i.size() - 1] == '\r') i.erase(i.size() - 1);
		// TODO: \\ -> \, anything else?

		boost::char_separator<char> sep(" ");
		boost::tokenizer<boost::char_separator<char> > tok(i, sep);
		for (boost::tokenizer<boost::char_separator<char> >::iterator b = tok.begin(); b != tok.end(); b++) {
			std::string y = *b;

			// std::cout << "Catalogue tokenizer got " << y << std::endl;

			if (y[0] == '#') {
				break;
			}

			// assert((!wasparsingstring) || (stage == 2));

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
					continue;
				}
				std::string r = y;
				r.erase(r.begin());
				if (parsingtag || parsingarray) {
					assert(stage == 1);
				}
				parsingstring = true;
				if (y[y.size() - 1] == '"') { r.erase(r.size() - 1); parsingstring = false; wasparsingstring = true; }
				t += r;
			} else if (y[y.size() - 1] == '"') {
				if (y[y.size() - 2] == '\\') { t += y; continue; }
				std::string r = y;
				r.erase(r.size() - 1);
				if (parsingtag || parsingarray) {
					assert(stage == 1);
					if (parsingarray) stage = 2;
				}
				assert(parsingstring);
				parsingstring = false;
				wasparsingstring = true;
				t += " " + r;
				if ((!parsingtag) && (!parsingarray)) {
					std::cout << "catalogue appending value: " << t << std::endl;
					x.push_back(t);
				}
			} else if (stage == 2) {
				// check for a number, store it in arraysize..
				stage = 3;
			} else if (parsingstring) {
				t += " " + y;
			} else {
				std::cerr << "ERROR: didn't understand token '" << y << "' in: " << i << std::endl;
			}
		}

		if (parsingstring) {
			std::cerr << "ERROR: incomplete string was found in: " << i << std::endl; 
			// TODO: spit error message (incomplete string)
		} else if (wasparsingstring) {
			if (parsingarray || parsingtag) {
				// TODO: once we fill in arraysize, uncomment following				
				// if (parsingarray) assert(arraysize != 0);
				std::cout << "catalogue starting to parse tag/array: " << t << std::endl;
				x = c.data[t];
			}
		}

		// somewhere we need to check: if (arraysize != 0) assert(arraysize == x.size());
	}
}

void Catalogue::reset() {
	data.clear();
}

void Catalogue::initFrom(boost::filesystem::path path) {
	assert(fs::exists(path));
	assert(fs::is_directory(path));
	
	std::cout << "Catalogue is reading " << path.native_directory_string() << std::endl;

	fs::directory_iterator end;
	for (fs::directory_iterator i(path); i != end; ++i) {
		if ((!fs::is_directory(*i)) && (fs::extension(*i) == ".catalogue")) {
			// TODO: localisation
			std::cout << "Catalogue " << fs::basename(*i) << " being read" << std::endl;
			fs::ifstream f(*i);
			f >> *this;
		}
	}	
}

