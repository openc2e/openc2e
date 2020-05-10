#pragma once

#include <cstdint>
#include <vector>

#include "span.h"

class CharsetDtaReader {
public:
    CharsetDtaReader(span<const uint8_t> buf_);
    size_t getNumCharacters() const;
    size_t getCharWidth(size_t index) const;
    size_t getCharHeight(size_t) const;
    std::vector<uint8_t> getCharData(size_t index) const;
private:
    const span<const uint8_t> buf;
};