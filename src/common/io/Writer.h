#pragma once

#include "common/StringView.h"
#include "common/span.h"

#include <stddef.h>
#include <stdint.h>

/// base class for all Writers
class Writer {
  public:
	virtual ~Writer() = default;

	inline void write(const uint8_t*, size_t);
	inline void write(const char*, size_t);
	inline void write(span<const uint8_t>);
	inline void write(StringView);
	virtual void flush() = 0;

  protected:
	virtual void do_write(const uint8_t*, size_t) = 0;
};

inline void Writer::write(const uint8_t* buf, size_t n) {
	return do_write(buf, n);
}

inline void Writer::write(const char* buf, size_t n) {
	return write(reinterpret_cast<const uint8_t*>(buf), n);
}

inline void Writer::write(span<const uint8_t> buf) {
	return write(buf.data(), buf.size());
}

inline void Writer::write(StringView sv) {
	// don't want to take a span, because a span<const char> can be created
	// from a const char[] literal _containing the final nul character_, which
	// usually isn't what we want.
	return write(sv.data(), sv.size());
}