#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <istream>
#include "boost/filesystem/path.hpp"

class Catalogue {
protected:
	std::map<std::string, std::vector<std::string> > data;

public:
	friend std::istream &operator >> (std::istream &, Catalogue &);

	const std::vector<std::string> &getTag(std::string t) const { return (*data.find(t)).second; }

	void reset();
	void initFrom(boost::filesystem::path path);
};

