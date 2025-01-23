#pragma once

#include "common/io/Writer.h"

#include <stddef.h>
#include <stdint.h>
#include <string>

/*

A Writer implementation over an internal std::string, similar
to std::stringstream.

*/

class StringWriter final : public Writer {
  public:
	void flush() override;
	const std::string& string() const;

  private:
	void do_write(const uint8_t* out, size_t n) override;
	std::string buf;
};
