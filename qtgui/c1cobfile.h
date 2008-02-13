#ifndef C1COBFILE_H
#define C1COBFILE_H 1

#include <iostream>
#include <string>
#include "endianlove.h"
#include "streamutils.h"
#include <boost/scoped_array.hpp>
#include <vector>

std::string readpascalstring(std::istream &s);

struct c1cobfile {
	uint16 no_objects;
	uint32 expire_month;
	uint32 expire_day;
	uint32 expire_year;
	std::vector<std::string> scripts;
	std::vector<std::string> imports;
	uint16 no_objects_used;
	std::string name;

	uint32 imagewidth, imageheight;

	boost::scoped_array<char> imagedata;

	c1cobfile(std::ifstream &s);
};

#endif /* C1COBFILE_H */
