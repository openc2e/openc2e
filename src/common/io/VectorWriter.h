#pragma once

#include "common/io/Writer.h"

#include <stddef.h>
#include <stdint.h>
#include <vector>

/*

A Writer implementation over an internal std::vector<uint8_t>,
similar to std::stringstream but for bytes.

*/

class VectorWriter final : public Writer {
  public:
	void flush() override;
	const std::vector<uint8_t>& vector() const;

  private:
	void do_write(const uint8_t* out, size_t n) override;
	std::vector<uint8_t> buf;
};
