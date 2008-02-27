#include "util.h"
#include "exceptions.h"
#include <sstream>

std::string readfile(std::istream &in) {
	std::ostringstream buf;
	while (in.good()) {
		char tempbuf[512];
		in.get(tempbuf, sizeof tempbuf, '\0');
		buf << tempbuf;
		if (!in.good())
			break;
		if (in.bad())
			throw creaturesException("IO error");
	}
	return buf.str();
}
