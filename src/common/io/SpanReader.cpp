#include "SpanReader.h"

#include "common/io/IOException.h"
#include "common/span.h"

#include <stddef.h>
#include <stdexcept>
#include <stdint.h>
#include <string.h>
#include <vector>

SpanReader::SpanReader() {
}

SpanReader::SpanReader(span<const uint8_t> buf_)
	: buf(buf_) {
}

SpanReader::SpanReader(StringView sv)
	: buf(reinterpret_cast<const uint8_t*>(sv.data()), sv.size()) {
}

void SpanReader::seek_absolute(size_t n) {
	if (n > buf.size()) {
		throw IOException("bad seek");
	}
	pos = n;
}

void SpanReader::seek_relative(int64_t off) {
	static_assert(sizeof(size_t) >= sizeof(int64_t), "positive int64_t can fit into size_t");
	if (off >= 0) {
		// static_cast is safe because off is positive and fits into size_t.
		seek_absolute(pos + static_cast<size_t>(off));
	} else if (off < 0) {
		// -(off + 1) is safe because it's guaranteed to fit into an off_t (we
		// don't use the simpler -off because it might be the minimum value,
		// which can't fit into a positive).
		// static_cast is safe because -(off + 1) is positive and fits into size_t.
		// off_negated is mathematically: -(off + 1) + 1 = -off - 1 + 1 = -off.
		size_t off_negated = static_cast<size_t>(-(off + 1)) + 1;
		if (off_negated > pos) {
			throw IOException("bad seek");
		}
		// subtraction is safe because we know off_negated <= pos so the result
		// is positive or zero.
		seek_absolute(pos - off_negated);
	}
}

size_t SpanReader::tell() const {
	return pos;
}

bool SpanReader::has_data_left() {
	return pos < buf.size();
}

std::vector<uint8_t> SpanReader::read_to_end() {
	auto result = std::vector<uint8_t>(buf.data() + pos, buf.end());
	pos = buf.size();
	return result;
}

uint8_t SpanReader::peek_byte() {
	if (pos >= buf.size()) {
		throw IOException("bad peek");
	}
	return buf[pos];
}

void SpanReader::do_read(uint8_t* out, size_t n) {
	// buf.size() - pos is safe because it's guaranteed to be >= 0.
	if (n > buf.size() - pos) {
		throw IOException("bad read");
	}
	memcpy(out, buf.data() + pos, n);
	// addition is safe because we know that the result is <= buf.size().
	pos += n;
}