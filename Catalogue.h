#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <istream>
#include "boost/filesystem/path.hpp"

class Catalogue {
//protected:
public:
	std::map<std::string, std::vector<std::string> > data;

public:
	friend std::istream &operator >> (std::istream &, Catalogue &);

	const std::vector<std::string> &getTag(std::string t) const { return (*data.find(t)).second; }
	bool hasTag(std::string t) const { return (data.find(t) != data.end()); }
	const std::string getAgentName(unsigned char family, unsigned char genus, unsigned short species) const;
  
	void reset();
	void initFrom(boost::filesystem::path path);
};

