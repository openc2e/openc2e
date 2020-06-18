#include "endianlove.h"
#include "creaturesException.h"
#include "fileformats/charsetdta.h"

#include <string>

CharsetDtaReader::CharsetDtaReader(span<const uint8_t> buf_) : buf(buf_) {
	if (!(buf.size() == 9472 || buf.size() == 18944)) {
		throw creaturesException("Expected size of charset.dta file to be 9472 or 18944 - got " + std::to_string(buf.size()));
	}
}

size_t CharsetDtaReader::getNumCharacters() const {
	return buf.size() / 74;
}

size_t CharsetDtaReader::getCharWidth(size_t index) const {
	size_t pos = getNumCharacters() * 72 + index * 2;
	return read16le(buf.data() + pos);
}

size_t CharsetDtaReader::getCharHeight(size_t) const {
	return 12;
}

std::vector<uint8_t> CharsetDtaReader::getCharData(size_t index) const {
	const size_t width = getCharWidth(index);
	std::vector<uint8_t> data(12 * width);
	const size_t charstartpos = index * 72;
	size_t charpos = 0;
	for (size_t row = 0; row < 12; ++row) {
		size_t rowpos = charstartpos + 6 * row;
		std::copy(&buf[rowpos], &buf[rowpos + width], data.data() + charpos);
		charpos += width;
	}
	return data;
}