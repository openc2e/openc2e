#include "endianlove.h"
#include <fstream> // don't actually need this

uint16 read16(std::istream &s) {
	uint16 t;
	s.read((char *)&t, 2);
	return swapEndianShort(t);
}

void write16(std::ostream &s, uint16 v) {
	uint16 t = swapEndianShort(v);
	s.write((char *)&t, 2);
}

uint32 read32(std::istream &s) {
	uint32 t;
	s.read((char *)&t, 4);
	return swapEndianLong(t);
}

void write32(std::ostream &s, uint32 v) {
	uint32 t = swapEndianLong(v);
	s.write((char *)&t, 4);
}

