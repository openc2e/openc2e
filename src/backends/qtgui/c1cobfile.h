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

#ifndef C1COBFILE_H
#define C1COBFILE_H 1

#include <iostream>
#include <string>
#include "endianlove.h"
#include "streamutils.h"
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

	std::vector<char> imagedata;

	c1cobfile(std::ifstream &s);
};

#endif /* C1COBFILE_H */
