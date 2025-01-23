#pragma once

#include "common/span.h"

#include <stddef.h>
#include <stdint.h>
#include <vector>

/// base class for all Readers
class Reader {
  public:
	virtual ~Reader() = default;

	virtual bool has_data_left() = 0;
	void read(uint8_t*, size_t);
	void read(char*, size_t);
	void read(span<uint8_t>);
	virtual std::vector<uint8_t> read_to_end() = 0;
	virtual void seek_absolute(size_t) = 0;
	virtual void seek_relative(int64_t) = 0;

	// TODO: should tell() return an off_t, similar to the underlying
	// ftell() call? that might be better if users are often doing
	// subtraction on the result anyways.
	virtual size_t tell() const = 0;

	// ugh, this is stupid. what would be _really_ nice is an interface
	// like peek(size_t) -> span<uint8_t>, and modify FileReader and
	// FileWriter to expose the underlying buffer directly when that
	// happens.
	virtual uint8_t peek_byte() = 0;

  protected:
	virtual void do_read(uint8_t*, size_t) = 0;
};

inline void Reader::read(uint8_t* out, size_t n) {
	return do_read(out, n);
}

inline void Reader::read(char* out, size_t n) {
	return read(reinterpret_cast<uint8_t*>(out), n);
}

inline void Reader::read(span<uint8_t> out) {
	return read(out.data(), out.size());
}