#include "c1cobfile.h"

std::string readpascalstring(std::istream &s) {
	uint16 size;
	uint8 a; s.read((char *)&a, 1);
	if (a == 255)
		size = read16(s);
	else
		size = a;

	boost::scoped_array<char> x(new char[size]);
	//char x[size];
	s.read(x.get(), size);
	return std::string(x.get(), size);
}

c1cobfile::c1cobfile(std::ifstream &s) {
	s >> std::noskipws;

	uint16 version = read16(s);

	// TODO: mph
	if (version != 1) {
		//QMessageBox::warning(this, tr("Failed to open"), tr("Version %1 is not supported").arg((int)version));
		return;
	}

	no_objects = read16(s);
	expire_month = read32(s);
	expire_day = read32(s);
	expire_year = read32(s);
	uint16 noscripts = read16(s);
	uint16 noimports = read16(s);
	no_objects_used = read16(s);
	uint16 reserved_zero = read16(s);
	assert(reserved_zero == 0);

	for (unsigned int i = 0; i < noscripts; i++)
		scripts.push_back(readpascalstring(s));
	for (unsigned int i = 0; i < noimports; i++)
		imports.push_back(readpascalstring(s));

	imagewidth = read32(s);
	imageheight = read32(s);
	uint16 secondimagewidth = read16(s);
	assert(imagewidth == secondimagewidth);
	imagedata.reset(new char[imagewidth * imageheight]);
	s.read(imagedata.get(), imagewidth * imageheight);
	name = readpascalstring(s);
}
