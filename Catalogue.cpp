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
	std::vector<std::string> &x = *((std::vector<std::string> *)0);
	bool live = false;

	std::string i;
	while (getline(s, i)) {
		bool parsingstring = false, parsingtag = false, parsingarray = false;
		unsigned int stage = 0;
		std::string t;

		boost::tokenizer<> tok(i);
		for (boost::tokenizer<>::iterator b = tok.begin(); b != tok.end(); ++b) {
			std::string y = *b;

			if (y == "TAG") {
				assert(!parsingtag);
				assert(!parsingarray);
				assert(!live);
				assert(stage == 0);
				parsingtag = true;
				stage = 1;
			} else if (y == "ARRAY") {
				assert(!parsingtag);
				assert(!parsingarray);
				assert(!live);
				assert(stage == 0);
				parsingarray = true;
				stage = 1;
			} else if (y[0] == '"') {
				std::string r = y;
				y.erase(r.begin());
				if (parsingtag || parsingarray) {
					assert(stage == 1);
				} else {
					assert(live);
					assert(stage == 0);
					stage = 1;
					parsingstring = true;
				}
				t += r;
			} else if (y[y.size() - 1] == '"') {
				std::string r = y;
				y.erase(r.end() - 1);
				if (parsingtag || parsingarray) {
					assert(stage == 1);
					assert(parsingstring);
				} else {
					assert(live);
					assert(parsingstring);
					parsingstring = false;
				}
				t += r;
			} else if (stage == 2) {
				// check for a number, store it in arraysize..
			} else if (parsingstring) {
				t += y;
			} else {
				// TODO: spit error message
			}
		}

		if (parsingarray || parsingtag) {
			if (parsingarray) assert(arraysize != 0);
			std::cout << "catalogue starting to parse tag/array: " << t << std::endl;
			x = c.data[t];
			live = true;
		} else if (parsingstring) {
			std::cout << "catalogue appending value: " << t << std::endl;
			x.push_back(t);
		} else {
			live = false;
			assert(arraysize == x.size());
		} 
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
		if ((!fs::is_directory(*i)) && (fs::extension(*i) == "catalogue")) {
			std::cout << "Catalogue " << fs::basename(*i) << " being read" << std::endl;
			fs::ifstream f(*i);
			f >> *this;
		}
	}	
}

