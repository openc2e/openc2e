/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "c1cobfile.h"
#include "endianlove.h"
#include <cassert>
#include <fstream>

std::string readpascalstring(std::istream &s) {
	uint16_t size;
	uint8_t a; s.read((char *)&a, 1);
	if (a == 255)
		size = read16le(s);
	else
		size = a;

	std::vector<char> x(size);
	//char x[size];
	s.read(x.data(), size);
	return std::string(x.data(), size);
}

c1cobfile::c1cobfile(std::ifstream &s) {
	s >> std::noskipws;

	uint16_t version = read16le(s);

	// TODO: mph
	if (version != 1) {
		//QMessageBox::warning(this, tr("Failed to open"), tr("Version %1 is not supported").arg((int)version));
		return;
	}

	no_objects = read16le(s);
	expire_month = read32le(s);
	expire_day = read32le(s);
	expire_year = read32le(s);
	uint16_t noscripts = read16le(s);
	uint16_t noimports = read16le(s);
	no_objects_used = read16le(s);
	uint16_t reserved_zero = read16le(s);
	assert(reserved_zero == 0);

	for (unsigned int i = 0; i < noscripts; i++)
		scripts.push_back(readpascalstring(s));
	for (unsigned int i = 0; i < noimports; i++)
		imports.push_back(readpascalstring(s));

	imagewidth = read32le(s);
	imageheight = read32le(s);
	uint16_t secondimagewidth = read16le(s);
	if (imagewidth != secondimagewidth && secondimagewidth) // ABK- Egg Gender.cob has it zeroed
		std::cout << "ignoring COB secondimage width " << (int)secondimagewidth <<
			", using width " << imagewidth << std::endl;
	imagedata.clear();
	imagedata.resize(imagewidth * imageheight);
	s.read(imagedata.data(), imagewidth * imageheight);
	name = readpascalstring(s);
}
