#include "readfile.h"

#include "common/io/FileReader.h"
#include "common/io/Reader.h"

std::string readfile(const std::string& filename) {
	FileReader in(filename);
	return readfile(in);
}

std::string readfile(Reader& in) {
	std::vector<uint8_t> data = in.read_to_end();
	return std::string(reinterpret_cast<char*>(data.data()), data.size());
}

std::vector<uint8_t> readfilebinary(const std::string& filename) {
	return FileReader(filename).read_to_end();
}