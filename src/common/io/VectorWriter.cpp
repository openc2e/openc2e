#include "VectorWriter.h"

#include <stddef.h>
#include <stdint.h>
#include <vector>

void VectorWriter::flush() {
}

const std::vector<uint8_t>& VectorWriter::vector() const {
	return buf;
}

void VectorWriter::do_write(const uint8_t* out, size_t n) {
	buf.insert(buf.end(), out, out + n);
}