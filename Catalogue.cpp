#include "Catalogue.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem/convenience.hpp"
#include "openc2e.h"

namespace fs = boost::filesystem;

std::istream &operator >> (std::istream &, Catalogue &) {
	// TODO
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

